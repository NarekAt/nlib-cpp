#include "nlib/container/hash_table.h"

#include "gtest/gtest.h"

namespace nlib {

namespace container_internal {

namespace {

TEST(HashTableTest, SimpleTest) {
  HashTable<int> testee(2);

  testee.insert(11);  // 11
  testee.insert(8);   // 11 8
  testee.insert(3);   // 11 8 3

  EXPECT_EQ(8, testee.num_buckets());
  EXPECT_EQ(3, testee.size());  // 11 8 3

  EXPECT_FALSE(testee.empty());

  testee.remove(11);  // 8 3
  EXPECT_FALSE(testee.contains(11));
  EXPECT_TRUE(testee.contains(8));

  testee.remove(8);  // 3
  EXPECT_FALSE(testee.contains(8));

  testee.remove(3);
  EXPECT_TRUE(testee.empty());
}

}  // namespace

}  // namespace container_internal
}  // namespace nlib