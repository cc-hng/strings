#pragma once

#include <exception>
#include <string>
#include <string_view>
#include <type_traits>
#include <stdint.h>
#include <stdio.h>

#define STRINGS_HAS_MEMBER(member)                                                            \
    template <typename T, typename... Args>                                                   \
    struct strings_has_member_##member {                                                      \
    private:                                                                                  \
        template <typename U>                                                                 \
        static auto Check(int)                                                                \
            -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type()); \
        template <typename U>                                                                 \
        static std::false_type Check(...);                                                    \
                                                                                              \
    public:                                                                                   \
        enum { value = std::is_same<decltype(Check<T>(0)), std::true_type>::value };          \
    };                                                                                        \
                                                                                              \
    template <typename F, typename... Args>                                                   \
    constexpr bool strings_has_member_##member##_v =                                          \
        strings_has_member_##member<F, Args...>::value;

STRINGS_HAS_MEMBER(data);
STRINGS_HAS_MEMBER(size);

namespace ss {

// to span
template <typename V, typename Dummy = std::enable_if_t<strings_has_member_data_v<V>
                                                        || strings_has_member_size_v<V>>>
inline std::string_view to_span(const V& v) {
    using T = typename V::value_type;
    static_assert(std::is_same_v<T, char> || std::is_same_v<T, unsigned char>,
                  "Expect type(T) == char");
    return std::string_view((const char*)v.data(), v.size());
}

template <typename T, bool IsChar = std::is_same_v<T, char> || std::is_same_v<T, unsigned char>>
std::enable_if_t<IsChar, std::string_view>  //
to_span(const T* chs) {
    return std::string_view((const char*)chs);
}

// Exception
class input_error : public std::exception {
    const size_t offset_;
    const uint8_t byte_;
    mutable std::string msg_;

public:
    input_error(size_t ofs, uint8_t b) : offset_(ofs), byte_(b) {}

    size_t offset() const { return offset_; }

    const char* what() const noexcept override {
        char buf[1024] = {0};
        auto n = snprintf(buf, 1024, "Input error. offset = %d, byte = %d(%c)", (int)offset_,
                          (int)byte_, (char)byte_);
        msg_   = std::string(buf, n);
        return msg_.c_str();
    }
};

}  // namespace ss
