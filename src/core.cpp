#include <algorithm>
#include <hwy/contrib/unroller/unroller-inl.h>
#include <hwy/highway.h>
#include <limits.h>
#include <string.h>
#include <strings/core.h>
#include <strings/object.h>

namespace hn = hwy::HWY_NAMESPACE;

static HWY_FULL(uint8_t) _du8;
using vec8_t               = hn::Vec<decltype(_du8)>;
static constexpr size_t N8 = hn::Lanes(_du8);

namespace ss {

namespace detail {

struct UpperUnit : hn::UnrollerUnit<UpperUnit, uint8_t, uint8_t> {
    using TT = hn::ScalableTag<uint8_t>;
    inline static constexpr TT _d{};
    const hn::Vec<TT> _0x61 = hn::Set(_d, 0x61);
    const hn::Vec<TT> _0x7a = hn::Set(_d, 0x7a);
    const hn::Vec<TT> _32   = hn::Set(_d, 32);

    hn::Vec<TT> Func(ptrdiff_t idx, const hn::Vec<TT> xx, const hn::Vec<TT> yy) {
        (void)idx;
        (void)yy;
        auto m = hn::And(hn::Ge(xx, _0x61), hn::Le(xx, _0x7a));
        return hn::IfThenElse(m, hn::Sub(xx, _32), xx);
    }
};

struct LowerUnit : hn::UnrollerUnit<LowerUnit, uint8_t, uint8_t> {
    using TT = hn::ScalableTag<uint8_t>;
    inline static constexpr TT _d{};
    const hn::Vec<TT> _0x41 = hn::Set(_d, 0x41);
    const hn::Vec<TT> _0x5a = hn::Set(_d, 0x5a);
    const hn::Vec<TT> _32   = hn::Set(_d, 32);

    hn::Vec<TT> Func(ptrdiff_t idx, const hn::Vec<TT> xx, const hn::Vec<TT> yy) {
        (void)idx;
        (void)yy;
        auto m = hn::And(hn::Ge(xx, _0x41), hn::Le(xx, _0x5a));
        return hn::IfThenElse(m, hn::Add(xx, _32), xx);
    }
};

inline int mcmp(const void* s1, const void* s2, size_t n) {
#if HWY_COMPILER_MSVC
    return memcmp(s1, s2, n);
#else
    return __builtin_memcmp(s1, s2, n);
#endif
}

}  // namespace detail

namespace {
inline char toupper0(char c) {
    return (c >= 'a' && c <= 'z') ? c - (char)32 : c;
}

inline char tolower0(char c) {
    return (c >= 'A' && c <= 'Z') ? c + (char)32 : c;
}
}  // namespace

std::string str_toupper(std::string_view s) {
    size_t len = s.size();
    std::string out(len, '\0');
#if HWY_COMPILER_MSVC
    size_t mod = len % N8;
    if (len > mod) {
        detail::UpperUnit upperfn;
        hn::Unroller(upperfn, (uint8_t*)s.data(), (uint8_t*)out.data(), len - mod);
    }
    if (mod > 0) {
        int start = len - mod;
        std::transform(s.begin() + start, s.end(), out.data() + start, toupper0);
    }
#else
    std::transform(s.begin(), s.end(), out.data(), toupper0);
#endif
    return out;
}

std::string str_tolower(std::string_view s) {
    size_t len = s.size();
    std::string out(len, '\0');
#if HWY_COMPILER_MSVC
    size_t mod = len % N8;
    if (len > mod) {
        detail::LowerUnit lowerfn;
        hn::Unroller(lowerfn, (uint8_t*)s.data(), (uint8_t*)out.data(), len - mod);
    }
    if (mod > 0) {
        int start = len - mod;
        std::transform(s.begin() + start, s.end(), out.data() + start, tolower0);
    }
#else
    std::transform(s.begin(), s.end(), out.data(), tolower0);
#endif
    return out;
}

std::vector<std::string_view>  //
str_split(std::string_view str, std::string_view delimiter, bool trim) {
#if LC_HAS_MEMMEM
    size_t start       = 0;
    size_t end         = 0;
    size_t dlen        = delimiter.size();
    size_t slen        = str.size();
    const char* ps     = str.data();
    const char* ps_end = ps + slen;
    const char* pd     = delimiter.data();
    std::vector<std::string_view> result;
    result.reserve(16);

    for (;;) {
        auto p = (const char*)memmem(ps, ps_end - ps, pd, dlen);
        if (!p) {
            result.emplace_back(std::string_view(ps, ps_end - ps));
            break;
        }
        result.emplace_back(std::string_view(ps, p - ps));
        ps = p + dlen;
    }

    return result;
#else
    size_t start = 0;
    size_t end   = 0;
    size_t dlen  = delimiter.size();
    std::vector<std::string_view> result;
    result.reserve(16);

    while ((end = str.find(delimiter, start)) != std::string_view::npos) {
        auto s = str.substr(start, end - start);
        result.emplace_back(trim ? str_trim(s) : s);
        start = end + dlen;
    }
    auto s = str.substr(start);
    result.emplace_back(trim ? str_trim(s) : s);
    return result;
#endif
}

std::string str_join(const std::vector<std::string_view>& vs, std::string_view delimiter) {
    int count      = 0;
    size_t dlen    = delimiter.size();
    const char* pd = delimiter.data();
    for (const auto& s : vs) {
        count += s.size();
    }
    count += dlen * (vs.size() - 1);

    std::string out(count, '\0');
    char* pout = out.data();
    bool first = true;
    for (const auto& s : vs) {
        if (!first) {
            hwy::CopyBytes(pd, pout, dlen);
            pout += dlen;
        }
        first      = false;
        size_t len = s.size();
        hwy::CopyBytes(s.data(), pout, len);
        pout += len;
    }
    return out;
}

std::string_view str_trim(std::string_view str) {
    size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }

    size_t end = str.size();
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }

    return str.substr(start, end - start);
}

bool str_starts_with(std::string_view s, std::string_view prefix) {
    const size_t plen = prefix.size();
    return s.size() >= plen && detail::mcmp(s.data(), prefix.data(), plen) == 0;
}

bool str_ends_with(std::string_view s, std::string_view prefix) {
    const size_t slen = s.size();
    const size_t plen = prefix.size();
    const char* ps    = s.data();
    return slen >= plen && detail::mcmp(ps + slen - plen, prefix.data(), plen) == 0;
}

}  // namespace ss
