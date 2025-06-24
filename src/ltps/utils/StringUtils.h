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

} // namespace string_utils