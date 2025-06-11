#include <fmt/core.h>
#include <hwy/highway.h>
#include <hwy/print-inl.h>
#include <lcrypt/base64.h>
#include <lcrypt/hex.h>

namespace hn = hwy::HWY_NAMESPACE;

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

// std::string input  = "abcdefghijk";
// std::string hex    = lc::hex_encode(input);
// std::string input2 = lc::hex_decode(hex);

template <typename D>
struct hwy_tag_inner;  // 主模板

// ScalableTag
template <typename T, size_t N, int Pow>
struct hwy_tag_inner<hn::Simd<T, N, Pow>> {
    using type = T;
};

template <typename D>
hn::Vec<D> Lt(D d, const hn::Vec<D> x, const hn::Vec<D> y) {
    using T            = typename hwy_tag_inner<D>::type;
    constexpr auto max = hwy::LimitsMax<T>();
    const auto diff    = hn::Sub(hn::Set(d, max), y);
    const auto v       = hn::SaturatedSub(hn::SaturatedAdd(x, diff), hn::Set(d, max - 1));
    return hn::Sub(v, hn::Set(d, 1));  // v = x + (max - y) - (max - 1) = x + (max - y - 1)
}

template <typename D>
hn::Vec<D> Gt(D d, const hn::Vec<D> x, const hn::Vec<D> y) {
    return Lt(d, y, x);
}

template <typename D>
hn::Vec<D> Ne(D d, const hn::Vec<D> x, const hn::Vec<D> y) {
    return hn::Or(Lt(d, x, y), Gt(d, x, y));
}

template <typename D>
hn::Vec<D> Eq(D d, const hn::Vec<D> x, const hn::Vec<D> y) {
    return hn::Not(Ne(d, x, y));
}

int main() {
    std::string s = "Hello, World!";
    // fmt::print("Hello, Base64!\n");

    HWY_FULL(uint8_t) d8;

    HWY_ALIGN static constexpr uint8_t in[64] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                                 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                                 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
                                                 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
                                                 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
                                                 55, 56, 57, 58, 59, 60, 61, 62, 63};

    const auto v1 = hn::LoadU(d8, in);
    auto less     = Lt(d8, v1, hn::Set(d8, 26));
    hn::Print(d8, "v1", v1, 0, 64);
    hn::Print(d8, "less", less, 0, 64);
    hn::Print(d8, "greater", Gt(d8, v1, hn::Set(d8, 26)), 0, 64);
    hn::Print(d8, "ne", Ne(d8, v1, hn::Set(d8, 26)), 0, 64);
    hn::Print(d8, "eq", Eq(d8, v1, hn::Set(d8, 26)), 0, 64);

    // fmt::print("Hex encode 1: {}\n", hex);
    // fmt::print("Hex decode 1: {}\n", input2);
    // fmt::print("Hex encode 2: {}\n", lc::hex_encode(input));
    // fmt::print("hex decode 2: {}\n", lc::hex_decode(hex));

    // HWY_FULL(uint8_t) d8;
    // fmt::print("lanes: {}\n", hn::Lanes(d8));

    // fmt::print("base64 encode: {}\n", lc::base64_encode(input));

    // auto input_base64d = lc::base64_encode(input);
    // fmt::print("hex: {}\n", lc::hex_encode("01234567890123456789012345678912abc"));
    // fmt::print("hex: {}\n", lc::hex_decode("3031323334353637383930313233343536373839303132333435363738393132"));

    //  std::string str = "30313233343536373839303132333435363738393031323334353637383931323031323334353637383930313233343536373839303132333435363738393132";
    //  for (int i = 0; i < str.size(); i += 1) {
    //     auto s0 = str;
    //     s0[i] = 'G';
    //     try {
    //         auto decoded = lc::hex_decode(s0);
    //     } catch (const std::exception& e) {
    //         fmt::print("{}: Error decoding hex: {}\n", i, e.what());
    //     }
    //  }

    fmt::print("base64 encode: {}\n", lc::base64_encode("abcdefghijklmnopqrstuvwxyz0123456789ABCDE"
                                                        "FGHIJKLMNOPQRSTUVWXYZ0123456789a"));
    // fmt::print("base64 encode: {}\n", lc::base64_decode("YWJkZWZhZWRmYWZhZmFmYWZhZmFmYXNmYWZhc2Zkc2FmZGFzeHp2enZhYXNkYWRhZGFzZGFk]Q=="));
    // fmt::print("base64 encode: {}\n", lc::base64_decode(input_base64d));
    // fmt::print("base64 encode: {}\n", lc::base64_decode(lc::base64_encode("a")));
    // fmt::print("base64 encode: {}\n", lc::base64_decode(lc::base64_encode("ab")));
    // fmt::print("base64 encode: {}\n", lc::base64_decode(lc::base64_encode("abc")));
    // fmt::print("base64 decode: {}\n", lc::base64_decode(lc::base64_encode("abcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwxabcdefghijklmnopqrstuvwx")));
    // fmt::print("base64 decode: {}\n", lc::base64_decode(lc::base64_encode("ab")));
    // fmt::print("base64 decode: {}\n", lc::base64_decode(lc::base64_encode("abc")));
    /*
    fmt::print("Hex encode: {}\n", lc::hex_encode(s));
    fmt::print("Hex decode: {}\n", lc::hex_decode(lc::hex_encode(s)));

    auto input_hex = lc::hex_encode(input);
    fmt::print("Hex encode: {}\n", lc::hex_encode(input));
    fmt::print("Hex decode: {}\n", lc::hex_decode(input_hex));
    fmt::print("Is Equal: {}\n", lc::hex_decode(input_hex) == input);
    */
    return 0;
}
