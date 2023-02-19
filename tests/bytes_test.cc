#include <gtest/gtest.h>

#include <bytes.h>

using namespace authpp;

TEST(BytesTest, Construction)
{
    Bytes b(10);
    ASSERT_EQ(10, b.size());
}

TEST(BytesTest, ConstructionFromInitializerList)
{
    Bytes b({ 1, 2, 3 });
    ASSERT_EQ(3, b.size());

    uint8_t value;
    b.getChar(value);
    ASSERT_EQ(1, value);
    b.getChar(value);
    ASSERT_EQ(2, value);
    b.getChar(value);
    ASSERT_EQ(3, value);

    Bytes b2 {};
    ASSERT_EQ(0, b2.size());
}

TEST(BytesTest, SetSize)
{
    Bytes b(0);
    b.setSize(100);
    ASSERT_EQ(100, b.size());
}

TEST(BytesTest, PutGetChar)
{
    Bytes b(10);
    b.pointTo(5);
    b.putChar(128);
    b.pointTo(5);
    uint8_t value;
    b.getChar(value);
    ASSERT_EQ(128, value);
}

TEST(BytesTest, PutGetI16)
{
    Bytes b(10);
    b.pointTo(7);
    b.putI16(10400);
    b.pointTo(7);
    uint16_t value;
    b.getI16(value);
    ASSERT_EQ(10400, value);
}

TEST(BytesTest, PutGetI32)
{
    Bytes b(10);
    b.pointTo(2);
    b.putI32(256000);
    b.pointTo(2);
    uint32_t value;
    b.getI32(value);
    ASSERT_EQ(256000, value);
}

TEST(BytesTest, PutGetBytes)
{
    Bytes b({ 10, 9, 8, 7 });

    Bytes b2(20);
    b2.pointTo(5);
    b2.putBytes(b);

    Bytes b3(4);
    b2.pointTo(5);
    b2.getBytes(b3);

    uint8_t value;
    b3.pointTo(0);
    b3.getChar(value);
    ASSERT_EQ(10, value);
    b3.getChar(value);
    ASSERT_EQ(9, value);
    b3.getChar(value);
    ASSERT_EQ(8, value);
    b3.getChar(value);
    ASSERT_EQ(7, value);
}

TEST(BytesTest, GetRaw)
{
    Bytes b({ 100, 200, 50, 255 });
    auto* raw { b.getRaw() };

    ASSERT_EQ(100, raw[0]);
    ASSERT_EQ(255, raw[3]);
}
