#pragma once

#include <string>
using string = std::string;

#include <filesystem>
namespace fs = std::filesystem;

#include "ll/api/i18n/I18n.h"
using ll::operator""_tr;

#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                                                             \
    TypeName(TypeName&&)                  = delete;                                                                    \
    TypeName(TypeName const&)             = delete;                                                                    \
    TypeName& operator=(TypeName&&)       = delete;                                                                    \
    TypeName& operator=(TypeName const&)  = delete;                                                                    \
    TypeName& operator=(TypeName const&&) = delete;
