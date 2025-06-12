#pragma once

#include <vector>
#include <string>
#include <strings/object.h>
#include <strings/hex.h>

namespace ss {

std::vector<char> md5(const char* buf, size_t len);

inline
std::string md5sum(const char* buf, size_t len) {
    return hex_encode(md5(buf, len));
}

template <typename V>
std::vector<char> md5(const V& v) {
    auto s = to_span(v);
    return md5(s.data(), s.size());
}

template <typename V>
std::string md5sum(const V& v) {
    auto s = to_span(v);
    return md5sum(s.data(), s.size());
}

}  // namespace ss
