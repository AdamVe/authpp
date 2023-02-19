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

    ASSERT_EQ(1, b.getByte());
    ASSERT_EQ(2, b.getByte());
    ASSERT_EQ(3, b.getByte());
}

TEST(BytesTest, ConstructionFromEmptyInitializerList)
{
    Bytes b {};
    ASSERT_EQ(0, b.size());
}

TEST(BytesTest, SetSize)
{
    Bytes b(0);
    b.setSize(100);
    ASSERT_EQ(100, b.size());
}

TEST(BytesTest, PutGetByte)
{
    Bytes b(10);
    b.pointTo(5);
    b.putByte(128);
    b.pointTo(5);
    ASSERT_EQ(128, b.getByte());
}

TEST(BytesTest, PutGetShort)
{
    Bytes b(10);
    b.pointTo(7);
    b.putShort(10400);
    b.pointTo(7);
    ASSERT_EQ(10400, b.getShort());
}

TEST(BytesTest, PutGetInt)
{
    Bytes b(10);
    b.pointTo(2);
    b.putInt(256000);
    b.pointTo(2);
    ASSERT_EQ(256000, b.getInt());
}

TEST(BytesTest, PutGetBytes)
{
    Bytes b({ 10, 9, 8, 7 });

    Bytes b2(20);
    b2.pointTo(5);
    b2.putBytes(b);

    b2.pointTo(5);
    Bytes b3 = b2.getBytes(4);

    b3.pointTo(0);
    ASSERT_EQ(10, b3.getByte());
    ASSERT_EQ(9, b3.getByte());
    ASSERT_EQ(8, b3.getByte());
    ASSERT_EQ(7, b3.getByte());
}

TEST(BytesTest, GetRaw)
{
    Bytes b({ 100, 200, 50, 255 });
    auto* raw { b.getRaw() };

    ASSERT_EQ(100, raw[0]);
    ASSERT_EQ(255, raw[3]);
}
