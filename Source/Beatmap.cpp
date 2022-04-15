#include "../Include/KaedeRequest/Beatmap.hpp"

#include <future>
#include <ranges>
#include <thread>
#include <vector>

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

    auto get_beatmap_info(const std::string_view& playerKey, const std::string_view& beatmapHash) -> Beatmap
    {
        if (playerKey.empty()) return { };

        std::string response { };

        core::get(fmt::format(endpoint::GET_BEATMAP_INFO, playerKey, beatmapHash), &response);
        
        const auto beatmapJson = nlohmann::json::parse(response)[0];

        if (beatmapJson.is_null()) { KAEDE_WARN(fmt::format("couldn't retrieve information for {}.", beatmapHash)); return { }; }

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

    auto get_beatmap_info(const std::string_view& playerKey, const std::vector<std::string_view>& beatmapHashes) -> std::vector<Beatmap>
    {
        std::vector<Beatmap> beatmaps { }; beatmaps.reserve(beatmapHashes.size());

        std::ranges::transform(beatmapHashes, std::back_inserter(beatmaps), [playerKey] (auto hash) -> Beatmap
        {
            return get_beatmap_info(playerKey, hash);
        });

        if (playerKey.empty()) KAEDE_WARN("playerKey was empty. request might not have been completed properly.");

        return beatmaps;
    }

    auto get_beatmap_info(const std::string_view& playerKey,
                          const std::vector<std::string>& beatmapHashes,
                          const std::size_t threadCount) -> std::vector<Beatmap>
    {
        std::vector<Beatmap> beatmaps { }; beatmaps.reserve(beatmapHashes.size());

        const auto processAfter = (beatmapHashes.size() % threadCount);
        const auto processNow   = beatmapHashes.size() - processAfter;

        using ptrGBI = Beatmap(*)(const std::string_view&, const std::string_view&);

        const auto processHashes = [&playerKey, &beatmaps](const std::vector<std::string>& beatmapHashes, const std::size_t processCount, const std::size_t threadCount)
        {
            std::vector<std::future<Beatmap>> workers { threadCount };

            for (std::size_t pos = 0; pos < processCount; pos += threadCount)
            {
                for (auto thread = 0; thread < threadCount; ++thread)
                {
                    workers[thread] = std::async<ptrGBI>(std::launch::async, get_beatmap_info, playerKey, beatmapHashes[pos + thread]);
                }

                std::ranges::transform(workers, std::back_inserter(beatmaps), [](auto& worker){ return worker.get(); });

                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
        };

        processHashes(beatmapHashes, processNow, threadCount);
        processHashes({ beatmapHashes.begin() + processNow, beatmapHashes.end() }, processAfter, processAfter);

        if (playerKey.empty()) KAEDE_WARN("playerKey was empty. request might not have been completed properly.");

        return beatmaps;
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