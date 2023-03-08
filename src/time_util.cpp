#include "time_util.h"

#include <chrono>

namespace authpp {
long TimeUtil::getTimeStep()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    long timeStep = seconds / 30;
    return timeStep;
}

long TimeUtil::getTime()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

} // namespace authpp
