#pragma once

#include <string>

namespace authpp {

class TimeUtil {
public:
    inline static const int DEFAULT_TOTP_INTERVAL { 30 };
    static long getTotpTimeStep(long seconds, int intervalLenSec = DEFAULT_TOTP_INTERVAL);
    static long getCurrentSeconds();
    static long getCurrentMilliSeconds();
    static std::string toString(long millis, const std::string& format = "%X");
};

} // authpp
