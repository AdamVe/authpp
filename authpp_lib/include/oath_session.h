#pragma once

#include <functional>
#include <sstream>
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

    struct Code {
        Type type;
        uint8_t digits;
        std::string value;

        static Code fromByteBuffer(uint8_t type, const ByteBuffer& byteBuffer)
        {
            Type t;
            uint8_t digits;
            std::string val;
            switch (type) {
            case 0x77:
                t = Type::HOTP;
                break;
            case 0x7c:
                t = Type::TOTP;
                break;
            case 0x75: {
                t = Type::TOTP;
                // TODO
                break;
            }
            case 0x76: {
                t = Type::TOTP;
                digits = byteBuffer.get<uint8_t>(0);
                auto codeValue = byteBuffer.get<uint16_t>(1);
                std::stringstream ss;
                ss << codeValue;
                val = ss.str();
                break;
            }
            }
            return { t, digits, val };
        }
    };

    struct Credential {
        std::string name;
        Algorithm algorithm;
        Code code;

        static Credential fromByteBuffer(const ByteBuffer& buffer)
        {
            auto typeAlgo = buffer.get<uint8_t>(0);
            return Credential {
                std::string(buffer.array() + 1, buffer.array() + buffer.size()),
                static_cast<Algorithm>(typeAlgo & 0x0F),
                Code {
                    static_cast<Type>(typeAlgo & 0xF0),
                }
            };
        }

        static struct {
            bool operator()(const oath::Credential& lhs, const oath::Credential& rhs) const
            {
                return lhs.name < rhs.name;
            }
        } compareByName;
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

        Credential calculateOne(long timeStep, std::string_view name) const;
        std::vector<Credential> listCredentials() const;
        std::vector<Credential> calculateAll(long timeStep) const;

        void unlock(std::string_view password);
        const Version& getVersion() const;

    private:
        using AccessKeyValidator = std::function<ByteBuffer(const ByteBuffer&)>;
        struct Properties {
            const Version version;
            const ByteBuffer salt;
            const ByteBuffer challenge;
            const Algorithm algorithm;
        };

        Properties initProperties(Response&&) const;
        ByteBuffer deriveAccessKey(std::string_view password) const;
        bool validate(AccessKeyValidator validator) const;

        const CcidConnection& connection;
        const Properties properties;

        ByteBuffer accessKey;
    };

} // namespace oath
} // namespace authpp
