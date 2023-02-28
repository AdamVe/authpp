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

void Session::calculateAll() const
{
    Apdu apdu(0x00, 0xa4, 0x00, 0x00);
    auto calculate_all_response = connection.send(apdu);
}

const Version& Session::getVersion() const
{
    return properties.version;
}

} // namespace authpp
