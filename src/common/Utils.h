#pragma once
#include "Global.h"
#include "ll/api/form/CustomForm.h"
#include <vector>

namespace tps {


string dimid2str(int dimid);

template <typename T>
string join(std::vector<T> const& vec, char const* delim = ", ");

template <typename T>
bool some(std::vector<T> const& vec, T const& arg);

template <typename... Args>
string vformat(std::string_view fmt, Args... args);


// Date
std::time_t tm2time(std::tm const& tm);
std::tm     time2tm(std::time_t const& time);
string      tm2str(std::tm const& tm);

std::time_t getCurrentDate();
std::time_t getFutureDate(int sec);

void PrintCustomFormResult(ll::form::CustomFormResult const&);


} // namespace tps