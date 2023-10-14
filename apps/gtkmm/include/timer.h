#pragma once

#include <functional>

#include <sigc++/connection.h>

namespace authppgtk {

class Timer {
public:
    Timer();
    void start(long delay, const std::function<bool()>& callback);
    void stop();

private:
    sigc::connection m_timeout;
};

} // authppgtk
