#ifndef BEATMAP
#define BEATMAP

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace kaede::api
{
    struct Beatmap // What the fuck is this. Holy shit.
    {
        std::string beatmapsetID;
        std::string beatmapID;
        std::string version;
        std::string mode;
        std::string creator;
        std::string difficultyRating;

        struct Song
        {
            std::string artist;
            std::string title;
            std::string totalLength;
            std::string bpm;
        };

        Song songInfo;
    };

    using PlayerKey = std::string_view;
    using Hash      = std::string_view;

    namespace fs = std::filesystem;

    auto get_beatmap_info(const PlayerKey& playerKey, const Hash& beatmapHash) -> Beatmap;
    auto get_beatmap_info(const PlayerKey& playerKey, const std::vector<Hash>& beatmapHashes) -> std::vector<Beatmap>;
    auto get_beatmap_info(const PlayerKey& playerKey, const std::vector<Hash>& beatmapHashes, const std::size_t threadCount) -> std::vector<Beatmap>;
    auto download_beatmap(const fs::path& path, const Beatmap& beatmap) -> void;
    auto download_beatmap(const fs::path& path, const std::vector<Beatmap>& beatmaps) -> void;
    auto download_beatmap(const fs::path& path, const std::vector<Beatmap>& beatmaps, const std::size_t threadCount) -> void;
}

#endif