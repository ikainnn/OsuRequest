#include "../../Include/KaedeRequest/Api/Osu.hpp"

#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include "../../Include/KaedeRequest/Net/Client.hpp"

namespace kaede::api
{
    namespace endpoint
    {
        constexpr const auto GET_BEATMAP_INFO = "https://osu.ppy.sh/api/get_beatmaps?k={}&h={}";
    }

    auto get_beatmap_info(const std::string_view playerKey, const std::string_view beatmapHash) -> Beatmap
    {
        std::string response { };

        core::get<std::string>(fmt::format(endpoint::GET_BEATMAP_INFO, playerKey, beatmapHash), &response);
        
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
}