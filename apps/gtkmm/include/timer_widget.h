#pragma once

#include <gtkmm/widget.h>

namespace authppgtk {
class TimerWidget : public Gtk::Widget {
public:
    TimerWidget();
    ~TimerWidget() override;

    void setTimeStep(int ts);

protected:
    void measure_vfunc(Gtk::Orientation orientation, int for_size, int& minimum, int& natural,
        int& minimum_baseline, int& natural_baseline) const override;
    void snapshot_vfunc(const Glib::RefPtr<Gtk::Snapshot>& snapshot) override;

    Gdk::RGBA foregroundColor { 0.0, 0.0, 0.0 };
    int timeStep;
    long lastUpdateMs;
};
} // authppgtk