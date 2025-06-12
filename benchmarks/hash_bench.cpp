#include "common.h"
#include <strings/md5.h>
#include <strings/sha1.h>
#include <xxh3.h>

static const std::string input =
    "hello,"
    "world111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    "11111111111111111111111111111111111111111111111111111111111111111111111111112222222222222"
    "222222222222222222222222222"
    "333333333333333333333333333333333333333333333333333333333333333"
    "444444444444444444444444444444444444444444444444444444444444444"
    "555555555555555555555555555555555555555555555555555555555555555"
    "666666666666666666666666666666666666666666666666666666666666666"
    "777777777777777777777777777777777777777777777777777777777777777"
    "888888888888888888888888888888888888888888888888888888888888888"
    "999999999999999999999999999999999999999999999999999999999999999"
    "000000000000000000000000000000000000000000000000000000000000000"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaa"
    "aaaa";

static void bench_md5(bench::Bench& b) {
    b.title("md5");
    auto old = b.epochIterations();
    b.minEpochIterations(2048);

    b.run("md5", [&] { bench::doNotOptimizeAway(ss::md5sum(input)); });
    b.run("sha1", [&] { bench::doNotOptimizeAway(ss::sha1sum(input)); });
    b.run("xxh3", [&] { bench::doNotOptimizeAway(XXH3_64bits(input.data(), input.size())); });

    b.minEpochIterations(old);
}
BENCHMARK_REGISTE(bench_md5);
