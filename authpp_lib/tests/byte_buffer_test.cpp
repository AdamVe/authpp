#include <gtest/gtest.h>

#include <libauthpp/byte_buffer.h>

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

    ASSERT_EQ(1, b.get<uint8_t>(0));
    ASSERT_EQ(2, b.get<uint8_t>(1));
    ASSERT_EQ(3, b.get<uint8_t>(2));
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
    b.put((uint8_t)128);
    ASSERT_EQ(128, b.get<uint8_t>(5));
}

TEST(BytesTest, PutGetShort)
{
    ByteBuffer b(10);
    b.pointTo(7);
    b.put((uint16_t)10400);
    ASSERT_EQ(10400, b.get<uint16_t>(7));
}

TEST(BytesTest, PutGetInt)
{
    ByteBuffer b(10);
    b.pointTo(2);
    b.put((uint32_t)256000);
    ASSERT_EQ(256000, b.get<uint32_t>(2));
}

TEST(BytesTest, PutGetLong)
{
    ByteBuffer b(10);
    b.pointTo(2);
    b.put((uint64_t)4756927171371729432ULL);
    ASSERT_EQ(4756927171371729432ULL, b.get<uint64_t>(2));
}

TEST(BytesTest, PutGetBEShort)
{
    ByteBuffer b(10);
    b.setByteOrder(std::endian::big);
    b.pointTo(7);
    b.put((uint16_t)10400);

    auto int16val = b.get(7, 2);
    ASSERT_EQ(10400, byteswap(int16val.get<uint16_t>(0)));
}

TEST(BytesTest, PutGetBEInt)
{
    ByteBuffer b(10);
    b.setByteOrder(std::endian::big);
    b.pointTo(2);
    b.put((uint32_t)256000);

    auto int32val = b.get(2, 4);
    ASSERT_EQ(256000, byteswap(int32val.get<uint32_t>(0)));
}

TEST(BytesTest, PutGetBELong)
{
    ByteBuffer b(10);
    b.setByteOrder(std::endian::big);
    b.pointTo(2);
    b.put((uint64_t)4756927171371729432ULL);

    auto int64val = b.get(2, 8);
    ASSERT_EQ(4756927171371729432ULL, byteswap(int64val.get<uint64_t>(0)));
}

TEST(BytesTest, PutGetBytes)
{
    ByteBuffer b({ 10, 9, 8, 7 });

    ByteBuffer b2(20);
    b2.pointTo(5);
    b2.put(b);

    ByteBuffer b3 = b2.get(5, 4);

    ASSERT_EQ(10, b3.get<uint8_t>(0));
    ASSERT_EQ(9, b3.get<uint8_t>(1));
    ASSERT_EQ(8, b3.get<uint8_t>(2));
    ASSERT_EQ(7, b3.get<uint8_t>(3));
}

TEST(BytesTest, Array)
{
    ByteBuffer b({ 100, 200, 50, 255 });
    auto* a { b.array() };

    ASSERT_EQ(100, a[0]);
    ASSERT_EQ(255, a[3]);
}
