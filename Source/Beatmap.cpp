#include "../Include/KaedeRequest/Beatmap.hpp"

#define EXTERNAL_INCLUDES
    #include <future>
    #include <ranges>
    #include <thread>

    #include "fmt/format.h"
    #include "nlohmann/json.hpp"
    #include "thread_pool/thread_pool.hpp"

    #include "../Include/Logging.hpp"
    #include "../Include/KaedeRequest/Core/Client.hpp"
#define EXTERNAL_INCLUDES

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

    auto get_beatmap_info(const PlayerKey& playerKey, const Hashes& beatmapHashes, const std::size_t numThread) -> std::vector<Beatmap>
    {
        thread_pool::ThreadPool pool { numThread };

        std::vector<std::future<Beatmap>> beatmapFutures{};
        std::vector<Beatmap> beatmaps{};

        using FunctionType = Beatmap(const PlayerKey&, const Hash&);
        std::ranges::transform(beatmapHashes, std::back_inserter(beatmapFutures), [&](auto&& beatmapHash)
        {
            return pool.Submit<FunctionType>(get_beatmap_info, playerKey, beatmapHash);
        });

        std::ranges::transform(beatmapFutures, std::back_inserter(beatmaps), &std::future<Beatmap>::get);

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

    auto download_beatmap(const fs::path& path, const std::vector<Beatmap>& beatmaps, const std::size_t numThread) -> void
    {
        thread_pool::ThreadPool pool { numThread };

        using FunctionType = void(const fs::path&, const Beatmap&);
        std::ranges::for_each(beatmaps, [&] (auto&& beatmap)
        {
            pool.Submit<FunctionType>(download_beatmap, path, beatmap); 
        });
    }
}