#include "sfl_viewer_states.h"
#include "sfl_viewer.h"

#include <iostream>//

// Boost
#include <boost/filesystem.hpp>

// Qt
#include <QTimer>
#include <QMessageBox>

using namespace boost::filesystem;

namespace sfl
{
    ViewerSM::ViewerSM(Viewer * _viewer) : viewer(_viewer)
    {
    }

    Inactive::Inactive(my_context ctx) : my_base(ctx), viewer(nullptr)
    {
        viewer = context<ViewerSM>().viewer;
    }

    void Inactive::onUpdate(const EvUpdate & event)
    {
        // Render to display
        viewer->display->setPixmap(QPixmap::fromImage(viewer->render_image->rgbSwapped()));
        viewer->display->update();
    }

    sc::result Inactive::react(const EvStart &)
    {
        if (viewer->vs == nullptr || viewer->sfl == nullptr)
        {
            QMessageBox msgBox;
            msgBox.setText("Failed to open sequence sources.");
            msgBox.exec();
            return discard_event();
        }
        return transit< Active >();
    }

    Active::Active(my_context ctx) : my_base(ctx), viewer(nullptr)
    {
        viewer = context<ViewerSM>().viewer;
        post_event(EvStart());
    }

    void Active::onSeek(const EvSeek & event)
    {
        if (event.i < 0 || event.i >= viewer->total_frames) return;

        viewer->vs->seek(event.i);
        if (viewer->vs->read())
        {
            viewer->curr_frame_pos = event.i;
            viewer->frame_slider->setValue(viewer->curr_frame_pos);
            viewer->curr_frame_lbl->setText(std::to_string(viewer->curr_frame_pos).c_str());
            viewer->frame = viewer->vs->getFrame();
            post_event(EvUpdate());
        }
    }

    void Active::onStart(const EvStart & event)
    {
        // Reshape window
        viewer->display->setMinimumSize(viewer->vs->getWidth(), viewer->vs->getHeight());
        viewer->adjustSize();

        // Read first video frame
        viewer->curr_frame_pos = 0;
        viewer->total_frames = viewer->vs->size();
        viewer->fps = viewer->vs->getFPS();
        if (viewer->fps < 1.0) viewer->fps = 30.0;
        viewer->vs->seek(viewer->curr_frame_pos);
        if (viewer->vs->read())
            viewer->frame = viewer->vs->getFrame();

        // Initialize render frame
        QSize displaySize = viewer->display->size();
        viewer->render_frame = cv::Mat::zeros(displaySize.height(), displaySize.width(), CV_8UC3);

        // Get sfl frames
        const std::list<std::unique_ptr<Frame>>& sfl_frames_list = viewer->sfl->getSequence();
        viewer->sfl_frames.clear();
        viewer->sfl_frames.reserve(sfl_frames_list.size());
        for (auto& frame : sfl_frames_list)
            viewer->sfl_frames.push_back(frame.get());

        // Initialize widgets
        path title(path(viewer->sequence_path).filename() += path(" / ") +=
            path(viewer->landmarks_path).filename() += path(" - SFL Viewer"));
        viewer->setWindowTitle(title.string().c_str());
        viewer->frame_slider->setMinimum(0);
        viewer->frame_slider->setMaximum(viewer->total_frames - 1);
        viewer->frame_slider->setValue(viewer->curr_frame_pos);
        viewer->curr_frame_lbl->setText(std::to_string(viewer->curr_frame_pos).c_str());
        viewer->max_frame_lbl->setText(std::to_string(viewer->total_frames - 1).c_str());
        viewer->frame_slider->setEnabled(true);
    }

    Paused::Paused(my_context ctx) : my_base(ctx), viewer(nullptr)
    {
        viewer = context<Active>().viewer;
        viewer->actionPlay->setIcon(
            QIcon::fromTheme(QStringLiteral(":/images/play.png")));
    }

    void Paused::onUpdate(const EvUpdate& event)
    {
        viewer->render();
    }

    Playing::Playing(my_context ctx) : my_base(ctx), viewer(nullptr)
    {
        viewer = context<Active>().viewer;
        viewer->actionPlay->setIcon(
            QIcon::fromTheme(QStringLiteral(":/images/pause.png")));

        // Start timer
        timer_id = viewer->startTimer((int)std::round(1000.0 / viewer->fps));
    }

    Playing::~Playing()
    {
        viewer->killTimer(timer_id);
    }

    void Playing::onUpdate(const EvUpdate& event)
    {
        viewer->render();
    }

    void Playing::onTimerTick(const EvTimerTick& event)
    {
        if (viewer->curr_frame_pos >= (viewer->total_frames - 1))
        {
            post_event(EvPlayPause());
            return;
        }

        if (viewer->vs->read())
        {
            viewer->frame_slider->setValue(++viewer->curr_frame_pos);
            viewer->curr_frame_lbl->setText(std::to_string(viewer->curr_frame_pos).c_str());
            viewer->frame = viewer->vs->getFrame();
            post_event(EvUpdate());
        }
    }
}   // namespace sfl

