#include "timer_widget.h"

#include <libauthpp/logger.h>
#include <libauthpp/time_util.h>

#include <gdkmm/frameclock.h>
#include <gdkmm/general.h>

#include <gtkmm/snapshot.h>

namespace authppgtk {

namespace {
    authpp::Logger log("TimerWidget");
}

TimerWidget::TimerWidget()
{
    set_expand(false);

    timeStep = 0;
    lastUpdateMs = 0L;

    add_tick_callback([this](const auto& clock) {
        auto currentFrameTime = clock->get_frame_time() / 1000;
        if (currentFrameTime > lastUpdateMs + 200) {
            queue_draw();
            lastUpdateMs = currentFrameTime;
        }
        return true;
    });
}

TimerWidget::~TimerWidget() = default;

void TimerWidget::setTimeStep(int ts) { this->timeStep = ts; }

void TimerWidget::measure_vfunc(Gtk::Orientation orientation, int /* for_size */, int& minimum,
    int& natural, int& minimum_baseline, int& natural_baseline) const
{
    minimum = 24;
    natural = 24;
    minimum_baseline = -1;
    natural_baseline = -1;
}

void TimerWidget::snapshot_vfunc(const Glib::RefPtr<Gtk::Snapshot>& snapshot)
{
    auto degToRad = [](double deg) -> double {
        auto d = deg - 90;
        if (d < 0) {
            d = 360 + d;
        }
        return d * M_PI / 180.0;
    };

    auto currentMillis = authpp::TimeUtil::getCurrentMilliSeconds();
    auto currentSeconds = currentMillis / 1000;

    auto ts = authpp::TimeUtil::getTotpTimeStep(currentSeconds, timeStep);

    const auto allocation = get_allocation();
    const auto width = allocation.get_width();
    const auto width_2 = width / 2.0;
    const auto height = allocation.get_height();
    const auto height_2 = height / 2.0;
    const Gdk::Rectangle rect(0, 0, width, height);
    auto cr = snapshot->append_cairo(rect);

    if (currentSeconds < ((ts + 1) * timeStep)) {
        auto accountDelayMs = (double)(currentMillis - ts * timeStep * 1000);
        auto deg = (360.0 / (timeStep * 1000)) * (accountDelayMs - 1500);
        Gdk::Cairo::set_source_rgba(cr, foregroundColor);
        cr->move_to(width_2, height_2);
        cr->arc_negative(width_2, height_2, height_2 - 2, degToRad(0), degToRad(deg));
        cr->move_to(width_2, height_2);
        cr->fill();
    }
    cr->arc(width_2, height_2, height_2 - 2, 0, 2 * M_PI);
    cr->stroke();
}

} // authppgtk