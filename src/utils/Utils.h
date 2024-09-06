#pragma once
#include "entry/Entry.h"
#include <fmt/format.h>
#include <iostream>
#include <ll/api/form/CustomForm.h>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>


namespace tps::utils {

using string = std::string;

inline int    string2int(const string& str) { return std::stoi(str); }
inline float  string2float(const string& str) { return std::stof(str); }
inline double string2double(const string& str) { return std::stod(str); }

inline string join(const std::vector<string>& vec, const string splitter = ", ") {
    if (vec.empty()) return "";
    return std::accumulate(
        std::next(vec.begin()),
        vec.end(),
        vec[0],
        [splitter](const string& a, const string& b) -> string { return a + splitter + b; }
    );
}

inline string join(const std::vector<int>& vec, const string splitter = ", ") {
    if (vec.empty()) return "";
    return std::accumulate(
        std::next(vec.begin()),
        vec.end(),
        std::to_string(vec[0]),
        [splitter](const string& a, const int& b) -> string { return a + splitter + std::to_string(b); }
    );
}

template <typename... Args>
inline string format(const string& fmt, Args... args) {
    try {
        return fmt::vformat(fmt, fmt::make_format_args(args...));
    } catch (const std::exception& e) {
        std::cerr << "\033[31m" << "[C++ Exception] Formatting string \"" << fmt << "\" failed: " << e.what()
                  << "\033[0m" << std::endl;
        return fmt;
    }
}

inline string toJson(const std::unordered_map<string, double>& map) {
    string json = "{";
    for (const auto& pair : map) {
        json += "\"" + pair.first + "\":" + std::to_string(pair.second) + ",";
    }
    json[json.size() - 1] = '}';
    return json;
}

inline string toJson(const std::unordered_map<string, int>& map) {
    string json = "{";
    for (const auto& pair : map) {
        json += "\"" + pair.first + "\":" + std::to_string(pair.second) + ",";
    }
    json[json.size() - 1] = '}';
    return json;
}

template <typename T>
inline bool some(const std::vector<T>& vec, const T& val) {
    if (vec.empty()) {
        return false;
    }
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}


inline void DebugFormPrint(const ll::form::CustomFormResult& dt) {
#ifdef DEBUG
    std::cout << "\033[0m\033[1;35m"
              << "======================================================================================" << "\033[0m"
              << std::endl;
    for (auto [name, result] : *dt) {
        static auto logDebugResult = [&](const ll::form::CustomFormElementResult& var) {
            if (std::holds_alternative<uint64_t>(var)) {
                std::cout << "\033[0m\033[1;33m" << "[CustomForm Debug] " << "\033[0m\033[1;32m" << name
                          << "\033[0m\033[1;35m    " << std::get<uint64_t>(var) << "    \033[0m\033[1;36muint64_t"
                          << "\033[0m" << std::endl;
            } else if (std::holds_alternative<double>(var)) {
                std::cout << "\033[0m\033[1;33m" << "[CustomForm Debug] " << "\033[0m\033[1;32m" << name
                          << "\033[0m\033[1;35m    " << std::get<double>(var) << "    \033[0m\033[1;36mdouble"
                          << "\033[0m" << std::endl;
            } else if (std::holds_alternative<std::string>(var)) {
                std::cout << "\033[0m\033[1;33m" << "[CustomForm Debug] " << "\033[0m\033[1;32m" << name
                          << "\033[0m\033[1;35m    " << std::get<std::string>(var) << "    \033[0m\033[1;36mstring"
                          << "\033[0m" << std::endl;
            }
        };
        logDebugResult(result);
    }
#endif
}

} // namespace tps::utils