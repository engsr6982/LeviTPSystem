#pragma once
#include <filesystem>
#include <string>


using string = std::string;
namespace fs = std::filesystem;


#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                                                             \
    TypeName(TypeName&&)                  = delete;                                                                    \
    TypeName(TypeName const&)             = delete;                                                                    \
    TypeName& operator=(TypeName&&)       = delete;                                                                    \
    TypeName& operator=(TypeName const&)  = delete;                                                                    \
    TypeName& operator=(TypeName const&&) = delete;
