#include "oath_session.h"

#include <cstddef>
#include <map>

#include <openssl/evp.h>
#include <openssl/hmac.h>

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
        response[1],
        response[2],
        parseAlgorithm(response[3])
    };
}

std::vector<Credential> Session::listCredentials() const
{
    std::vector<Credential> credentials;
    Apdu list_apdu(0x00, 0xa1, 0x00, 0x00);
    auto response = connection.send(list_apdu);
    for (int i = 0; i < response.size(); ++i) {
        credentials.push_back(Credential::fromByteBuffer(response[i]));
    }
    return credentials;
}

Credential fromAllDataResponse(const ByteBuffer& nameBuffer, uint8_t codeType, const ByteBuffer& response)
{
    auto name = std::string(nameBuffer.array(), nameBuffer.array() + nameBuffer.size());
    std::string code = "";

    Credential credential {
        name,
        Algorithm::HMAC_SHA1,
        Code::fromByteBuffer(codeType, response)
    };
    return credential;
}

Credential Session::calculateOne(long timeStep, std::string_view name) const
{
    ByteBuffer challenge(8);
    challenge.putLong(timeStep);

    auto challengeSize = static_cast<uint8_t>(challenge.size());
    ByteBuffer calculateData(2 + name.length() + 2 + challengeSize);
    calculateData
        .putByte(0x71)
        .putByte(name.length());

    for (std::size_t i = 0; i < name.length(); ++i) {
        calculateData.putByte(name[i]);
    };
    calculateData.putByte(0x74)
        .putByte(challengeSize)
        .putBytes(challenge);

    Apdu apdu(0x00, 0xa2, 0x00, 0x01, calculateData);
    auto response = connection.send(apdu);
    Credential credential {
        std::string(name),
        Algorithm::HMAC_SHA1,
        Code::fromByteBuffer(response.tag(0), response[0])
    };
    return credential;
}

std::vector<Credential> Session::calculateAll(long timeStep) const
{
    std::vector<Credential> credentials;

    ByteBuffer challenge(8);
    challenge.putLong(timeStep);

    auto challengeSize = static_cast<uint8_t>(challenge.size());
    ByteBuffer calculateData(2 + challengeSize);
    calculateData
        .putByte(0x74)
        .putByte(challengeSize)
        .putBytes(challenge);

    Apdu apdu(0x00, 0xa4, 0x00, 0x01, calculateData);
    auto response = connection.send(apdu);
    for (int i = 0; i < response.size(); i += 2) {
        auto credential = fromAllDataResponse(response[i], response.tag(i + 1), response[i + 1]);
        credentials.push_back(credential);
        log.v("Found {} code:{} ({:02x} {})",
            credential.name,
            credential.code.value,
            response.tag(i + 1),
            response[i + 1]);
    }

    return credentials;
}

ByteBuffer Session::deriveAccessKey(std::string_view password) const
{
    std::size_t accessKeyLength = 16;
    ByteBuffer derivedKey(accessKeyLength * 8);

    PKCS5_PBKDF2_HMAC_SHA1(
        password.data(),
        password.size(),
        properties.salt.array(),
        properties.salt.size(),
        1000,
        accessKeyLength * 8,
        derivedKey.array());

    derivedKey.setSize(accessKeyLength);
    return derivedKey;
}

void Session::unlock(std::string_view password)
{
    auto derivedKey = deriveAccessKey(password);
    if (validate([&derivedKey](const ByteBuffer& challenge) -> ByteBuffer {
            ByteBuffer hmac_sha1(20 * 8);
            unsigned int size;
            HMAC(EVP_sha1(),
                derivedKey.array(),
                derivedKey.size(),
                challenge.array(),
                challenge.size(),
                hmac_sha1.array(),
                &size);
            hmac_sha1.setSize(static_cast<std::size_t>(size));
            return hmac_sha1;
        })) {
        accessKey = derivedKey;
    }
}

bool Session::validate(AccessKeyValidator validator) const
{
    ByteBuffer unlockChallenge(8);
    unlockChallenge.putByte('c');
    unlockChallenge.putByte('h');
    unlockChallenge.putByte('a');
    unlockChallenge.putByte('c');
    unlockChallenge.putByte('h');
    unlockChallenge.putByte('a');
    unlockChallenge.putByte('r');
    unlockChallenge.putByte('c');

    auto challengeResponse = validator(properties.challenge);

    auto validateDataSize = static_cast<uint8_t>(2 + challengeResponse.size() + 2 + unlockChallenge.size());
    ByteBuffer validateData(validateDataSize);
    validateData
        .putByte(0x75)
        .putByte(challengeResponse.size())
        .putBytes(challengeResponse)
        .putByte(0x74)
        .putByte(unlockChallenge.size())
        .putBytes(unlockChallenge);

    Apdu apdu(0x00, 0xa3, 0x00, 0x00, validateData);
    auto response = connection.send(apdu);

    if (response.tag(0) == 0x75) {
        auto clienteChallengeResponse = validator(unlockChallenge);
        return true;
    }

    return false;
}

const Version& Session::getVersion() const
{
    return properties.version;
}

} // namespace authpp
