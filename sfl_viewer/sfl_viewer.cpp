#include "sfl_viewer.h"
#include <vsal/VideoStreamFactory.h>
#include <sfl/utilities.h>

#include <exception>
#include <iostream>//

// Boost
#include <boost/filesystem.hpp>

// OpenCV
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// Qt
#include <QEventTransition>
#include <QKeyEventTransition>
#include <QSignalTransition>
#include <QFileDialog>
#include <QResizeEvent>
#include <QMessageBox>//

using namespace boost::filesystem;

namespace sfl
{
    Viewer::Viewer() : 
        sm(this)
    {
        setupUi(this);
        setupBl();
    }

    void Viewer::setupBl()
    {
        // Initialize state machine
        sm.initiate();

        // Connect actions
        connect(action_Open, &QAction::triggered, this, &Viewer::open);
        connect(action_Close, &QAction::triggered, this, &Viewer::close);
        connect(actionPlay, &QAction::triggered, this, &Viewer::playPause);
        connect(actionBackward, &QAction::triggered, this, &Viewer::backward);
        connect(actionForward, &QAction::triggered, this, &Viewer::forward);
        connect(frame_slider, SIGNAL(valueChanged(int)), this, SLOT(frameSliderChanged(int)));
        connect(actionShowLandmarks, SIGNAL(toggled(bool)), this, SLOT(toggleRenderParams(bool)));
        connect(actionShowBBox, SIGNAL(toggled(bool)), this, SLOT(toggleRenderParams(bool)));
        connect(actionShowIDs, SIGNAL(toggled(bool)), this, SLOT(toggleRenderParams(bool)));
        connect(actionShowLabels, SIGNAL(toggled(bool)), this, SLOT(toggleRenderParams(bool)));

        play_pause_btn->setDefaultAction(actionPlay);
        backward_btn->setDefaultAction(actionBackward);
        forward_btn->setDefaultAction(actionForward);

        // Adjust window size
        adjustSize();
    }

    void Viewer::setInputPath(const std::string & input_path)
    {
        if (path(input_path).extension() == ".lms")
            initLandmarks(input_path);
        else initVideoSource(input_path);
    }

    void Viewer::initLandmarks(const std::string & _landmarks_path)
    {
        if (!is_regular_file(_landmarks_path)) return;
        sfl = sfl::SequenceFaceLandmarks::create(_landmarks_path);
        landmarks_path = _landmarks_path;
        initVideoSource(sfl->getInputPath());
        sm.process_event(EvStart());
    }

    void Viewer::initVideoSource(const std::string & _sequence_path)
    {
        if (!is_regular_file(_sequence_path)) return;

        vsal::VideoStreamFactory& vsf = vsal::VideoStreamFactory::getInstance();
        vs.reset((vsal::VideoStreamOpenCV*)vsf.create(_sequence_path));
        if (vs != nullptr && !vs->open()) vs = nullptr;
        else
        {
            sequence_path = _sequence_path;
            path input = path(sequence_path);
            initLandmarks((input.parent_path() / (input.stem() += ".lms")).string());
            sm.process_event(EvStart());
        }
    }

    void Viewer::resizeEvent(QResizeEvent* event)
    {
        QMainWindow::resizeEvent(event);

        QSize displaySize = display->size();
        render_frame = cv::Mat::zeros(displaySize.height(), displaySize.width(), CV_8UC3);

        // Make Qt image.
        render_image.reset(new QImage((const uint8_t*)render_frame.data,
            render_frame.cols,
            render_frame.rows,
            render_frame.step[0],
            QImage::Format_RGB888));

        sm.process_event(EvUpdate());
    }

    void Viewer::timerEvent(QTimerEvent *event)
    {
        sm.process_event(EvTimerTick());
    }

    void Viewer::open()
    {
        QString file = QFileDialog::getOpenFileName(
            this,
            "Select one or more files to open",
            QString(),
            "Landmarks (*.lms);;Videos (*.mp4 *.mkv *.avi *wmv);;All files (*.*)",
            nullptr);
        setInputPath(file.toStdString());
    }

    void Viewer::close()
    {
        QMainWindow::close();
    }

    void Viewer::playPause()
    {
        sm.process_event(EvPlayPause());
    }

    void Viewer::backward()
    {
        sm.process_event(EvSeek(curr_frame_pos - 1));
    }

    void Viewer::forward()
    {
        sm.process_event(EvSeek(curr_frame_pos + 1));
    }

    void Viewer::frameSliderChanged(int i)
    {
        sm.process_event(EvSeek(i));
    }

    void Viewer::toggleRenderParams(bool toggled)
    {
        sm.process_event(EvUpdate());
    }

    void Viewer::render()
    {
        // Render landmarks
        landmarks_render_frame = frame.clone();

        for (auto& face : sfl_frames[curr_frame_pos]->faces)
        {
            if (actionShowLandmarks->isChecked())
                sfl::render(landmarks_render_frame, face->landmarks, 
                    actionShowLabels->isChecked(), landmarks_color);
            if (actionShowBBox->isChecked())
                sfl::render(landmarks_render_frame, face->bbox, bbox_color);
            if (actionShowIDs->isChecked())
                renderFaceID(landmarks_render_frame, *face, bbox_color);
        } 

        // Resize frame
        QSize displaySize = display->size();
        if (displaySize.width() != frame.cols || displaySize.height() != frame.rows)
        {
            float frame_ratio = float(frame.cols) / float(frame.rows);
            int rh = displaySize.height();
            int rw = (int)std::round(frame_ratio * rh);
            if (rw > displaySize.width())
            {
                rw = displaySize.width();
                rh = (int)std::round(rw / frame_ratio);
            }
            cv::resize(landmarks_render_frame, resized_frame, cv::Size(rw, rh), 0.0, 0.0, cv::INTER_CUBIC);
        }
        else resized_frame = landmarks_render_frame;

        // Create render frame
        int dx = (displaySize.width() - resized_frame.cols) / 2;
        int dy = (displaySize.height() - resized_frame.rows) / 2;
        resized_frame.copyTo(render_frame(cv::Rect(dx, dy, resized_frame.cols, resized_frame.rows)));

        // Render to display
        display->setPixmap(QPixmap::fromImage(render_image->rgbSwapped()));
        display->update();
    }

}   // namespace sfl

