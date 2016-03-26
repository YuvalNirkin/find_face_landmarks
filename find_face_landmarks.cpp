/************************************************************************************
*									   Includes										*
************************************************************************************/
#include "find_face_landmarks.h"
#include <iostream>
#include <exception>

// vsal
#include <vsal/VideoStreamFactory.h>
#include <vsal/VideoStreamOpenCV.h>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// dlib
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing/shape_predictor.h>

/************************************************************************************
*									  Namespaces									*
************************************************************************************/
using std::cout;
using std::endl;
using std::string;
using std::exception;

/************************************************************************************
*									 Declarations									*
************************************************************************************/
namespace dlib
{
    bool verbose = true;
    double frame_scale = 1.0;
    bool running = true;

/************************************************************************************
*									Implementation									*
************************************************************************************/

    void render_face_detections(cv::Mat img,
        const std::vector<dlib::full_object_detection>& shapes,
        cv::Scalar color = cv::Scalar(0, 255, 0))
    {
        for (size_t j = 0; j < shapes.size(); ++j)
        {
            if (shapes[j].num_parts() != 68)
                throw exception("Each shape size must be exactly 68!");

            const dlib::full_object_detection& d = shapes[j];
            for (unsigned long i = 1; i <= 16; ++i)
                cv::line(img, cv::Point(d.part(i).x(), d.part(i).y()),
                cv::Point(d.part(i - 1).x(), d.part(i - 1).y()), color);

            for (unsigned long i = 28; i <= 30; ++i)
                cv::line(img, cv::Point(d.part(i).x(), d.part(i).y()),
                cv::Point(d.part(i - 1).x(), d.part(i - 1).y()), color);

            for (unsigned long i = 18; i <= 21; ++i)
                cv::line(img, cv::Point(d.part(i).x(), d.part(i).y()),
                cv::Point(d.part(i - 1).x(), d.part(i - 1).y()), color);
            for (unsigned long i = 23; i <= 26; ++i)
                cv::line(img, cv::Point(d.part(i).x(), d.part(i).y()),
                cv::Point(d.part(i - 1).x(), d.part(i - 1).y()), color);
            for (unsigned long i = 31; i <= 35; ++i)
                cv::line(img, cv::Point(d.part(i).x(), d.part(i).y()),
                cv::Point(d.part(i - 1).x(), d.part(i - 1).y()), color);
            cv::line(img, cv::Point(d.part(30).x(), d.part(30).y()),
                cv::Point(d.part(35).x(), d.part(35).y()), color);

            for (unsigned long i = 37; i <= 41; ++i)
                cv::line(img, cv::Point(d.part(i).x(), d.part(i).y()),
                cv::Point(d.part(i - 1).x(), d.part(i - 1).y()), color);
            cv::line(img, cv::Point(d.part(36).x(), d.part(36).y()),
                cv::Point(d.part(41).x(), d.part(41).y()), color);

            for (unsigned long i = 43; i <= 47; ++i)
                cv::line(img, cv::Point(d.part(i).x(), d.part(i).y()),
                cv::Point(d.part(i - 1).x(), d.part(i - 1).y()), color);
            cv::line(img, cv::Point(d.part(42).x(), d.part(42).y()),
                cv::Point(d.part(47).x(), d.part(47).y()), color);

            for (unsigned long i = 49; i <= 59; ++i)
                cv::line(img, cv::Point(d.part(i).x(), d.part(i).y()),
                cv::Point(d.part(i - 1).x(), d.part(i - 1).y()), color);
            cv::line(img, cv::Point(d.part(48).x(), d.part(48).y()),
                cv::Point(d.part(59).x(), d.part(59).y()), color);

            for (unsigned long i = 61; i <= 67; ++i)
                cv::line(img, cv::Point(d.part(i).x(), d.part(i).y()),
                cv::Point(d.part(i - 1).x(), d.part(i - 1).y()), color);
            cv::line(img, cv::Point(d.part(60).x(), d.part(60).y()),
                cv::Point(d.part(67).x(), d.part(67).y()), color);
            /*
            // Add labels
            for (unsigned long i = 0; i < 68; ++i)
            cv::putText(img, std::to_string(i), cv::Point(d.part(i).x(), d.part(i).y()),
            cv::FONT_HERSHEY_PLAIN, 1.0, color, 2.0);
            */
        }
    }

    void dlib_obj_to_points(const dlib::full_object_detection& obj,
        std::vector<cv::Point2f>& points)
    {
        points.resize(obj.num_parts());
        for (unsigned long i = 0; i < obj.num_parts(); ++i)
        {
            cv::Point2f& p = points[i];
            const dlib::point& obj_p = obj.part(i);
            p.x = (float)obj_p.x();
            p.y = (float)obj_p.y();
        }
    }

