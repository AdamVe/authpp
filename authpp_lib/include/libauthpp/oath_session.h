#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "byte_buffer.h"
#include "ccid_connection.h"
#include "logger.h"

namespace authpp::oath {
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

    static Code fromByteBuffer(uint8_t type, const ByteBuffer& byteBuffer);
};

struct Credential {

    inline static const int DEFAULT_TIME_STEP { 30 };

    std::string name;
    std::string issuer;
    Algorithm algorithm;
    Code code;
    int timeStep { DEFAULT_TIME_STEP };

    static Credential fromByteBuffer(const ByteBuffer& buffer);

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

    [[nodiscard]] Credential calculateOne(long timeStep, std::string_view name) const;
    [[nodiscard]] std::vector<Credential> listCredentials() const;
    [[nodiscard]] std::vector<Credential> calculateAll(long timeStep) const;

    void unlock(std::string_view password);
    [[maybe_unused]] [[nodiscard]] const Version& getVersion() const;

private:
    using AccessKeyValidator = std::function<ByteBuffer(const ByteBuffer&)>;
    struct Properties {
        const Version version;
        const ByteBuffer salt;
        const ByteBuffer challenge;
        const Algorithm algorithm;
    };

    static Properties initProperties(Response&&);
    [[nodiscard]] ByteBuffer deriveAccessKey(std::string_view password) const;
    [[nodiscard]] bool validate(const AccessKeyValidator& validator) const;

    const CcidConnection& connection;
    const Properties properties;

    ByteBuffer accessKey;
};

} // namespace authpp::oath
