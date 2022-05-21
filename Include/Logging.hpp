#pragma once

#include <bits/chrono.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <ratio>
#include <vector>

#include "fmt/format.h"

#define KAEDE_ERRO
#define KAEDE_WARN
#define KAEDE_INFO

namespace osu_request::logging
{
    void print(std::string_view _tag, std::string_view _format, auto&&... _args)
    {
    #if OSU_MAN_DEBUG_BUILD
        const auto clock       = std::chrono::system_clock::now();
        const auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.time_since_epoch()).count();
        fmt::print("{} {} {}", tag, currentTime, fmt::format(format, args...));
    #else
        // FIXME: write these logs to a file.
    #endif
    }

    void error(std::string_view _format, auto&&... _args)
    {
        print("ERROR", _format, _args...);
    }

    void warn(std::string_view _format, auto&&... _args)
    {
        print("WARN", _format, _args...);
    }

    void info(std::string_view _format, auto&&... _args)
    {
        print("INFO", _format, _args...);
    }
}