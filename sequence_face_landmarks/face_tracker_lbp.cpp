#include "sfl/face_tracker.h"
#include "sfl/utilities.h"

// std
#include <memory>
#include <exception>
#include <numeric>
#include <set>
#include <iostream> // Debug

// OpenCV
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

#ifdef WITH_OPENCV_CONTRIB
#include <opencv2/face.hpp>
#endif

using std::runtime_error;

namespace sfl
{
#ifdef WITH_OPENCV_CONTRIB

    struct TrackedFaceLBP
    {
        int id;
        int frame_id;
        cv::Ptr<cv::face::LBPHFaceRecognizer> model;
        cv::Point2f pos;
        bool tracking_lost = false;
    };

    struct CandidateFace
    {
        cv::Mat frame;
        cv::Point2f pos;
    };

    class FaceTrackerLBP : public FaceTracker
    {
    public:
        FaceTrackerLBP()
        {
        }

        FaceTrackerLBP(const FaceTrackerLBP& ft) :
            m_id_counter(ft.m_id_counter),
            m_tracking_lost_range(ft.m_tracking_lost_range),
            m_verbose(ft.m_verbose)
        {
            // Deep copy tracked faces
            for (auto& face : ft.m_tracked_faces)
                m_tracked_faces.push_back(std::make_unique<TrackedFaceLBP>(*face));

            // Deep copy lost faces
            for (auto& face : ft.m_lost_faces)
                m_lost_faces.push_back(std::make_unique<TrackedFaceLBP>(*face));
        }

        ~FaceTrackerLBP()
        {
        }

        void addFrame(const cv::Mat& frame, Frame& sfl_frame)
        {
            // Create candidate faces
            std::vector<CandidateFace> candidates;
            createCandidateFaces(frame, sfl_frame, candidates);

            // Create sfl faces vector
            std::vector<Face*> sfl_faces;
            sfl_faces.reserve(sfl_frame.faces.size());
            for (auto& sfl_face : sfl_frame.faces)
                sfl_faces.push_back(sfl_face.get());

            // Initialize candidate indices set
            std::set<size_t> cand_indices;
            for (size_t i = 0; i < candidates.size(); ++i)
                cand_indices.insert(i);

            // Matched tracked faces with candidates
            match(m_tracked_faces, candidates, cand_indices, sfl_faces, sfl_frame.id);

            // Matched lost faces with remaining candidates
            match(m_lost_faces, candidates, cand_indices, sfl_faces, sfl_frame.id);

            // Move lost tracked faces to lost faces list
            {
                auto it = m_tracked_faces.begin();
                while (it != m_tracked_faces.end())
                {
                    if ((sfl_frame.id - (*it)->frame_id) < m_tracking_lost_range)
                    {
                        ++it;
                        continue;
                    }

                    if (m_verbose)
                        std::cout << "Moving face " << (*it)->id << " to lost faces" << std::endl;//
                    (*it)->tracking_lost = true;
                    m_lost_faces.push_back(std::move(*it));
                    it = m_tracked_faces.erase(it);
                }
            }

            // Bring back found tracked faces to tracked faces list
            {
                auto it = m_lost_faces.begin();
                while (it != m_lost_faces.end())
                {
                    if ((*it)->tracking_lost)
                    {
                        ++it;
                        continue;
                    }

                    if (m_verbose)
                        std::cout << "Moving face " << (*it)->id << " to tracked faces" << std::endl;//
                    m_tracked_faces.push_back(std::move(*it));
                    it = m_lost_faces.erase(it);
                }
            }

            // Add unmatched candidates as new tracked faces
            for (size_t cand_ind : cand_indices)
            {
                // Add new tracked face
                m_tracked_faces.push_back(
                    createTrackedFace(candidates[cand_ind], sfl_frame.id));
                sfl_faces[cand_ind]->id = m_tracked_faces.back()->id;
            }
        }

        void clear()
        {
            m_id_counter = 0;
            m_tracked_faces.clear();
        }

        std::shared_ptr<FaceTracker> clone()
        {
            return std::make_shared<FaceTrackerLBP>(*this);
        }

    private:
        void createCandidateFaces(const cv::Mat& frame, const Frame& sfl_frame,
            std::vector<CandidateFace>& candidates) const
        {
            const cv::Size frame_size(128, 128);

            // Convert frame to grayscale
            cv::Mat frame_gray;
            if (frame.channels() == 3)
                cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
            else frame_gray = frame;

            // For each face
            candidates.reserve(sfl_frame.faces.size());
            for (auto& face : sfl_frame.faces)
            {
                CandidateFace candidate;

                // Calculate frame
                std::vector<cv::Point> full_face;
                createFullFace(face->landmarks, full_face);
                cv::Rect bbox = cv::boundingRect(full_face);
                bbox.x = std::max(bbox.x, 0);
                bbox.y = std::max(bbox.y, 0);
                bbox.width = std::min(bbox.width, frame_gray.cols - bbox.x);
                bbox.height = std::min(bbox.height, frame_gray.rows - bbox.y);
                cv::Mat frame_gray_cropped = frame_gray(bbox);
                cv::resize(frame_gray_cropped, frame_gray_cropped, frame_size);
                candidate.frame = frame_gray_cropped;

                // Calculate position
                if (face->landmarks.size() > 0)
                {
                    for (const cv::Point& p : face->landmarks)
                        candidate.pos += cv::Point2f((float)p.x, (float)p.y);
                    candidate.pos /= (float)face->landmarks.size();
                }
                candidates.push_back(candidate);
            }
        }

