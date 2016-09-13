#include "sfl/face_tracker.h"

// std
#include <memory>
#include <exception>

// OpenCV
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

using std::runtime_error;

namespace sfl
{
	struct TrackedFaceBRISK
	{
		int id;
		int frame_id;
		cv::Rect bbox;
		std::vector<cv::KeyPoint> landmarks;
		cv::Mat descriptors;
		std::vector<int> desc_ind;
		Face* ref_face;
        cv::Point2f pos;
	};

	class FaceTrackerBRISK : public FaceTracker
	{
	protected:
		int m_id_counter = 0;
		cv::Ptr<cv::Feature2D> m_desc_extractor;
		std::list<std::unique_ptr<TrackedFaceBRISK>> m_tracked_faces;
		
	public:
		FaceTrackerBRISK() : m_desc_extractor(cv::BRISK::create())
		{
		}

		FaceTrackerBRISK(const FaceTrackerBRISK& ft) :
			m_id_counter(ft.m_id_counter), m_desc_extractor(ft.m_desc_extractor)
		{
			// Deep copy tracked faces
			for (auto& face : ft.m_tracked_faces)
				m_tracked_faces.push_back(std::make_unique<TrackedFaceBRISK>(*face));
		}

		void addFrame(const cv::Mat& frame, Frame& sfl_frame)
		{
			// Convert frame to grayscale
			cv::Mat frame_gray;
			if(frame.channels() == 3)
				cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
			else frame_gray = frame;

			// Initialize candidate list
			std::list<std::unique_ptr<TrackedFaceBRISK>> candidates;
			for (auto& face : sfl_frame.faces)
				candidates.push_back(createTrackedFace(frame_gray, *face, sfl_frame.id));

			// For each tracked face
			double dist, similarity_dist, spatial_dist;
			cv::Mat_<double> distances(m_tracked_faces.size(), candidates.size());
			double* distances_data = (double*)distances.data;
			std::list<std::unique_ptr<TrackedFaceBRISK>>::iterator best_candidate;
			for (auto& tracked_face : m_tracked_faces)
			{
				// For each candidate face
				std::list<std::unique_ptr<TrackedFaceBRISK>>::iterator it;
				for (it = candidates.begin(); it != candidates.end(); ++it)
				{
                    similarity_dist = match(tracked_face.get(), it->get());
                    spatial_dist = cv::norm(tracked_face->pos - (*it)->pos);
                    dist = (similarity_dist + spatial_dist)*0.5f;
					*distances_data++ = dist;
				}
			}

			// Find matches
			if (m_tracked_faces.size() > 0)
			{
				std::list<std::unique_ptr<TrackedFaceBRISK>>::iterator tracked_it, cand_it;
				std::list<std::unique_ptr<TrackedFaceBRISK>>::iterator best_tracked_it, best_cand_it;
				double min_dist = std::numeric_limits<double>::max();
				const double max_dist = 250.0f;
				int i, j, best_i, best_j;
				std::vector<std::pair<std::list<std::unique_ptr<TrackedFaceBRISK>>::iterator,
					std::list<std::unique_ptr<TrackedFaceBRISK>>::iterator>> matches;

				while (candidates.size() > 0)
				{
					min_dist = std::numeric_limits<double>::max();

					for (tracked_it = m_tracked_faces.begin(), i = 0; tracked_it != m_tracked_faces.end(); ++tracked_it, ++i)
					{
						for (cand_it = candidates.begin(), j = 0; cand_it != candidates.end(); ++cand_it, ++j)
						{
							dist = distances(i, j);
							if (dist < min_dist && dist < max_dist)
							{
								min_dist = dist;
								best_tracked_it = tracked_it;
								best_cand_it = cand_it;
								best_i = i;
								best_j = j;
							}
						}
					}
					if (min_dist < max_dist)
					{
						// Found match
						matches.push_back(std::make_pair(best_tracked_it, best_cand_it));
						if (matches.size() == m_tracked_faces.size() || matches.size() == candidates.size())
							break;
						distances(cv::Range(best_i, best_i + 1), cv::Range::all()) = max_dist;
						distances(cv::Range::all(), cv::Range(best_j, best_j + 1)) = max_dist;
					}
					else break;
				}

				// Process matches
				for (auto& match : matches)
				{
					// Set candidate data to matched tracked face
					(*match.first)->bbox = (*match.second)->bbox;
					(*match.first)->landmarks = (*match.second)->landmarks;
					(*match.first)->frame_id = sfl_frame.id;
					(*match.first)->descriptors = (*match.second)->descriptors;
					(*match.first)->desc_ind = (*match.second)->desc_ind;
                    (*match.first)->pos = (*match.second)->pos;

					// Output the tracked id and remove the candidate
					(*match.second)->ref_face->id = (*match.first)->id;
					candidates.erase(match.second);
				}
			}

			// Add unmatched candidates to tracked faces list
			std::list<std::unique_ptr<TrackedFaceBRISK>>::iterator it;
			for (it = candidates.begin(); it != candidates.end(); ++it)
			{
				// Output new id and add the candidate to the tracked faces list
				(*it)->id = m_id_counter++;
				(*it)->ref_face->id = (*it)->id;
				m_tracked_faces.push_back(std::move(*it));
			}
		}

