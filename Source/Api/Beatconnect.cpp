#include "../../Include/KaedeRequest/Api/Beatconnect.hpp"

#include "fmt/format.h"

#include "../../Include/KaedeRequest/Net/Client.hpp"

namespace kaede::api
{
    namespace endpoint
    {
        constexpr const auto DOWNLOAD_BEATMAP = "https://beatconnect.io/b/{}";
    }

    auto download_beatmap(const std::filesystem::path path, const Beatmap& beatmap) -> void
    {
        const auto filename = fmt::format("{}/{} {} - {}", path.string(), beatmap.beatmapsetID, beatmap.songInfo.artist, beatmap.songInfo.title);

        std::ofstream beatmapStream { filename, std::ios::binary };

        core::get(fmt::format(endpoint::DOWNLOAD_BEATMAP, beatmap.beatmapsetID), &beatmapStream);
    }
}