        std::unique_ptr<TrackedFaceLBP> createTrackedFace(
            const CandidateFace& candidate, int frame_id)
        {
            if (m_verbose)
                std::cout << "Adding new tracked face with id " << m_id_counter << std::endl;

            std::unique_ptr<TrackedFaceLBP> tracked_face = std::make_unique<TrackedFaceLBP>();
            tracked_face->id = m_id_counter++;
            tracked_face->frame_id = frame_id;
            tracked_face->model = cv::face::createLBPHFaceRecognizer(3, 8, 8, 8);
            tracked_face->pos = candidate.pos;
            tracked_face->tracking_lost = false;

            // Train initial model
            std::vector<cv::Mat> face_frames = { candidate.frame };
            std::vector<int> labels = { tracked_face->id };
            tracked_face->model->train(face_frames, labels);

            return tracked_face;
        }

        double calc_dist(const TrackedFaceLBP& face, const CandidateFace& candidate) const
        {
            int label;
            double dist, similarity_dist, spatial_dist;
            face.model->predict(candidate.frame, label, similarity_dist);
            spatial_dist = cv::norm(face.pos - candidate.pos);
            if (!face.tracking_lost && spatial_dist <= 30.0f)
                dist = (similarity_dist + spatial_dist)*0.5f;
            else dist = similarity_dist;
            return dist;
        }

        cv::Mat calc_dist(const std::list<std::unique_ptr<TrackedFaceLBP>>& faces,
            const std::vector<CandidateFace>& candidates,
            std::set<size_t>& cand_indices) const
        {
            if (faces.empty() || cand_indices.empty()) return cv::Mat();

            cv::Mat dists = cv::Mat_<double>::zeros(faces.size(), candidates.size());
            double* dists_data = (double*)dists.data;

            // For each tracked face
            for (auto& tracked_face : faces)
            {
                // For each candidate
                for (size_t cand_ind : cand_indices)
                    *dists_data++ = calc_dist(*tracked_face, candidates[cand_ind]);
            }

            if (m_verbose)
            {
                dists_data = (double*)dists.data;
                for (auto& tracked_face : faces)
                {
                    std::cout << "face " << tracked_face->id << ": ";
                    for (const CandidateFace& cand : candidates)
                        std::cout << *dists_data++ << " ";
                    std::cout << std::endl;
                }
            }

            return dists;
        }

        void match(const std::list<std::unique_ptr<TrackedFaceLBP>>& faces,
            const std::vector<CandidateFace>& candidates, std::set<size_t>& cand_indices,
            std::vector<Face*>& sfl_faces, int frame_id)
        {
            if (cand_indices.empty()) return;

            // Get match distances
            cv::Mat dists = calc_dist(faces, candidates, cand_indices);
            double* dists_data = (double*)dists.data;

            // Create index structures
            std::vector<size_t> dist_indices(dists.total());
            std::iota(dist_indices.begin(), dist_indices.end(), 0);
            std::set<size_t> tracked_indices;
            for (size_t i = 0; i < faces.size(); ++i)
                tracked_indices.insert(i);
            std::map<size_t, size_t> cand_map;
            size_t i = 0;
            for (size_t cand_ind : cand_indices)
                cand_map[i++] = cand_ind;

            // Sort dist indices 
            std::sort(dist_indices.begin(), dist_indices.end(), [&dists_data](size_t i1, size_t i2)
            {return dists_data[i1] < dists_data[i2]; });

            // Create tracked faces vector
            std::vector<TrackedFaceLBP*> tracked_faces;
            tracked_faces.reserve(faces.size());
            for (auto& tracked_face : faces)
                tracked_faces.push_back(tracked_face.get());

            // Look for matches
            // For each dist index
            for (size_t i : dist_indices)
            {
                if (dists_data[i] > 128.0) break;
                size_t tracked_ind = i / dists.cols;
                size_t cand_ind = cand_map[i % dists.cols];

                auto& tracked_it = tracked_indices.find(tracked_ind);
                if (tracked_it == tracked_indices.end()) continue;
                auto& cand_it = cand_indices.find(cand_ind);
                if (cand_it == cand_indices.end()) continue;

                // Match found
                tracked_indices.erase(tracked_it);
                cand_indices.erase(cand_it);
                TrackedFaceLBP* tracked_face = tracked_faces[tracked_ind];
                tracked_face->frame_id = frame_id;
                //tracked_face->model->clear();
                std::vector<cv::Mat> train_frames = { candidates[cand_ind].frame };
                std::vector<int> labels = { tracked_face->id };
                //tracked_face->model->train(train_frames, labels);
                tracked_face->model->update(train_frames, labels);
                tracked_face->pos = candidates[cand_ind].pos;
                tracked_face->tracking_lost = false;
                sfl_faces[cand_ind]->id = tracked_face->id;
            }
        }

    protected:
        int m_id_counter = 0;
        int m_tracking_lost_range = 10;
        bool m_verbose = false;
        std::list<std::unique_ptr<TrackedFaceLBP>> m_tracked_faces;
        std::list<std::unique_ptr<TrackedFaceLBP>> m_lost_faces;
    };

    std::shared_ptr<FaceTracker> createFaceTrackerLBP()
    {
        return std::make_shared<FaceTrackerLBP>();
    }

#else
    std::shared_ptr<FaceTracker> createFaceTrackerLBP()
    {
        throw std::runtime_error("LBP face tracked is not available!"
            " Please build with OpenCV's extra modules.");
        return nullptr;
    }
#endif

}   // namespace sfl

