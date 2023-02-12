#pragma once

#include <string>
#include <vector>

#include "byte_array.h"

namespace authpp {

class CcidConnection;

class OathSession {
public:
    struct DataPair {
        std::byte tag;
        ByteArray byte_array;
    };

    using MessageData = std::vector<DataPair>;

    struct Version {
        Version(int major, int minor, int patch)
            : major(major)
            , minor(minor)
            , patch(patch)
        {
        }

        const int major;
        const int minor;
        const int patch;
    };

    enum class Algorithm : int {
        HMAC_SHA1 = 0x01,
        HMAC_SHA256 = 0x02,
        HMAC_SHA512 = 0x03
    };

    OathSession(const CcidConnection& connection);

    void ListCredentials() const;
    void CalculateAll() const;

    const Version& GetVersion() const;

private:
    const CcidConnection& connection;
    MessageData message_data;
    const Version version;
    const std::string name;
    const ByteArray challenge;
    const Algorithm algorithm;
};

} // namespace authpp
