#include "../../Include/KaedeRequest/Api/Beatconnect.hpp"

#include "fmt/format.h"

#include "../../Include/Logging.hpp"
#include "../../Include/KaedeRequest/Net/Client.hpp"

namespace kaede::api
{
    namespace endpoint
    {
        constexpr const auto DOWNLOAD_BEATMAP = "https://beatconnect.io/b/{}";
    }

    auto download_beatmap(const std::filesystem::path path, const Beatmap& beatmap) -> void
    {
        if (!std::filesystem::exists(path))
        {
            KAEDE_ERRO(fmt::format("Couldn't find path {}", path.string())); return;
        }

        auto beatmapName = fmt::format("{} {} - {}", beatmap.beatmapsetID, beatmap.songInfo.artist, beatmap.songInfo.title);

        std::replace_if(beatmapName.begin(), beatmapName.end(), [](auto value)
        {
            return value == '\\' || value == '\"' || value == '/' ||
                   value == '*'  || value == '?'  || value == '|' ||
                   value == '<'  || value == '>'  || value == ':';
        }, '_');

        std::ofstream beatmapStream { fmt::format("{}/{}.osz", path.string(), beatmapName), std::ios::binary };

        core::get(fmt::format(endpoint::DOWNLOAD_BEATMAP, beatmap.beatmapsetID), &beatmapStream);
    }
}