#include "timer.h"

#include <glibmm/main.h>

#include <libauthpp/logger.h>
#include <libauthpp/time_util.h>

namespace authppgtk {
using TimeUtil = authpp::TimeUtil;
namespace {
    authpp::Logger log("Timer");
}

Timer::Timer(std::function<bool()>&& callback)
    : callback(std::move(callback))
    , connection()
{
}

void Timer::schedule(long delay)
{
    if (connection.connected()) {
        cancel();
    }
    auto currentMs = authpp::TimeUtil::getCurrentMilliSeconds();
    log.d("Next update in {} ({})", delay, TimeUtil::toString(currentMs + delay));
    connection = Glib::signal_timeout().connect(sigc::bind(callback), delay);
}

void Timer::cancel()
{
    auto currentMs = TimeUtil::getCurrentMilliSeconds();
    log.d("Stopped at {}", TimeUtil::toString(currentMs));
    connection.disconnect();
}

} // authppgtk
