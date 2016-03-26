#ifndef __DLIB_FIND_FACE_LANDMARKS__
#define __DLIB_FIND_FACE_LANDMARKS__

/************************************************************************************
*									   Includes										*
************************************************************************************/
#include <string>
#include <opencv2/core/core.hpp>
//#include <opencv2\core\types.hpp>

/************************************************************************************
*									 Declarations									*
************************************************************************************/
namespace dlib
{
    extern bool verbose;
    extern double frame_scale;

    //typedef cv::Point2f Landmark;
    //typedef std::vector<Landmark> Face;
    //typedef std::vector<Face> Frame;

    struct Face
    {
        std::vector<cv::Point2f> landmarks;
        cv::Mat bbox;
    };

    struct Frame
    {
        std::vector<Face> faces;
        int width;
        int height;
    };

    void find_face_landmarks(const std::string& modelPath,
        const std::string& inputPath, std::vector<Frame>& frames);

    void find_face_landmarks(const std::string& modelPath,
        int device, int width, int height,
        std::vector<Frame>& frames);

    // Forward declarations
    //class frontal_face_detector;
    //class shape_predictor;

/************************************************************************************
*										Classes										*
************************************************************************************/

   /* class FaceLandmarksDetector
    {
    public:
        FaceLandmarksDetector(const std::string& modelPath);
        ~FaceLandmarksDetector();

        void operator()(const std::string& inputPath, std::vector<Frame>& frames);

        void operator()(int device, int width, int height, std::vector<Frame>& frames);

    private:
        bool running;
        frontal_face_detector* mFaceDetector;
        shape_predictor* mPoseModel;
    };*/

}   // namespace dlib

#endif	// __DLIB_FIND_FACE_LANDMARKS__