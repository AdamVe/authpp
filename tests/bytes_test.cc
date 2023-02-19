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

    ASSERT_EQ(1, b.uint8());
    ASSERT_EQ(2, b.uint8());
    ASSERT_EQ(3, b.uint8());

    Bytes b2 {};
    ASSERT_EQ(0, b2.size());
}

TEST(BytesTest, SetSize)
{
    Bytes b(0);
    b.setSize(100);
    ASSERT_EQ(100, b.size());
}

TEST(BytesTest, UInt8)
{
    Bytes b(10);
    b.pointTo(5);
    b.uint8(128);
    b.pointTo(5);
    ASSERT_EQ(128, b.uint8());
}

TEST(BytesTest, UInt16)
{
    Bytes b(10);
    b.pointTo(7);
    b.uint16(10400);
    b.pointTo(7);
    ASSERT_EQ(10400, b.uint16());
}

TEST(BytesTest, UInt32)
{
    Bytes b(10);
    b.pointTo(2);
    b.uint32(256000);
    b.pointTo(2);
    ASSERT_EQ(256000, b.uint32());
}

TEST(BytesTest, PutGetBytes)
{
    Bytes b({ 10, 9, 8, 7 });

    Bytes b2(20);
    b2.pointTo(5);
    b2.set(b);

    b2.pointTo(5);
    Bytes b3 = b2.get(4);

    b3.pointTo(0);
    ASSERT_EQ(10, b3.uint8());
    ASSERT_EQ(9, b3.uint8());
    ASSERT_EQ(8, b3.uint8());
    ASSERT_EQ(7, b3.uint8());
}

TEST(BytesTest, GetRaw)
{
    Bytes b({ 100, 200, 50, 255 });
    auto* raw { b.getRaw() };

    ASSERT_EQ(100, raw[0]);
    ASSERT_EQ(255, raw[3]);
}
