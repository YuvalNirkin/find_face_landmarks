// std
#include <iostream>
#include <exception>

// Boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// sfl
#include <sfl/sequence_face_landmarks.h>

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
	float frame_scale;
	bool preview;
	try {
		options_description desc("Allowed options");
		desc.add_options()
			("help", "display the help message")
			("input,i", value<string>(&inputPath)->required(), "path to video sequence")
			("output,o", value<string>(&outputPath), "output path")
			("landmarks,l", value<string>(&landmarksModelPath)->required(), "path to landmarks model file")
			("scale,s", value<float>(&frame_scale)->default_value(1.0f), "frame scale for finding small faces")
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
		std::shared_ptr<sfl::SequenceFaceLandmarks> sfl =
			sfl::SequenceFaceLandmarks::create(landmarksModelPath, frame_scale);

		// Create video source
		vsal::VideoStreamFactory& vsf = vsal::VideoStreamFactory::getInstance();
		std::unique_ptr<vsal::VideoStreamOpenCV> vs(
			(vsal::VideoStreamOpenCV*)vsf.create(inputPath));
		if (vs == nullptr) throw runtime_error("No video source specified!");

		// Open video source
		if (!vs->open()) throw runtime_error("Failed to open video source!");

		// Main loop
		cv::Mat frame;
		int faceCounter = 0;
		while (vs->read())
		{
			if (!vs->isUpdated()) continue;

			frame = vs->getFrame();
			const sfl::Frame& landmarks_frame = sfl->addFrame(frame);
			
			if (preview)
			{
				faceCounter += landmarks_frame.faces.size();

				// Render landmarks
				sfl::render(frame, landmarks_frame);

				// Render overlay
				string msg = "Faces found so far: " + std::to_string(faceCounter);
				cv::putText(frame, msg, cv::Point(15, 15),
					cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1, CV_AA);
				cv::putText(frame, "press any key to stop", cv::Point(10, frame.rows - 20),
					cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 255), 1, CV_AA);

				// Show frame
				cv::imshow("cache_face_landmarks", frame);
				int key = cv::waitKey(1);
				if (key >= 0) break;
			}
		}

		// Set output path
		path input = path(inputPath);
		if (outputPath.empty()) outputPath = 
			(input.parent_path() / (input.stem() += "_landmarks.pb")).string();
		else if (is_directory(outputPath))	outputPath = 
			(path(outputPath) / (input.stem() += "_landmarks.pb")).string();

		// Saving to file
		cout << "Saving landmarks to \"" << outputPath << "\"." << endl;
		sfl->save(outputPath);
	}
	catch (std::exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}

	return 0;
}

