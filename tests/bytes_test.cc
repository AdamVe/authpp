#include <gtest/gtest.h>

#include <bytes.h>

using namespace authpp;

TEST(BytesTest, Initiaalization)
{
    Bytes b(10);
    ASSERT_EQ(10, b.size());
}
