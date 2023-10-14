#include "time_util.h"

#include <chrono>

#include <iomanip>
#include <sstream>

namespace authpp {
namespace chrono = std::chrono;

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
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return chrono::duration_cast<chrono::milliseconds>(duration).count();
}

std::string TimeUtil::toString(long millis, const std::string& format) {
    chrono::milliseconds dur(millis);
    chrono::time_point<chrono::system_clock> dt(dur);
    auto timeT = chrono::system_clock::to_time_t(dt);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&timeT), format.c_str());
    return ss.str();
}

} // authpp
