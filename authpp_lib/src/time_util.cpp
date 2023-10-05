#include "time_util.h"

#include <chrono>

namespace authpp {
long TimeUtil::getTotpTimeStep(long seconds, int intervalLenSec)
{
    long timeStep = seconds / intervalLenSec;
    return timeStep;
}

long TimeUtil::getCurrentSeconds()
{
    return getCurrentMilliSeconds() / 1000;
}

long TimeUtil::getCurrentMilliSeconds()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

} // namespace authpp
