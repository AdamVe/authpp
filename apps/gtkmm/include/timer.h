#pragma once

#include <functional>

#include <sigc++/connection.h>

namespace authppgtk {

class Timer {
public:
    explicit Timer(std::function<bool()>&& callback);
    void schedule(long delay);
    void cancel();

private:
    std::function<bool()> callback;
    sigc::connection connection;
};

} // authppgtk