		void clear()
		{
			m_id_counter = 0;
			m_tracked_faces.clear();
		}

		std::shared_ptr<FaceTracker> clone()
		{
			return std::make_shared<FaceTrackerBRISK>(*this);
		}

	private:	
		std::unique_ptr<TrackedFaceBRISK> createTrackedFace(const cv::Mat& frame_gray,
			sfl::Face& face, int _frame_id)
		{
			std::unique_ptr<TrackedFaceBRISK> tracked_face = std::make_unique<TrackedFaceBRISK>();
			tracked_face->id = face.id;
			tracked_face->frame_id = _frame_id;
			tracked_face->bbox = face.bbox;
			tracked_face->ref_face = &face;

			// Find scale
			std::vector<cv::KeyPoint> keypoints;
			cv::Mat mask = cv::Mat_<unsigned char>::zeros(frame_gray.size());
			cv::Point tl(std::max(face.bbox.tl().x, 0), std::max(face.bbox.tl().y, 0));
			cv::Point br(std::min(face.bbox.br().x, frame_gray.cols - 1),
				std::min(face.bbox.br().y, frame_gray.rows - 1));
			mask(cv::Rect(tl, br)) = 1;
			m_desc_extractor->detect(frame_gray, keypoints, mask);
			float scale = 0.0f;
			for (cv::KeyPoint& kp : keypoints) scale += kp.size;
			if (keypoints.empty()) scale = 10.0f;
			else scale /= keypoints.size();

			// Convert landmarks format
			std::vector<cv::KeyPoint>& landmarks = tracked_face->landmarks;
			landmarks.reserve(face.landmarks.size());
			for (int i = 0; i < face.landmarks.size(); ++i)
			{
				const cv::Point& p = face.landmarks[i];
				landmarks.push_back(cv::KeyPoint((float)p.x, (float)p.y, scale, 0.0f, 0, i));
			}

			// Calculate descriptors
			std::vector<cv::KeyPoint> landmarks_extracted = landmarks;
			m_desc_extractor->compute(frame_gray, landmarks_extracted,
				tracked_face->descriptors);

			// Calculate descriptor indices
			tracked_face->desc_ind.resize(landmarks_extracted.size());
			int delta;
			for (int i = 0, j = 0; i < landmarks.size() && j < landmarks_extracted.size(); ++i)
			{
				cv::Point2f& pt = landmarks[i].pt;
				cv::Point2f& pt_ex = landmarks_extracted[j].pt;
				if ((int)std::round(pt.x - pt_ex.x + pt.y - pt_ex.y) != 0) continue;
				tracked_face->desc_ind[j++] = i;
			}

            // Calculate position
            for (const cv::KeyPoint& kp : tracked_face->landmarks)
                tracked_face->pos += kp.pt;
            tracked_face->pos /= (float)tracked_face->landmarks.size();

			return tracked_face;
		}

		double match(TrackedFaceBRISK* face1, TrackedFaceBRISK* face2)
		{
			double dist, avg_dist = 0;
			int di, dj, total = 0;
			for (int i = 0, j = 0; i < face1->descriptors.rows && j < face2->descriptors.rows;)
			{
				di = face1->desc_ind[i];
				dj = face2->desc_ind[j];
				if (di < dj) ++i;
				else if (di > dj) ++j;
				else
				{
					dist = cv::norm(face1->descriptors.row(i), face2->descriptors.row(j), cv::NORM_HAMMING);
					avg_dist += dist;
					++i; ++j, ++total;
				}
			}

			return avg_dist / total;
		}
	};

    std::shared_ptr<FaceTracker> createFaceTrackerBRISK()
    {
        return std::make_shared<FaceTrackerBRISK>();
    }

}   // namespace sfl

