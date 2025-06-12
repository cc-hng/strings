#include <gtest/gtest.h>
#include <strings/md5.h>
#include <strings/sha1.h>

using namespace ss;

TEST(strings, hash) {
    EXPECT_EQ("5d41402abc4b2a76b9719d911017c592", md5sum("hello"));
    EXPECT_EQ("aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d", sha1sum("hello"));
}
