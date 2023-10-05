#pragma once

namespace authpp {

class TimeUtil {
public:
    inline static const int DEFAULT_TOTP_INTERVAL { 30 };
    static long getTotpTimeStep(long seconds, int intervalLenSec = DEFAULT_TOTP_INTERVAL);
    static long getCurrentSeconds();
    static long getCurrentMilliSeconds();
};

} // namespace authpp
