#pragma once

#include <vector>
#include <string>
#include <strings/object.h>
#include <strings/hex.h>
#include <memory>

struct SHA1_CTX;

namespace ss {

std::unique_ptr<SHA1_CTX> sha1_init();
void sha1_update(SHA1_CTX *ctx, const char* buf, size_t len);
std::vector<char> sha1_final(SHA1_CTX *ctx);

std::vector<char>
sha1(const char* buf, size_t len);

inline std::string
sha1sum(const char *buf, size_t len) {
    return hex_encode(sha1(buf, len));
}

template <typename V>
std::vector<char>
sha1(const V& v) {
    auto s = to_span(v);
    return sha1(s.data(), s.size());
}

template <typename V>
std::string
sha1sum(const V& v) {
    auto s = to_span(v);
    return sha1sum(s.data(), s.size());
}


}  // namespace ss
