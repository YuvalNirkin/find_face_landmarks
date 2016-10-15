#ifndef __SFL_SEQUENCE_FACE_LANDMARKS__
#define __SFL_SEQUENCE_FACE_LANDMARKS__

// std
#include <string>
#include <list>
#include <memory>

// OpenCV
#include <opencv2/core.hpp>

namespace sfl
{
	/** @brief Represents a face detected in a frame.
	*/
    struct Face
    {
		int id;								///< Face id.
		cv::Rect bbox;						///< Bounding box.
        std::vector<cv::Point> landmarks;	///< Face landmarks.
    };

	/** @brief Represents a frame that might include faces.
	*/
    struct Frame
    {
		int id;									///< Frame id.
		int width;								///< Frame width [pixels]
		int height;								///< Frame height [pixels]
        std::list<std::unique_ptr<Face>> faces;	///< Detected faces in the frame

		/** @brief Get face by id.
		Return null if a face with the specified id is not found.
		*/
		const Face* getFace(int id) const;
    };

    /** @brief Represents face tracking type.
    */
    enum FaceTrackingType
    {
        TRACKING_NONE = 0,
        TRACKING_BRISK = 1,
        TRACKING_LBP = 2
    };

	/** @brief Interface for sequence face landmarks functionality.

	This class provide face landmarks functionality over a sequence of frames.
	*/
	class SequenceFaceLandmarks
	{
	public:

		/** @brief Add a frame to process.
		@param frame The frame to process [BGR|Grayscale].
		@param id Frame id. If negative, an internal counter will be used instead.
		*/
		virtual const Frame& addFrame(const cv::Mat& frame, int id = -1) = 0;

		/** @brief Get the frame sequence with all landmarks and bounding boxes 
		for each detected face.
		*/
		virtual const std::list<std::unique_ptr<Frame>>& getSequence() const = 0;

        /** @brief Get the frame sequence with all landmarks and bounding boxes
        for each detected face.
        */
        virtual std::list<std::unique_ptr<Frame>>& getSequenceMutable() = 0;

		/** @brief Clear all processed or loaded data.
		*/
		virtual void clear() = 0;

		/** @brief Create a full copy, loaded face detector and landmark model 
		will be shared.
		*/
		virtual std::shared_ptr<SequenceFaceLandmarks> clone() = 0;

		/** @brief Get landmarks model file.
		*/
		virtual const std::string& getModel() const = 0;

		/** @brief Get frame scale.
		*/
		virtual float getFrameScale() const = 0;

        /** Get source input path.
        This was either loaded from file or set manually.
        */
        virtual const std::string& getInputPath() const = 0;

		/** @brief Get the current type of tracking.
		*/
		virtual FaceTrackingType getTracking() const = 0;

		/** @brief Load a sequence of face landmarks from file.
		*/
		virtual void load(const std::string& filePath) = 0;

		/** @brief Save current sequence of face landmarks to file.
		*/
		virtual void save(const std::string& filePath) const = 0;

		/** @brief Set frame scale.
		*/
		virtual void setFrameScale(float frame_scale) = 0;

		/** @brief Set landmarks model file.
		*/
		virtual void setModel(const std::string& modelPath) = 0;

        /** Get source input path.
        The input path can be then saved and loaded from file.
        */
        virtual void setInputPath(const std::string& inputPath) = 0;

		/** @brief Set tracking type [TRACKING_NONE | TRACKING_BRISK | TRACKING_LBP].
			This will keep the face ids consistent in the sequence.
		*/
		virtual void setTracking(FaceTrackingType tracking) = 0;
		
		/** @brief Get the number of the current frames.
		*/
		virtual size_t size() const = 0;

		/** @brief Create an instance initialized with a landmarks model file.
		@param landmarks_path Path to the landmarks model file or landmarks cache file (.pb).
		@param frame_scale Each frame will be scaled by this factor. Useful for detection of small
		faces. The landmarks will still be in the original frame's pixel coordinates.
        @param tracking Tracking type [TRACKING_NONE | TRACKING_BRISK | TRACKING_LBP].
		*/
		static std::shared_ptr<SequenceFaceLandmarks> create(
			const std::string& landmarks_path, float frame_scale = 1.0f,
            FaceTrackingType tracking = TRACKING_NONE);

		/** @brief Create an instance.
		@param frame_scale Each frame will be scaled by this factor. Useful for detection of small
		faces. The landmarks will still be in the original frame's pixel coordinates.
        @param tracking Tracking type [TRACKING_NONE | TRACKING_BRISK | TRACKING_LBP].
		*/
		static std::shared_ptr<SequenceFaceLandmarks> create(
			float frame_scale = 1.0f, FaceTrackingType tracking = TRACKING_NONE);
	};

}   // namespace sfl

#endif	// __SFL_SEQUENCE_FACE_LANDMARKS__
