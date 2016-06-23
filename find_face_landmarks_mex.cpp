/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "find_face_landmarks.h"
#include <vector>
#include <string>
#include <exception>
#include <mex.h>
#include "MxArray.hpp"
#include <opencv2/imgproc.hpp>

/************************************************************************************
*									  Namespaces									*
************************************************************************************/
using std::vector;
using std::string;
using std::runtime_error;

/************************************************************************************
*									 Declarations									*
************************************************************************************/
#define printfFnc(...) { mexPrintf(__VA_ARGS__); mexEvalString("drawnow;");}

/************************************************************************************
*									Implementation									*
************************************************************************************/

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    try
    {
        vector<dlib::Frame> frames;

        // Parse input
        if (nrhs == 0) throw runtime_error("No parameters specified!");
        if (!MxArray(prhs[0]).isChar()) throw runtime_error(
            "modelFile must be a string containing the path to the model file!");
        string modelFile = MxArray(prhs[0]).toString();
        if (nrhs == 2 || nrhs == 3)
        {
            if (!MxArray(prhs[1]).isChar()) throw runtime_error(
                "inputPath must be a string containing the path a video file"
                " or a directory of images!");
            string inputPath = MxArray(prhs[1]).toString();
            if (nrhs == 3 && MxArray(prhs[2]).isDouble()) 
                dlib::frame_scale = MxArray(prhs[2]).toDouble();
            dlib::find_face_landmarks(modelFile, inputPath, frames);
        }
        else if (nrhs == 4 || nrhs == 5)
        {
            int device = MxArray(prhs[1]).toInt();
            int width = MxArray(prhs[2]).toInt();
            int height = MxArray(prhs[3]).toInt();
            if (nrhs == 5 && MxArray(prhs[4]).isDouble())
                dlib::frame_scale = MxArray(prhs[4]).toDouble();
            dlib::find_face_landmarks(modelFile, device, width, height, frames);
        }
        else throw runtime_error("Invalid number of parameters!");

        if (frames.empty()) throw runtime_error("Failed to read from video source!");

        ///
        // Output results
        ///

        // Create the frames as a 1-by-n array of structs.
        mwSize dims[2] = { 1, 1 };
        dims[1] = frames.size();
        const char *frame_fields[] = { "faces", "width", "height" };
        const char *face_fields[] = { "landmarks", "bbox" };
        plhs[0] = mxCreateStructArray(2, dims, 3, frame_fields);

        // For each frame
        for (size_t i = 0; i < frames.size(); ++i)
        {
            dlib::Frame& frame = frames[i];
            if (frame.faces.empty()) continue;

            // Create the faces as a 1-by-n array of structs.
            dims[1] = frame.faces.size();
            mxArray* facesStructArray = mxCreateStructArray(2, dims, 2, face_fields);            

            // Set the faces to the field of the current frame
            mxSetField(plhs[0], i, frame_fields[0], facesStructArray);

            // For each face
            for (size_t j = 0; j < frame.faces.size(); ++j)
            {
                dlib::Face& face = frame.faces[j];

                // Convert the landmarks to Matlab's pixel format
                for (size_t k = 0; k < face.landmarks.size(); ++k)
                {
                    face.landmarks[k].x += 1;
                    face.landmarks[k].y += 1;
                }

                // Set the landmarks to the field of the current face
                cv::Mat_<float> landmarks(face.landmarks.size(), 2, (float*)face.landmarks.data());
                mxSetField(facesStructArray, j, face_fields[0], MxArray(landmarks));

                // Convert the bounding box to Matlab's pixel format
                ++face.bbox.at<double>(0);
                ++face.bbox.at<double>(1);

                // Set the bounding to the field of the current face
                mxSetField(facesStructArray, j, face_fields[1], MxArray(face.bbox));
            }

            // Set the width and height to the fields of the current frame
            mxSetField(plhs[0], i, frame_fields[1], MxArray(frame.width));
            mxSetField(plhs[0], i, frame_fields[2], MxArray(frame.height));
        }
    }
    catch (std::exception& e)
    {
        //printfFnc("Error: %s", e.what());
        mexErrMsgIdAndTxt("dlib_find_face_landmarks:parsing", "Error: %s", e.what());
    }  
}
