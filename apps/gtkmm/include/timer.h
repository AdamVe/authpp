#pragma once

#include <functional>

#include <sigc++/connection.h>

namespace authppgtk {

class Timer {
public:
    explicit Timer(std::function<bool()>&& callback);
    void start(long delay);
    void stop();

private:
    std::function<bool()> m_callback;
    sigc::connection m_timeout;
};

} // authppgtk
