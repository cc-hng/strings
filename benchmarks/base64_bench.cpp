#include "common.h"
#include <stdexcept>
#include <strings/base64.h>

using namespace ss;

#define SMALL_CHUNK 256

std::string base64__marshal(const std::string& what) {
    static const char* encoding =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t sz           = what.size();
    const uint8_t* text = (const uint8_t*)what.data();
    int encode_sz       = (sz + 2) / 3 * 4;
    char tmp[SMALL_CHUNK];
    char* buffer = tmp;
    if (encode_sz > SMALL_CHUNK) {
        buffer = new char[encode_sz];
    }
    int i, j;
    j = 0;
    for (i = 0; i < (int)sz - 2; i += 3) {
        uint32_t v    = text[i] << 16 | text[i + 1] << 8 | text[i + 2];
        buffer[j]     = encoding[v >> 18];
        buffer[j + 1] = encoding[(v >> 12) & 0x3f];
        buffer[j + 2] = encoding[(v >> 6) & 0x3f];
        buffer[j + 3] = encoding[(v) & 0x3f];
        j += 4;
    }
    int padding = sz - i;
    uint32_t v;
    switch (padding) {
    case 1:
        v             = text[i];
        buffer[j]     = encoding[v >> 2];
        buffer[j + 1] = encoding[(v & 3) << 4];
        buffer[j + 2] = '=';
        buffer[j + 3] = '=';
        break;
    case 2:
        v             = text[i] << 8 | text[i + 1];
        buffer[j]     = encoding[v >> 10];
        buffer[j + 1] = encoding[(v >> 4) & 0x3f];
        buffer[j + 2] = encoding[(v & 0xf) << 2];
        buffer[j + 3] = '=';
        break;
    }
    std::string r(buffer, buffer + encode_sz);
    if (encode_sz > SMALL_CHUNK) {
        delete[] buffer;
    }
    return r;
}

static inline int b64index(uint8_t c) {
    static const int decoding[] = {62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1,
                                   -1, -1, -2, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                   10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                                   -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
                                   36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
    int decoding_size           = sizeof(decoding) / sizeof(decoding[0]);
    if (c < 43) {
        return -1;
    }
    c -= 43;
    if (c >= decoding_size) return -1;
    return decoding[c];
}

std::string base64__unmarshal(const std::string& what) {
    size_t sz           = what.size();
    const uint8_t* text = (const uint8_t*)what.data();
    int decode_sz       = (sz + 3) / 4 * 3;
    char tmp[SMALL_CHUNK];
    char* buffer = tmp;
    if (decode_sz > SMALL_CHUNK) {
        buffer = new char[decode_sz];
    }
    int i, j;
    int output = 0;
    for (i = 0; i < sz;) {
        int padding = 0;
        int c[4];
        for (j = 0; j < 4;) {
            if (i >= sz) {
                throw std::runtime_error("Invalid base64 text");
            }
            c[j] = b64index(text[i]);
            if (c[j] == -1) {
                ++i;
                continue;
            }
            if (c[j] == -2) {
                ++padding;
            }
            ++i;
            ++j;
        }
        uint32_t v;
        switch (padding) {
        case 0:
            v                  = (unsigned)c[0] << 18 | c[1] << 12 | c[2] << 6 | c[3];
            buffer[output]     = v >> 16;
            buffer[output + 1] = (v >> 8) & 0xff;
            buffer[output + 2] = v & 0xff;
            output += 3;
            break;
        case 1:
            if (c[3] != -2 || (c[2] & 3) != 0) {
                if (decode_sz > SMALL_CHUNK) {
                    delete[] buffer;
                }
                throw std::runtime_error("Invalid base64 text");
            }
            v                  = (unsigned)c[0] << 10 | c[1] << 4 | c[2] >> 2;
            buffer[output]     = v >> 8;
            buffer[output + 1] = v & 0xff;
            output += 2;
            break;
        case 2:
            if (c[3] != -2 || c[2] != -2 || (c[1] & 0xf) != 0) {
                if (decode_sz > SMALL_CHUNK) {
                    delete[] buffer;
                }
                throw std::runtime_error("Invalid base64 text");
            }
            v              = (unsigned)c[0] << 2 | c[1] >> 4;
            buffer[output] = v;
            ++output;
            break;
        default:
            if (decode_sz > SMALL_CHUNK) {
                delete[] buffer;
            }
            throw std::runtime_error("Invalid base64 text");
        }
    }
    std::string r(buffer, buffer + output);
    if (decode_sz > SMALL_CHUNK) {
        delete[] buffer;
    }
    return r;
}

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
static std::string input_base64 = base64_encode(input);

static void bench_base64(bench::Bench& b) {
    b.title("base64");
    auto old = b.epochIterations();
    b.minEpochIterations(20480);

    b.run("base64::encode(simd)", [&] { bench::doNotOptimizeAway(base64_encode(input)); });
    b.run("base64::encode", [&] { bench::doNotOptimizeAway(base64__marshal(input)); });
    b.run("base64::decode(simd)", [&] { bench::doNotOptimizeAway(base64_decode(input_base64)); });
    b.run("base64::decode", [&] { bench::doNotOptimizeAway(base64__unmarshal(input_base64)); });

    b.minEpochIterations(old);
}
BENCHMARK_REGISTE(bench_base64);
