#pragma once

#include <bits/chrono.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <ratio>
#include <vector>

#include "fmt/format.h"

namespace osu_request::logging
{
    void print(std::string_view _tag, std::string_view _format, auto&&... _args)
    {
    #if DEBUG_BUILD
        const auto clock       = std::chrono::system_clock::now();
        const auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(clock.time_since_epoch()).count();
        fmt::print("{} {} {}", _tag, currentTime, fmt::format(fmt::runtime(_format), _args...));
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