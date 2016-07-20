#ifndef __SFL_FACE_TRACKER__
#define __SFL_FACE_TRACKER__

// sfl
#include "sequence_face_landmarks.h"

// OpenCV
#include <opencv2/core.hpp>

namespace sfl
{
	/** @brief Interface for tracking faces across a sequence of frames.
	*/
	class FaceTracker
	{
	public:

		/** @brief Add a frame to process.
		@param frame The frame to process [BGR|Grayscale].
		@param sfl_frame The face landmarks frame to track the faces from. The faces 
		ids will be changed according to previous tracked faces.
		*/
		virtual void addFrame(const cv::Mat& frame, Frame& sfl_frame) = 0;

		/** @brief Clear all processed data.
		*/
		virtual void clear() = 0;

		/** @brief Create a full copy of the face tracker.
		*/
		virtual std::shared_ptr<FaceTracker> clone() = 0;

		/** @brief Create an instance of the face tracker.
		*/
		static std::shared_ptr<FaceTracker> create();
	};

}   // namespace sfl

#endif	// __SFL_FACE_TRACKER__
