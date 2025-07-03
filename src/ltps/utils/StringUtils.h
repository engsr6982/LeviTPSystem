#pragma once
#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string>


namespace string_utils {


// 获取字符长度(U8)
inline size_t length(std::string const& str) {
    return std::ranges::count_if(str, [](char c) { return (static_cast<unsigned char>(c) & 0xC0) != 0x80; });
}

inline bool isLengthValid(std::string const& str, size_t max_len) { return length(str) <= max_len; }

inline std::string snake_to_pascal(std::string_view snake_str) {
    std::string result;
    result.reserve(snake_str.size());
    bool next_upper = true;
    for (char c : snake_str) {
        if (c == '_') {
            next_upper = true;
        } else if (next_upper) {
            result     += static_cast<char>(std::toupper(c));
            next_upper  = false;
        } else {
            result += static_cast<char>(std::tolower(c));
        }
    }
    return result;
}

} // namespace string_utils