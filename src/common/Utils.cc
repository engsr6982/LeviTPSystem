#include "Utils.h"
#include "ll/api/form/CustomForm.h"
#include "mc/world/level/dimension/VanillaDimensions.h"
#include <ctime>

namespace tps {


string dimid2str(int dimid) {
    auto dim = VanillaDimensions::fromSerializedInt(dimid);
    return VanillaDimensions::toString(dim);
}

template <typename T>
string join(std::vector<T> const& vec, char const* delim) {
    std::ostringstream oss;
    for (std::size_t i = 0; i < vec.size(); ++i) {
        if (i != 0 && i != (vec.size() - 1)) oss << delim;
        oss << vec[i];
    }
    return oss.str();
}

template <typename T>
bool some(std::vector<T> const& vec, T const& arg) {
    if (vec.empty()) return false;
    return std::find(vec.begin(), vec.end(), arg) != vec.end();
}

template <typename... Args>
string vformat(std::string_view fmt, Args... args) {
    try {
        return fmt::vformat(fmt, fmt::make_format_args(args...));
    } catch (...) {
        return string(fmt);
    }
}


std::time_t tm2time(std::tm const& tm) { return std::mktime(const_cast<std::tm*>(&tm)); }
std::tm     time2tm(std::time_t const& time) {
    std::tm tm;
    localtime_s(&tm, &time);
    return tm;
}
string tm2str(std::tm const& tm) {
    return vformat("{}-{}-{} {}:{}:{}", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

std::time_t getCurrentDate() { return std::time(nullptr); }
std::time_t getFutureDate(int sec) { return std::time(nullptr) + sec; }


#ifdef DEBUG
void PrintCustomFormResult(ll::form::CustomFormResult const& res) {
    std::cout << "\033[0m\033[1;35m"
              << "======================================================================================" << "\033[0m"
              << std::endl;
    for (auto [name, result] : *res) {
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
}
#else
void PrintCustomFormResult(ll::form::CustomFormResult const&) {}
#endif


} // namespace tps