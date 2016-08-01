#include "sfl/utilities.h"

// std
#include <map>

// OpenCV
#include <opencv2/imgproc.hpp>

using std::runtime_error;

namespace sfl
{
	void render(cv::Mat & img, const std::vector<cv::Point>& landmarks,
		bool drawLabels, const cv::Scalar & color, int thickness)
	{
		if (landmarks.size() == 68)
		{
			for (size_t i = 1; i <= 16; ++i)
				cv::line(img, landmarks[i], landmarks[i - 1], color, thickness);

			for (size_t i = 28; i <= 30; ++i)
				cv::line(img, landmarks[i], landmarks[i - 1], color, thickness);

			for (size_t i = 18; i <= 21; ++i)
				cv::line(img, landmarks[i], landmarks[i - 1], color, thickness);

			for (size_t i = 23; i <= 26; ++i)
				cv::line(img, landmarks[i], landmarks[i - 1], color, thickness);

			for (size_t i = 31; i <= 35; ++i)
				cv::line(img, landmarks[i], landmarks[i - 1], color, thickness);
			cv::line(img, landmarks[30], landmarks[35], color, thickness);

			for (size_t i = 37; i <= 41; ++i)
				cv::line(img, landmarks[i], landmarks[i - 1], color, thickness);
			cv::line(img, landmarks[36], landmarks[41], color, thickness);

			for (size_t i = 43; i <= 47; ++i)
				cv::line(img, landmarks[i], landmarks[i - 1], color, thickness);
			cv::line(img, landmarks[42], landmarks[47], color, thickness);

			for (size_t i = 49; i <= 59; ++i)
				cv::line(img, landmarks[i], landmarks[i - 1], color, thickness);
			cv::line(img, landmarks[48], landmarks[59], color, thickness);

			for (size_t i = 61; i <= 67; ++i)
				cv::line(img, landmarks[i], landmarks[i - 1], color, thickness);
			cv::line(img, landmarks[60], landmarks[67], color, thickness);
		}
		else
		{
			for (size_t i = 0; i < landmarks.size(); ++i)
				cv::circle(img, landmarks[i], thickness, color, -1);
		}

		if (drawLabels)
		{
			// Add labels
			for (size_t i = 0; i < landmarks.size(); ++i)
				cv::putText(img, std::to_string(i), landmarks[i],
					cv::FONT_HERSHEY_PLAIN, 0.5, color, thickness);
		}
	}

	void render(cv::Mat& img, const cv::Rect& bbox, const cv::Scalar& color,
		int thickness)
	{
		cv::rectangle(img, bbox, color, thickness);
	}

	void render(cv::Mat& img, const Face& face, bool drawLabels,
		const cv::Scalar& bbox_color, const cv::Scalar& landmarks_color, int thickness,
		double fontScale)
	{
		render(img, face.bbox, bbox_color, thickness);
		render(img, face.landmarks, drawLabels, landmarks_color, thickness);

		// Add face id label
		std::string lbl = std::to_string(face.id);
		int baseline = 0;
		cv::Size textSize = cv::getTextSize(lbl, cv::FONT_HERSHEY_PLAIN,
			fontScale, thickness, &baseline);
		cv::Point lbl_pt(face.bbox.x + (face.bbox.width - textSize.width) / 2, 
			face.bbox.y - textSize.height / 4);
		cv::putText(img, std::to_string(face.id), lbl_pt,
			cv::FONT_HERSHEY_PLAIN, fontScale, bbox_color, thickness);
	}

	void render(cv::Mat& img, const Frame& frame, bool drawLabels,
		const cv::Scalar& bbox_color, const cv::Scalar& landmarks_color, int thickness,
		double fontScale)
	{
		for (auto& face : frame.faces)
			render(img, *face, drawLabels, bbox_color, landmarks_color, thickness,
				fontScale);
	}

	void getSequenceStats(const std::list<std::unique_ptr<Frame>>& sequence,
		std::vector<FaceStat>& stats)
	{
		std::map<int, int> face_map;
		int total_frames = 0;
		cv::Point2f center, pos;
		float dist, max_dist = 0;
		float size, max_size = 0;
		float avg_frame_width = 0, avg_frame_height = 0;

		// For each frame
		for (auto& frame : sequence)
		{
			if (frame->faces.empty()) continue;
			++total_frames;

			center.x = frame->width*0.5f;
			center.y = frame->height*0.5f;
			avg_frame_width += (float)frame->width;
			avg_frame_height += (float)frame->height;

			// For each face
			for (auto& face : frame->faces)
			{
				// Get face stat
				int i = face_map[face->id];
				if (i >= stats.size() || face->id != stats[i].id)
				{
					// Create new face stat
					stats.push_back(FaceStat());
					i = stats.size() - 1;
					face_map[face->id] = i;
					stats[i].id = face->id;
				}
				FaceStat& face_stat = stats[i];

				// Add center distance
				cv::Point tl = face->bbox.tl();
				cv::Point br = face->bbox.br();
				pos.x = (tl.x + br.x)*0.5f;
				pos.y = (tl.y + br.y)*0.5f;
				dist = (float)cv::norm(pos - center);
				face_stat.avg_center_dist += dist;

				// Increase frame count
				++(face_stat.frame_count);

				// Add face size
				size = (face->bbox.width + face->bbox.height)*0.5f;
				face_stat.avg_size += size;

			}
		}

		if (total_frames == 0) return;

		// Calculate averages and ranges
		avg_frame_width /= total_frames;
		avg_frame_height /= total_frames;
		max_dist = 0.25f*std::sqrt(avg_frame_width*avg_frame_width +
			avg_frame_height*avg_frame_height);
		max_size = 0.25f*(avg_frame_width + avg_frame_height);

		for (auto& stat : stats)
		{
			stat.avg_center_dist /= stat.frame_count;
			stat.avg_size /= stat.frame_count;
		}

		// Finalize stats
		for (auto& stat : stats)
		{
			// Calculate central ratio
			if (max_dist < 1e-6f) stat.central_ratio = 1.0f;
			else stat.central_ratio = (1 - stat.avg_center_dist / max_dist);
			stat.central_ratio = std::min(std::max(0.0f, stat.central_ratio), 1.0f);

			// Calculate frame ratio
			stat.frame_ratio = float(stat.frame_count) / total_frames;

			// Calculate size ratio
			if (max_size < 1e-6f) stat.size_ratio = 1.0f;
			else stat.size_ratio = stat.avg_size / max_size;
			stat.size_ratio = std::min(std::max(0.0f, stat.size_ratio), 1.0f);
		}
	}

	int getMainFaceID(const std::list<std::unique_ptr<Frame>>& sequence)
	{
		std::vector<FaceStat> stats;
		getSequenceStats(sequence, stats);
		return getMainFaceID(stats);
	}

	int getMainFaceID(const std::vector<FaceStat>& stats)
	{
		int best_id = -1;
		float score, best_score = 0;

		// For each stat
		for (auto& stat : stats)
		{
			score = (stat.central_ratio + stat.frame_ratio + stat.size_ratio) / 3;
			if (score > best_score)
			{
				best_score = score;
				best_id = stat.id;
			}
		}

		return best_id;
	}
}   // namespace sfl

