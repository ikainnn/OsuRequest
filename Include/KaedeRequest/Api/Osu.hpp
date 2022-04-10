#ifndef OSU
#define OSU

#include "../Beatmap.hpp"
#include "../Net/Client.hpp"

namespace kaede::api
{
    auto get_beatmap_info(const std::string_view playerKey, const std::string_view beatmapHash) -> Beatmap;
}

#endif
