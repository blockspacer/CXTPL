#include <algorithm>
#include <random>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

TEST(offset_table, big_values)
{
    std::vector<std::size_t> values = {0, 213, 12'148'409'321};
    ASSERT_EQ(0, values[0]);
}

}  // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}