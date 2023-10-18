#include "timer_widget.h"

#include <gdkmm/general.h>
#include <gtkmm/snapshot.h>

namespace authppgtk {
TimerWidget::TimerWidget()
{
    set_expand(false);

    padding.set_left(0);
    padding.set_right(0);
    padding.set_top(0);
    padding.set_bottom(0);
}

TimerWidget::~TimerWidget() = default;

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
   auto toRad = [](double deg) -> double {
        auto d = deg - 90;
        if (d < 0) {
            d = 360 + d;
        }
        return d * M_PI / 180.0;
    };

    const auto allocation = get_allocation();
    const Gdk::Rectangle rect(0, 0, allocation.get_width(), allocation.get_height());
    auto cr = snapshot->append_cairo(rect);
    cr->set_antialias(Cairo::ANTIALIAS_SUBPIXEL);
    Gdk::Cairo::set_source_rgba(cr, foregroundColor);
    cr->translate(padding.get_left(), padding.get_top());
    cr->move_to(allocation.get_width() / 2.0, allocation.get_height() / 2.0);
    cr->arc_negative(allocation.get_width() / 2.0, allocation.get_height() / 2.0,
        allocation.get_height() / 2.0 - 2, toRad(0), toRad(90));
    cr->move_to(allocation.get_width() / 2.0, allocation.get_height() / 2.0);
    cr->fill();
    cr->arc(allocation.get_width() / 2.0, allocation.get_height() / 2.0,
        allocation.get_height() / 2.0 - 2, 0, 2 * M_PI);
    cr->stroke();
}

} // authppgtk