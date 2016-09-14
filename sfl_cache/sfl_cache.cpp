// std
#include <iostream>
#include <exception>

// Boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

// sfl
#include <sfl/sequence_face_landmarks.h>
#include <sfl/utilities.h>

// vsal
#include <vsal/VideoStreamFactory.h>
#include <vsal/VideoStreamOpenCV.h>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::runtime_error;
using namespace boost::program_options;
using namespace boost::filesystem;

int main(int argc, char* argv[])
{
	// Parse command line arguments
	string inputPath, outputPath, landmarksModelPath;
	std::vector<float> frame_scales;
    unsigned int track;
	bool preview;
	try {
		options_description desc("Allowed options");
		desc.add_options()
			("help", "display the help message")
			("input,i", value<string>(&inputPath)->required(), "path to video sequence")
			("output,o", value<string>(&outputPath), "output path")
			("landmarks,l", value<string>(&landmarksModelPath)->required(), "path to landmarks model file")
			("scales,s", value<std::vector<float>>(&frame_scales)->default_value({ 1.0f }, "{1}"),
				"frame scales for finding small faces. Best scale will be selected")
			("track,t", value<unsigned int>(&track)->default_value(1), 
                "track faces across frames [0=NONE|1=BRISK|2=LBP]")
			("preview,p", value<bool>(&preview)->default_value(true), "preview landmarks")
			;
		variables_map vm;
		store(command_line_parser(argc, argv).options(desc).
			positional(positional_options_description().add("input", -1)).run(), vm);
		if (vm.count("help")) {
			cout << "Usage: cache_face_landmarks [options]" << endl;
			cout << desc << endl;
			exit(0);
		}
		notify(vm);
		if (!is_regular_file(landmarksModelPath)) throw error("landmarks must be a path to a file!");
	}
	catch (const error& e) {
		cout << "Error while parsing command-line arguments: " << e.what() << endl;
		cout << "Use --help to display a list of options." << endl;
		exit(1);
	}

	try
	{
		// Initialize Sequence Face Landmarks
		std::vector<std::shared_ptr<sfl::SequenceFaceLandmarks>> sfls(frame_scales.size());
		sfls[0] = sfl::SequenceFaceLandmarks::create(landmarksModelPath, frame_scales[0],
            (sfl::FaceTrackingType)track);
		for (int i = 1; i < frame_scales.size(); ++i)
		{
			sfls[i] = sfls[0]->clone();
			sfls[i]->setFrameScale(frame_scales[i]);
		}

		int max_faces = 0;
		std::shared_ptr<sfl::SequenceFaceLandmarks> best_sfl;

		// For each sfl configuration
		for (auto& sfl : sfls)
		{
			// Create video source
			vsal::VideoStreamFactory& vsf = vsal::VideoStreamFactory::getInstance();
			std::unique_ptr<vsal::VideoStreamOpenCV> vs(
				(vsal::VideoStreamOpenCV*)vsf.create(inputPath));
			if (vs == nullptr) throw runtime_error("No video source specified!");

			// Open video source
			if (!vs->open()) throw runtime_error("Failed to open video source!");

			// Main loop
			cv::Mat frame;
			int frameCounter = 0, faceCounter = 0;
			while (vs->read())
			{
				if (!vs->isUpdated()) continue;

				frame = vs->getFrame();
				const sfl::Frame& landmarks_frame = sfl->addFrame(frame);
                faceCounter += landmarks_frame.faces.size();

				if (preview)
				{
					// Render landmarks
					sfl::render(frame, landmarks_frame);

					// Render overlay
					string msg = "Frame count: " + std::to_string(++frameCounter);
					cv::putText(frame, msg, cv::Point(15, 15),
						cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
					msg = "Faces found so far: " + std::to_string(faceCounter);
					cv::putText(frame, msg, cv::Point(15, 40),
						cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
					msg = (boost::format("Current frame scale: %.1f") % sfl->getFrameScale()).str();
					cv::putText(frame, msg, cv::Point(15, 65),
						cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
					msg = "Tracking: " + std::string(track ? "Enabled" : "Disabled");
					cv::putText(frame, msg, cv::Point(15, 90),
						cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
					
					cv::putText(frame, "press any key to stop", cv::Point(10, frame.rows - 20),
						cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);

					// Show frame
					cv::imshow("sfl_cache", frame);
					int key = cv::waitKey(1);
					if (key >= 0) break;
				}
			}

			if (faceCounter > max_faces || !best_sfl)
			{
				max_faces = faceCounter;
				best_sfl = sfl;
			}
		}
		
		if (best_sfl)
		{
			// Set output path
			path input = path(inputPath);
			if (outputPath.empty()) outputPath =
				(input.parent_path() / (input.stem() += ".lms")).string();
			else if (is_directory(outputPath)) outputPath =
				(path(outputPath) / (input.stem() += ".lms")).string();

			// Saving to file
			cout << "Best scale: " << (boost::format("%.1f") % best_sfl->getFrameScale()).str() << endl;
			cout << "Total faces found: " + std::to_string(max_faces) << endl;
			cout << "Saving landmarks to \"" << outputPath << "\"." << endl;
			best_sfl->save(outputPath);
		}
	}
	catch (std::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}

	return 0;
}

