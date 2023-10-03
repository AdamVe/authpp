#include <gtest/gtest.h>

#include <libauthpp/oath_name_parser.h>

using namespace authpp::oath;

TEST(OathNameParserTest, Invalid)
{
    auto [name, issuer, timeStep] = OathNameParser::parseTotpName("");
    ASSERT_EQ("invalid", name);
    ASSERT_EQ(Credential::DEFAULT_TIME_STEP, timeStep);
    ASSERT_EQ("", issuer);
}

TEST(OathNameParserTest, NameOnly)
{
    auto [name, issuer, timeStep] = OathNameParser::parseTotpName("credential name");
    ASSERT_EQ("credential name", name);
    ASSERT_EQ(Credential::DEFAULT_TIME_STEP, timeStep);
    ASSERT_EQ("", issuer);
}

TEST(OathNameParserTest, NameWithIssuer)
{
    auto [name, issuer, timeStep] = OathNameParser::parseTotpName("issuer name:credential name");
    ASSERT_EQ("credential name", name);
    ASSERT_EQ("issuer name", issuer);
    ASSERT_EQ(Credential::DEFAULT_TIME_STEP, timeStep);
}

TEST(OathNameParserTest, NameOnlyWithTimeStep)
{
    auto [name, issuer, timeStep] = OathNameParser::parseTotpName("10/some name");
    ASSERT_EQ("some name", name);
    ASSERT_EQ(10, timeStep);
    ASSERT_EQ("", issuer);
}

TEST(OathNameParserTest, NameWithIssuerWithTimeStep)
{
    auto [name, issuer, timeStep] = OathNameParser::parseTotpName("16/issuer name:some name");
    ASSERT_EQ("some name", name);
    ASSERT_EQ(16, timeStep);
    ASSERT_EQ("issuer name", issuer);
}

TEST(OathNameParserTest, InvalidTimeStep)
{
    {
        auto [name, issuer, timeStep] = OathNameParser::parseTotpName("/issuer1:some name");
        ASSERT_EQ("some name", name);
        ASSERT_EQ(Credential::DEFAULT_TIME_STEP, timeStep);
        ASSERT_EQ("issuer1", issuer);
    }
    {
        auto [name, issuer, timeStep] = OathNameParser::parseTotpName("sds/:another name");
        ASSERT_EQ("another name", name);
        ASSERT_EQ(Credential::DEFAULT_TIME_STEP, timeStep);
        ASSERT_EQ("", issuer);
    }
    {
        auto [name, issuer, timeStep]
            = OathNameParser::parseTotpName("10000000200200202022/issuer 2:new name::part:1000x");
        ASSERT_EQ("new name::part:1000x", name);
        ASSERT_EQ(Credential::DEFAULT_TIME_STEP, timeStep);
        ASSERT_EQ("issuer 2", issuer);
    }
}