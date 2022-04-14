#include "../Include/KaedeRequest/Beatmap.hpp"

#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include "../Include/Logging.hpp"
#include "../Include/KaedeRequest/Core/Client.hpp"

namespace kaede::api
{
    namespace endpoint
    {
        constexpr const auto DOWNLOAD_BEATMAP = "https://beatconnect.io/b/{}";
        constexpr const auto GET_BEATMAP_INFO = "https://osu.ppy.sh/api/get_beatmaps?k={}&h={}";
    }

    auto get_beatmap_info(const std::string_view playerKey, const std::string_view beatmapHash) -> Beatmap
    {
        std::string response { };

        core::get(fmt::format(endpoint::GET_BEATMAP_INFO, playerKey, beatmapHash), &response);
        
        const auto beatmapJson = nlohmann::json::parse(response)[0];

        return Beatmap
        {
            .beatmapsetID = beatmapJson["beatmapset_id"],
            .beatmapID = beatmapJson["beatmap_id"],
            .version = beatmapJson["version"],
            .mode = beatmapJson["mode"],
            .creator = beatmapJson["creator"],
            .difficultyRating = beatmapJson["difficultyrating"],

            .songInfo = Beatmap::Song
            {
                .artist = beatmapJson["artist"],
                .title = beatmapJson["title"],
                .totalLength = beatmapJson["total_length"],
                .bpm = beatmapJson["bpm"]
            }
        };
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