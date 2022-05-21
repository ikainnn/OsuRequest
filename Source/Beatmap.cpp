#include "../Include/OsuRequest/Beatmap.hpp"

#include <future>
#include <thread>

#include "nlohmann/json.hpp"
#include "thread_pool/thread_pool.hpp"

#include "../Include/Logging.hpp"
#include "../Include/OsuRequest/Core/Client.hpp"

namespace osu_request::api
{
    constexpr const auto DOWNLOAD_BEATMAP = "https://beatconnect.io/b/{}";
    constexpr const auto BEATMAP_INFO = "https://osu.ppy.sh/api/get_beatmaps?k={}&h={}";

    auto get_beatmap_info(std::string_view _playerKey, std::string_view _beatmapHash) -> Beatmap
    {
        if (_playerKey.empty()) return { };

        std::string response { };
        core::get(fmt::format(BEATMAP_INFO, _playerKey, _beatmapHash), &response);

        const auto beatmapJson = nlohmann::json::parse(response)[0];

        if (beatmapJson.is_null())
        {
            osu_request::logging::warn("couldn't retrieve information for {}.", _beatmapHash); return { };
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

    auto get_beatmap_info(std::string_view _playerKey, const Hashes& _beatmapHashes) -> std::vector<Beatmap>
    {
        std::vector<Beatmap> beatmaps { }; beatmaps.reserve(_beatmapHashes.size());

        std::ranges::transform(_beatmapHashes, std::back_inserter(beatmaps), [_playerKey] (auto hash) -> Beatmap
        {
            return get_beatmap_info(_playerKey, hash);
        });

        if (_playerKey.empty()) osu_request::logging::warn("Key was empty. request might not have been completed properly.");

        return beatmaps;
    }

    auto get_beatmap_info(std::string_view _playerKey, const Hashes& _beatmapHashes, std::size_t _numThread) -> std::vector<Beatmap>
    {
        thread_pool::ThreadPool pool { _numThread };

        std::vector<std::future<Beatmap>> beatmapFutures{};
        std::vector<Beatmap> beatmaps{};

        using FunctionType = Beatmap(*)(std::string_view, std::string_view);
        std::ranges::transform(_beatmapHashes, std::back_inserter(beatmapFutures), [&](auto&& beatmapHash)
        {
            return pool.Submit<FunctionType>(get_beatmap_info, _playerKey, beatmapHash);
        });

        std::transform(beatmapFutures.begin(), beatmapFutures.end(), std::back_inserter(beatmaps), [](auto&& future){ return future.get(); });

        return beatmaps;
    }

    auto download_beatmap(const std::filesystem::path& _path, const Beatmap& _beatmap) -> void
    {
        if (!std::filesystem::exists(_path))
        {
            osu_request::logging::error("Couldn't find path {}", _path.string()); return;
        }

        auto beatmapName = fmt::format("{} {} - {}", _beatmap.beatmapsetID, _beatmap.songInfo.artist, _beatmap.songInfo.title);
        std::ranges::replace_if(beatmapName, [](auto&& value)
        {
            return value == '\\' || value == '\"' || value == '/' ||
                   value == '*'  || value == '?'  || value == '|' ||
                   value == '<'  || value == '>'  || value == ':';
        }, '_');

        std::ofstream beatmapStream { fmt::format("{}/{}.osz", _path.string(), beatmapName), std::ios::binary };
        core::get(fmt::format(DOWNLOAD_BEATMAP, _beatmap.beatmapsetID), &beatmapStream);
    }

    auto download_beatmap(const std::filesystem::path& _path, const std::vector<Beatmap>& _beatmaps) -> void
    {
        for (const auto& beatmap : _beatmaps) download_beatmap(_path, beatmap);
    }

    auto download_beatmap(const std::filesystem::path& _path, const std::vector<Beatmap>& _beatmaps, std::size_t _numThread) -> void
    {
        thread_pool::ThreadPool pool { _numThread };

        using FunctionType = void(*)(const std::filesystem::path&, const Beatmap&);
        std::ranges::for_each(_beatmaps, [&] (auto&& beatmap)
        {
            pool.Submit<FunctionType>(download_beatmap, _path, beatmap); 
        });
    }
}