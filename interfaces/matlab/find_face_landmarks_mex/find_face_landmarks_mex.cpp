#include "MxArray.hpp"

// std
#include <vector>
#include <string>
#include <exception>

// Boost
#include <boost/filesystem.hpp>

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

// Matlab
#include <mex.h>

// Namespaces
using std::vector;
using std::string;
using std::runtime_error;
using namespace boost::filesystem;

#define printfFnc(...) { mexPrintf(__VA_ARGS__); mexEvalString("drawnow;");}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	try
	{
		// Parse arguments
		std::string inputPath, landmarksModelPath, landmarksPath;
		int device = -1;
		int width = 0, height = 0;
        int track = 1;
		float frame_scale = 1.0f;
		bool preview = true;
		if (nrhs == 0) throw runtime_error("No parameters specified!");
		/*
		if (nrhs < 2) throw runtime_error("Invalid number of parameters!");

		
		if (!MxArray(prhs[0]).isChar()) throw runtime_error(
			"modelFile must be a string containing the path to the model file!");
		landmarksModelPath = MxArray(prhs[0]).toString();
		*/
		if (nrhs == 1)
		{
			if (!MxArray(prhs[0]).isChar()) throw runtime_error(
				"inputPath must be sequence path or a landmarks cache file (.pb)!");
			inputPath = MxArray(prhs[0]).toString();

			path input = path(inputPath);
			if (input.extension() == ".lms") landmarksPath = inputPath;
			else landmarksPath =
				(input.parent_path() / (input.stem() += ".lms")).string();
			if (!is_regular_file(landmarksPath))
				throw runtime_error("Couldn't find landmarks file!");
		}
		else if (MxArray(prhs[1]).isChar())			// Dataset
		{
			landmarksModelPath = MxArray(prhs[0]).toString();
			inputPath = MxArray(prhs[1]).toString();
			if (nrhs > 2) frame_scale = (float)MxArray(prhs[2]).toDouble();
			if (nrhs > 3) track = MxArray(prhs[3]).toInt();
            if (nrhs > 4) preview = MxArray(prhs[4]).toBool();

			// Check for landmarks file
			path input = path(inputPath);
			landmarksPath =
				(input.parent_path() / (input.stem() += ".lms")).string();
			if (is_regular_file(landmarksPath)) landmarksModelPath.clear();
			else landmarksPath.clear();
		}
		else if (MxArray(prhs[1]).isInt32() || MxArray(prhs[1]).isDouble())	// Live video
		{
			landmarksModelPath = MxArray(prhs[0]).toString();
			device = MxArray(prhs[1]).toInt();
			if (nrhs > 2) width = MxArray(prhs[2]).toInt();
			if (nrhs > 3) height = MxArray(prhs[3]).toInt();
			if (nrhs > 4) frame_scale = (float)MxArray(prhs[4]).toDouble();
            if (nrhs > 5) track = MxArray(prhs[3]).toBool();
		}
		else throw runtime_error("Second parameter must be either a sequence path or a device id!");

		// Initialize Sequence Face Landmarks
		std::shared_ptr<sfl::SequenceFaceLandmarks> sfl =
			sfl::SequenceFaceLandmarks::create(landmarksModelPath, frame_scale,
            (sfl::FaceTrackingType)track);

		if (landmarksPath.empty())
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

                // Matlab and OpenCV's GUI do not play well on other playforms
#ifdef _WIN32
				if (preview)
				{
					faceCounter += landmarks_frame.faces.size();

					// Render landmarks
					sfl::render(frame, landmarks_frame);

                    // Show overlay
                    string msg = "Frame count: " + std::to_string(++frameCounter);
                    cv::putText(frame, msg, cv::Point(15, 15),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
                    msg = "Face count: " + std::to_string(faceCounter);
                    cv::putText(frame, msg, cv::Point(15, 40),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);
                    cv::putText(frame, "press any key to stop", cv::Point(10, frame.rows - 20),
                        cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 102, 255), 1, CV_AA);

					// Show frame
					cv::imshow("find_face_landmarks", frame);
					int key = cv::waitKey(1);
					if (key >= 0) break;
				}
#endif  // _WIN32
			}
		}
		else sfl->load(landmarksPath);		

		///
		// Output results
		///
		const std::list<std::unique_ptr<sfl::Frame>>& sfl_frames = sfl->getSequence();

		// Create the frames as a 1-by-n array of structs.
		mwSize dims[2] = { 1, 1 };
		dims[1] = sfl_frames.size();
		const char *frame_fields[] = { "faces", "width", "height" };
		const char *face_fields[] = { "landmarks", "bbox" };
		plhs[0] = mxCreateStructArray(2, dims, 3, frame_fields);

		// For each frame
		std::list<std::unique_ptr<sfl::Frame>>::const_iterator it;
		size_t i = 0;
		for (it = sfl_frames.begin(); it != sfl_frames.end(); ++it, ++i)
		{
			const std::unique_ptr<sfl::Frame>& sfl_frame = *it;

			// Set the width and height to the fields of the current frame
			mxSetField(plhs[0], i, frame_fields[1], MxArray(sfl_frame->width));
			mxSetField(plhs[0], i, frame_fields[2], MxArray(sfl_frame->height));

			const std::list<std::unique_ptr<sfl::Face>>& faces = sfl_frame->faces;
			if (faces.empty()) continue;

			// Create the faces as a 1-by-n array of structs.
			dims[1] = faces.size();
			mxArray* facesStructArray = mxCreateStructArray(2, dims, 2, face_fields);

			// Set the faces to the field of the current frame
			mxSetField(plhs[0], i, frame_fields[0], facesStructArray);

			// For each face
			std::list<std::unique_ptr<sfl::Face>>::const_iterator face_it;
			size_t j = 0;
			for (face_it = faces.begin(); face_it != faces.end(); ++face_it, ++j)
			{
				const std::unique_ptr<sfl::Face>& face = *face_it;
				//const sfl::Face& face = sfl_frame->faces[j];

				// Convert the landmarks to Matlab's pixel format
				cv::Mat_<int> landmarks(face->landmarks.size(), 2, (int*)face->landmarks.data());
				landmarks += 1;

				// Set the landmarks to the field of the current face
				mxSetField(facesStructArray, j, face_fields[0], MxArray(landmarks));

				// Convert the bounding box to Matlab's pixel format
				cv::Mat bbox = (cv::Mat_<int>(1, 4) <<
					face->bbox.x + 1, face->bbox.y + 1, face->bbox.width, face->bbox.height);

				// Set the bounding to the field of the current face
				mxSetField(facesStructArray, j, face_fields[1], MxArray(bbox));
			}
		}

		// Cleanup
		cv::destroyWindow("find_face_landmarks");
	}
	catch (std::exception& e)
	{
		mexErrMsgIdAndTxt("find_face_landmarks:parsing", "Error: %s", e.what());
	}
}
