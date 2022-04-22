#include "../Include/KaedeRequest/Beatmap.hpp"

#include <future>
#include <ranges>
#include <thread>

#include "fmt/format.h"
#include "nlohmann/json.hpp"

#include "../Include/Logging.hpp"
#include "../Include/KaedeRequest/Core/Client.hpp"

namespace kaede::api
{
    namespace endpoint
    {
        constexpr const auto DOWNLOAD_BEATMAP = "https://beatconnect.io/b/{}";
        constexpr const auto BEATMAP_INFO = "https://osu.ppy.sh/api/get_beatmaps?k={}&h={}";
    }

    auto get_beatmap_info(const PlayerKey& playerKey, const Hash& beatmapHash) -> Beatmap
    {
        if (playerKey.empty()) return { };

        std::string response { };
        core::get(fmt::format(endpoint::BEATMAP_INFO, playerKey, beatmapHash), &response);

        const auto beatmapJson = nlohmann::json::parse(response)[0];

        if (beatmapJson.is_null())
        {
            KAEDE_WARN(fmt::format("couldn't retrieve information for {}.", beatmapHash)); return { };
        }

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

    auto get_beatmap_info(const PlayerKey& playerKey, const Hashes& beatmapHashes) -> std::vector<Beatmap>
    {
        std::vector<Beatmap> beatmaps { }; beatmaps.reserve(beatmapHashes.size());

        std::ranges::transform(beatmapHashes, std::back_inserter(beatmaps), [playerKey] (auto hash) -> Beatmap
        {
            return get_beatmap_info(playerKey, hash);
        });

        if (playerKey.empty()) KAEDE_WARN("playerKey was empty. request might not have been completed properly.");

        return beatmaps;
    }

    auto get_beatmap_info(const PlayerKey& playerKey, const Hashes& beatmapHashes, const std::size_t threadCount) -> std::vector<Beatmap>
    {
        std::vector<Beatmap> beatmaps { }; beatmaps.reserve(beatmapHashes.size());

        const auto processAfter = (beatmapHashes.size() % threadCount);
        const auto processNow   = beatmapHashes.size() - processAfter;

        using FunctionSignature = Beatmap(*)(const std::string_view&, const std::string_view&);

        const auto processHashes = [&playerKey, &beatmaps](const Hashes& beatmapHashes, const std::size_t processCount, const std::size_t threadCount)
        {
            std::vector<std::future<Beatmap>> workers { threadCount };

            for (std::size_t pos = 0; pos < processCount; pos += threadCount)
            {
                for (auto thread = 0; thread < threadCount; ++thread)
                {
                    workers[thread] = std::async<FunctionSignature>(std::launch::async, get_beatmap_info, playerKey, beatmapHashes[pos + thread]);
                }

                std::ranges::transform(workers, std::back_inserter(beatmaps), &std::future<Beatmap>::get);
            }
        };

        processHashes(beatmapHashes, processNow, threadCount);
        processHashes({ beatmapHashes.begin() + processNow, beatmapHashes.end() }, processAfter, processAfter);

        if (playerKey.empty()) KAEDE_WARN("playerKey was empty. request might not have been completed properly.");

        return beatmaps;
    }

    auto download_beatmap(const fs::path& path, const Beatmap& beatmap) -> void
    {
        if (!fs::exists(path))
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

    auto download_beatmap(const fs::path& path, const std::vector<Beatmap>& beatmaps) -> void
    {
        for (const auto& beatmap : beatmaps) download_beatmap(path, beatmap);
    }

    auto download_beatmap(const fs::path& path, const std::vector<Beatmap>& beatmaps, const std::size_t threadCount) -> void
    {
        const auto processAfter = (beatmaps.size() % threadCount);
        const auto processNow   = beatmaps.size() - processAfter;

        using FunctionSignature = void(*)(const fs::path&, const Beatmap&);

        const auto processBeatmaps = [&path](const std::vector<Beatmap>& beatmaps, const std::size_t processCount, const std::size_t threadCount)
        {
            std::vector<std::future<void>> workers { threadCount };

            for (std::size_t pos = 0; pos < processCount; pos += threadCount)
            {
                for (auto thread = 0; thread < threadCount; ++thread)
                {
                    workers[thread] = std::async<FunctionSignature>(std::launch::async, download_beatmap, path, beatmaps[pos + thread]);
                }

                std::ranges::for_each(workers, &std::future<void>::get);
            }
        };

        processBeatmaps(beatmaps, processNow, threadCount);
        processBeatmaps({ beatmaps.begin() + processNow, beatmaps.end() }, processAfter, processAfter);
    }
}