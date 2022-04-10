#ifndef BEATCONNECT
#define BEATCONNECT

#include <filesystem>

#include "../Beatmap.hpp"

namespace kaede::api
{
    auto download_beatmap(const std::filesystem::path path, const Beatmap& beatmap) -> void;
}

#endif
