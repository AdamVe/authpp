#include "timer.h"

#include <glibmm/main.h>

#include <libauthpp/logger.h>
#include <libauthpp/time_util.h>

namespace authppgtk {
using TimeUtil = authpp::TimeUtil;
namespace {
    authpp::Logger log("Timer");
}

Timer::Timer()
    : m_timeout()
{
}

void Timer::start(long delay, const std::function<bool()>& callback)
{
    if (m_timeout.connected()) {
        stop();
    }
    auto currentMs = authpp::TimeUtil::getCurrentMilliSeconds();
    log.d("Next update in {} ({})", delay, TimeUtil::toString(currentMs + delay));
    m_timeout = Glib::signal_timeout().connect(sigc::bind(callback), delay);
}

void Timer::stop()
{
    auto currentMs = TimeUtil::getCurrentMilliSeconds();
    log.d("Stopped at {}", TimeUtil::toString(currentMs));
    m_timeout.disconnect();
}

} // namespace authppgtk