    template<typename pixel_type>
    void extract_landmarks_from_frame(cv::Mat& frame,
        dlib::frontal_face_detector& detector, dlib::shape_predictor& pose_model,
        Frame& frame_landmarks, int& faceCount)
    {
        Face face;

        // Up scale by a factor of 2 to find small faces
        cv::Mat frameScaled;
        cv::resize(frame, frameScaled, cv::Size(), frame_scale, frame_scale);

        // Convert OpenCV's mat to dlib format 
        dlib::cv_image<pixel_type> dlib_frame(frameScaled);  

        // Detect bounding boxes around all the faces in the image.
        std::vector<dlib::rectangle> faces = detector(dlib_frame);

        // Find the pose of each face we detected.
        std::vector<dlib::full_object_detection> shapes;
        for (size_t i = 0; i < faces.size(); ++i)
        {
            dlib::full_object_detection shape = pose_model(dlib_frame, faces[i]);
            dlib_obj_to_points(shape, face.landmarks);

            // Scale landmarks to the original frame's pixel coordinates
            for (size_t j = 0; j < face.landmarks.size(); ++j)
            {
                face.landmarks[j].x /= frame_scale;
                face.landmarks[j].y /= frame_scale;
            }

            // Set face bounding box
            face.bbox = (cv::Mat_<double>(1, 4) << 
                (double)faces[i].left(), (double)faces[i].top(),
                (double)faces[i].width(), (double)faces[i].height());
            face.bbox /= frame_scale;
            
            frame_landmarks.faces.push_back(face);

            shapes.push_back(shape);    // For drawing later
            ++faceCount;
        }

        // Show face poses
        render_face_detections(frameScaled, shapes);
        string msg = "Faces found so far: " + std::to_string(faceCount);
        cv::putText(frameScaled, msg, cv::Point(10, 20),
            cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 255),
            1, CV_AA);
        cv::putText(frameScaled, "press any key to stop", cv::Point(10, frameScaled.rows - 20),
            cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 255),
            1, CV_AA);
        cv::imshow("dlib_find_face_landmarks", frameScaled);
        int key = cv::waitKey(1);
        if (key >= 0) running = false;
    }

    void find_face_landmarks(const std::string& modelPath, vsal::VideoStreamOpenCV* vs,
        bool live, std::vector<Frame>& frames)
    {
        int faceCount = 0;

        // Open video source
        if(!vs->open()) throw exception("Failed to open video source!");
        int width = vs->getWidth();
        int height = vs->getHeight();

        // Face detector for finding bounding boxes for each face in an image
        dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();

        // Shape predictor for finding landmark positions given an image and face bounding box.
        dlib::shape_predictor pose_model;
        dlib::deserialize(modelPath) >> pose_model;

        // For each frame
        cv::Mat frame;
        running = true;
        while (running && vs->read())
        {
            frame = vs->getFrame();
            Frame frame_landmarks;

            if (frame.channels() == 3)  // BGR
                extract_landmarks_from_frame<bgr_pixel>(
                frame, detector, pose_model, frame_landmarks, faceCount);
            else // grayscale
                extract_landmarks_from_frame<unsigned char>(
                frame, detector, pose_model, frame_landmarks, faceCount);

            // Set frame resolution
            frame_landmarks.width = frame.cols;
            frame_landmarks.height = frame.rows;
            
            // Add detected face landmarks to output
            frames.push_back(frame_landmarks);
        }
    }

    void find_face_landmarks(const std::string& modelPath,
        const std::string& inputPath, std::vector<Frame>& frames)
    {
        // Create video source
        vsal::VideoStreamFactory& vsf = vsal::VideoStreamFactory::getInstance();
        vsal::VideoStreamOpenCV* vs = (vsal::VideoStreamOpenCV*)vsf.create(inputPath);

        // Continue processing
        find_face_landmarks(modelPath, vs, false, frames);

        // Cleanup
        delete vs;
        cv::destroyAllWindows();
    }

    void find_face_landmarks(const std::string& modelPath,
        int device, int width, int height,
        std::vector<Frame>& frames)
    {
        // Create video source
        vsal::VideoStreamFactory& vsf = vsal::VideoStreamFactory::getInstance();
        vsal::VideoStreamOpenCV* vs = (vsal::VideoStreamOpenCV*)vsf.create(device, width, height);

        // Continue processing
        find_face_landmarks(modelPath, vs, true, frames);

        // Cleanup
        delete vs;
        cv::destroyAllWindows();
    }

    /*
    FaceLandmarksDetector::FaceLandmarksDetector(const std::string& modelPath)
    {

    }

    FaceLandmarksDetector::~FaceLandmarksDetector()
    {
        if (mFaceDetector != nullptr) delete mFaceDetector;
        if (mPoseModel != nullptr) delete mPoseModel;
    }
    */

}   // namespace dlib

