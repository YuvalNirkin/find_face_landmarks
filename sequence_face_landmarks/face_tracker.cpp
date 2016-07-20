#include "sfl/face_tracker.h"

// std
#include <exception>

// OpenCV
#include <opencv2/imgproc.hpp>

using std::runtime_error;

namespace sfl
{
	class FaceTrackerImpl : public FaceTracker
	{
	public:
		FaceTrackerImpl() 
		{
		}

		void addFrame(const cv::Mat& frame, Frame& sfl_frame)
		{
		}

		void clear()
		{
		}

		std::shared_ptr<FaceTracker> clone()
		{
			return std::make_shared<FaceTrackerImpl>(*this);
		}

	private:	

	protected:

	};

	std::shared_ptr<FaceTracker> FaceTracker::create()
	{
		return std::make_shared<FaceTrackerImpl>();
	}

}   // namespace sfl

