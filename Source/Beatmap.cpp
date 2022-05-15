#include "../Include/KaedeRequest/Beatmap.hpp"

#include <future>
#include <thread>

#include "nlohmann/json.hpp"
#include "thread_pool/thread_pool.hpp"

#include "../Include/Logging.hpp"
#include "../Include/KaedeRequest/Core/Client.hpp"

namespace kaede::api
{
    namespace endpoint
    {
        constexpr const auto DOWNLOAD_BEATMAP = "https://beatconnect.io/b/{}";
        constexpr const auto BEATMAP_INFO = "https://osu.ppy.sh/api/get_beatmaps?k={}&h={}";
    }

    auto get_beatmap_info(std::string_view playerKey, std::string_view beatmapHash) -> Beatmap
    {
        if (playerKey.empty()) return { };

        std::string response { };
        core::get(fmt::format(endpoint::BEATMAP_INFO, playerKey, beatmapHash), &response);

        const auto beatmapJson = nlohmann::json::parse(response)[0];

        if (beatmapJson.is_null())
        {
            KAEDE_WARN("couldn't retrieve information for {}.", beatmapHash); return { };
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

    auto get_beatmap_info(std::string_view playerKey, const Hashes& beatmapHashes) -> std::vector<Beatmap>
    {
        std::vector<Beatmap> beatmaps { }; beatmaps.reserve(beatmapHashes.size());

        std::transform(beatmapHashes.begin(), beatmapHashes.end(), std::back_inserter(beatmaps), [playerKey] (auto hash) -> Beatmap
        {
            return get_beatmap_info(playerKey, hash);
        });

        if (playerKey.empty()) KAEDE_WARN("Key was empty. request might not have been completed properly.");

        return beatmaps;
    }

    auto get_beatmap_info(std::string_view playerKey, const Hashes& beatmapHashes, std::size_t numThread) -> std::vector<Beatmap>
    {
        thread_pool::ThreadPool pool { numThread };

        std::vector<std::future<Beatmap>> beatmapFutures{};
        std::vector<Beatmap> beatmaps{};

        using FunctionType = Beatmap(*)(std::string_view, std::string_view);
        std::transform(beatmapHashes.begin(), beatmapHashes.end(), std::back_inserter(beatmapFutures), [&](auto&& beatmapHash)
        {
            return pool.Submit<FunctionType>(get_beatmap_info, playerKey, beatmapHash);
        });

        std::transform(beatmapFutures.begin(), beatmapFutures.end(), std::back_inserter(beatmaps), [](auto&& future){ return future.get(); });

        return beatmaps;
    }

    auto download_beatmap(const filesystem::path& path, const Beatmap& beatmap) -> void
    {
        if (!filesystem::exists(path))
        {
            KAEDE_ERRO("Couldn't find path {}", path.string()); return;
        }

        auto beatmapName = fmt::format("{} {} - {}", beatmap.beatmapsetID, beatmap.songInfo.artist, beatmap.songInfo.title);

        std::replace_if(beatmapName.begin(), beatmapName.end(), [](auto&& value)
        {
            return value == '\\' || value == '\"' || value == '/' ||
                   value == '*'  || value == '?'  || value == '|' ||
                   value == '<'  || value == '>'  || value == ':';
        }, '_');

        std::ofstream beatmapStream { fmt::format("{}/{}.osz", path.string(), beatmapName), std::ios::binary };
        core::get(fmt::format(endpoint::DOWNLOAD_BEATMAP, beatmap.beatmapsetID), &beatmapStream);
    }

    auto download_beatmap(const filesystem::path& path, const std::vector<Beatmap>& beatmaps) -> void
    {
        for (const auto& beatmap : beatmaps) download_beatmap(path, beatmap);
    }

    auto download_beatmap(const filesystem::path& path, const std::vector<Beatmap>& beatmaps, std::size_t numThread) -> void
    {
        thread_pool::ThreadPool pool { numThread };

        using FunctionType = void(*)(const filesystem::path&, const Beatmap&);
        std::for_each(beatmaps.begin(), beatmaps.end(), [&] (auto&& beatmap)
        {
            pool.Submit<FunctionType>(download_beatmap, path, beatmap); 
        });
    }
}
