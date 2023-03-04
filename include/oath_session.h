#pragma once

#include <string>
#include <vector>

#include "byte_buffer.h"
#include "ccid_connection.h"

namespace authpp {

namespace oath {
    enum class Type : uint8_t {
        HOTP = 0x10,
        TOTP = 0x20
    };

    enum class Algorithm : uint8_t {
        HMAC_SHA1 = 0x01,
        HMAC_SHA256 = 0x02,
        HMAC_SHA512 = 0x03
    };

    struct Credential {
        std::string name;
        Type type;
        Algorithm algorithm;
        std::string code;

        static Credential fromByteBuffer(const ByteBuffer& buffer)
        {
            auto typeAlgo = buffer.getByte(0);
            return Credential {
                std::string(buffer.array() + 1, buffer.array() + buffer.size()),
                static_cast<Type>(typeAlgo & 0xF0),
                static_cast<Algorithm>(typeAlgo & 0x0F),
                ""
            };
        }
    };

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

    class Session {
    public:
        explicit Session(const CcidConnection& connection);

        void listCredentials() const;
        void calculateAll() const;

        const Version& getVersion() const;

    private:
        struct Properties {
            const Version version;
            const std::string name;
            const ByteBuffer challenge;
            const Algorithm algorithm;
        };

        const CcidConnection& connection;
        const Properties properties;

        Properties initProperties(Response&&) const;
    };

} // namespace oath
} // namespace authpp
