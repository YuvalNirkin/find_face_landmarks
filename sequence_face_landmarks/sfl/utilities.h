/** @file
@brief Sequence face landmarks utility functions.
*/

#ifndef __SFL_UTILITIES__
#define __SFL_UTILITIES__

// sfl
#include "sequence_face_landmarks.h"

namespace sfl
{
	/** @brief Render landmarks.
	@param img The image that the landmarks will be rendered on.
	@param landmarks The landmark points to render.
	@param drawLabels if true, for each landmark, it's 0 based index will be
	rendererd as a label.
	@param color Line\point and label color.
	@param thickness Line\point thickness.
	*/
	void render(cv::Mat& img, const std::vector<cv::Point>& landmarks,
		bool drawLabels = false, const cv::Scalar& color = cv::Scalar(0, 255, 0),
		int thickness = 1);

	/** @brief Render bounding box.
	@param img The image that the bounding box will be rendered on.
	@param bbox The bounding box rectangle to render.
	@param color Line color.
	@param thickness Line thickness.
	*/
	void render(cv::Mat& img, const cv::Rect& bbox,
		const cv::Scalar& color = cv::Scalar(0, 255, 0), int thickness = 1);

	/** @brief Render face's bounding box and landmarks.
	@param img The image that the face will be rendered on.
	@param face The face to render.
	@param drawLabels if true, for each landmark, it's 0 based index will be
	rendererd as a label.
	@param bbox_color Bounding box line color.
	@param landmarks_color Landmarks line\point and label color.
	@param thickness Line\point thickness.
	*/
	void render(cv::Mat& img, const Face& face, bool drawLabels = false,
		const cv::Scalar& bbox_color = cv::Scalar(0, 0, 255),
		const cv::Scalar& landmarks_color = cv::Scalar(0, 255, 0), int thickness = 1);

	/** @brief Render all frame faces including bounding boxs and landmarks.
	@param img The image that the faces will be rendered on.
	@param frame The frame to render.
	@param drawLabels if true, for each landmark, it's 0 based index will be
	rendererd as a label.
	@param bbox_color Bounding box line color.
	@param landmarks_color Landmarks line\point and label color.
	@param thickness Line\point thickness.
	*/
	void render(cv::Mat& img, const Frame& frame, bool drawLabels = false,
		const cv::Scalar& bbox_color = cv::Scalar(0, 0, 255),
		const cv::Scalar& landmarks_color = cv::Scalar(0, 255, 0), int thickness = 1);


	/** @brief Represents a face statistics in the sequence.
	*/
	struct FaceStat
	{
		int id = 0;					///< Face id.

		float avg_center_dist = 0;	///< Average distance from frame center.
		int frame_count = 0;		///< Frames appeared in.
		float avg_size = 0;			///< Average face's bounding box size.

		float central_ratio = 0;	///< avg_center_dist / (||(avg_width, avg_height)|| / 4).
		float frame_ratio = 0;		///< frame_count / total_frames
		float size_ratio = 0;		///< avg_size / ((avg_width + avg_height) / 4)
	};

	/** @brief Get the face statistics of the sequence
		@param sequence The sequence of frames to calculate the statistics for.
		@param stats Output vector of statistics for each face in the sequence.
	*/
	void getSequenceStats(const std::list<std::unique_ptr<Frame>>& sequence,
		std::vector<FaceStat>& stats);

	/** @brief Get the main face in a sequence.
	*/
	int getMainFaceID(const std::list<std::unique_ptr<Frame>>& sequence);

	/** @brief Get the main face from face statistics.
	*/
	int getMainFaceID(const std::vector<FaceStat>& stats);

}   // namespace sfl

#endif	// __SFL_UTILITIES__
