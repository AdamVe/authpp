#include <gtest/gtest.h>

#include <byte_buffer.h>

using namespace authpp;

TEST(BytesTest, Construction)
{
    ByteBuffer b(10);
    ASSERT_EQ(10, b.size());
}

TEST(BytesTest, ConstructionFromInitializerList)
{
    ByteBuffer b({ 1, 2, 3 });
    ASSERT_EQ(3, b.size());

    ASSERT_EQ(1, b.getByte(0));
    ASSERT_EQ(2, b.getByte(1));
    ASSERT_EQ(3, b.getByte(2));
}

TEST(BytesTest, ConstructionFromEmptyInitializerList)
{
    ByteBuffer b {};
    ASSERT_EQ(0, b.size());
}

TEST(BytesTest, SetSize)
{
    ByteBuffer b(0);
    b.setSize(100);
    ASSERT_EQ(100, b.size());
}

TEST(BytesTest, PutGetByte)
{
    ByteBuffer b(10);
    b.pointTo(5);
    b.putByte(128);
    ASSERT_EQ(128, b.getByte(5));
}

TEST(BytesTest, PutGetShort)
{
    ByteBuffer b(10);
    b.pointTo(7);
    b.putShort(10400);
    ASSERT_EQ(10400, b.getShort(7));
}

TEST(BytesTest, PutGetInt)
{
    ByteBuffer b(10);
    b.pointTo(2);
    b.putInt(256000);
    ASSERT_EQ(256000, b.getInt(2));
}

TEST(BytesTest, PutGetBytes)
{
    ByteBuffer b({ 10, 9, 8, 7 });

    ByteBuffer b2(20);
    b2.pointTo(5);
    b2.putBytes(b);

    ByteBuffer b3 = b2.getBytes(5, 4);

    ASSERT_EQ(10, b3.getByte(0));
    ASSERT_EQ(9, b3.getByte(1));
    ASSERT_EQ(8, b3.getByte(2));
    ASSERT_EQ(7, b3.getByte(3));
}

TEST(BytesTest, Array)
{
    ByteBuffer b({ 100, 200, 50, 255 });
    auto* a { b.array() };

    ASSERT_EQ(100, a[0]);
    ASSERT_EQ(255, a[3]);
}
