#include "oath_session.h"

#include <cstddef>
#include <map>
#include <sstream>

#include "apdu.h"
#include "ccid_connection.h"
#include "fmt/fmt_byte_buffer.h"
#include "fmt/fmt_oath.h"
#include "logger.h"
#include "message.h"
#include "util.h"

namespace authpp::oath {

namespace {
    Logger log("OathSession");
}

const ByteBuffer kOathId { 0xa0, 0x00, 0x00, 0x05, 0x27, 0x21, 0x01, 0x01 };

Response select(const CcidConnection& connection, const ByteBuffer& app_id)
{
    Apdu select_oath(0x00, 0xa4, 0x04, 0x00, app_id);
    auto tags = connection.send(select_oath);

    if (tags.size() < 0) {
        log.e("Invalid data: parse failed");
    }

    return tags;
}

Session::Session(const CcidConnection& connection)
    : connection(connection)
    , properties(initProperties(select(connection, kOathId)))
{
    log.d("Opened OathSession version {} / algo {} / challenge {}", properties.version,
        std::to_underlying(properties.algorithm), properties.challenge);
}

Session::Properties Session::initProperties(Response&& response) const
{
    auto parseVersion = [](auto&& buffer) -> Version {
        if (buffer.size() > 2) {
            return Version(buffer.getByte(0), buffer.getByte(1), buffer.getByte(2));
        }
        return Version(0, 0, 0);
    };

    auto parseName = [](auto&& buffer) -> std::string {
        return "TODO";
    };

    auto parseAlgorithm = [](auto&& buffer) -> Algorithm {
        if (buffer.size() == 0) {
            return Algorithm::HMAC_SHA1;
        }
        auto type = buffer.getByte(0);

        if (type == 0x02) {
            return Algorithm::HMAC_SHA256;
        } else if (type == 0x03) {
            return Algorithm::HMAC_SHA512;
        }

        return Algorithm::HMAC_SHA1;
    };

    return Properties {
        parseVersion(response[0]),
        parseName(response[1]),
        response[2],
        parseAlgorithm(response[3])
    };
}

void Session::listCredentials() const
{
    Apdu list_apdu(0x00, 0xa1, 0x00, 0x00);
    auto response = connection.send(list_apdu);
    for (int i = 0; i < response.size(); ++i) {
        auto credential = Credential::fromByteBuffer(response[i]);
        log.d("Found {}", credential);
    }
}

Credential fromAllDataResponse(const ByteBuffer& nameBuffer, uint8_t codeType, const ByteBuffer& response)
{
    auto name = std::string(nameBuffer.array(), nameBuffer.array() + nameBuffer.size());
    auto type = Type::HOTP;
    std::string code = "";
    switch (codeType) {
    case 0x77:
        type = Type::HOTP;
        break;
    case 0x7c:
        type = Type::TOTP;
        break;
    case 0x75: {
        type = Type::TOTP;
        uint8_t digits = response.getByte(0);
        code = std::string(response.array() + 1, response.array() + response.size());
        break;
    }
    case 0x76: {
        type = Type::TOTP;
        uint8_t digits = response.getByte(0);
        code = std::string(response.array() + 1, response.array() + response.size());
        break;
    }
    }

    Credential credential {
        name,
        type,
        Algorithm::HMAC_SHA1,
        code
    };
    return credential;
}

void Session::calculateAll() const
{
    auto challengeSize = static_cast<uint8_t>(properties.challenge.size());
    ByteBuffer calculateData(2 + challengeSize);
    calculateData
        .putByte(0x74)
        .putByte(challengeSize)
        .putBytes(properties.challenge);

    Apdu apdu(0x00, 0xa4, 0x00, 0x00, calculateData);
    auto response = connection.send(apdu);
    for (int i = 0; i < response.size(); i += 2) {
        auto credential = fromAllDataResponse(response[i], response.tag(i + 1), response[i + 1]);
        log.d("Found {} code:{} ({:02x} {})", credential.name, credential.code, response.tag(i), response[i]);
    }
}

const Version& Session::getVersion() const
{
    return properties.version;
}

} // namespace authpp
