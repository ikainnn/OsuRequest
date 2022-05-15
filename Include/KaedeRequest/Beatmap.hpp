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

    using Hashes = std::vector<std::string>;

    namespace filesystem = std::filesystem;

    auto get_beatmap_info(std::string_view playerKey, std::string_view beatmapHash) -> Beatmap;
    auto get_beatmap_info(std::string_view playerKey, const Hashes& beatmapHashes) -> std::vector<Beatmap>;
    auto get_beatmap_info(std::string_view playerKey, const Hashes& beatmapHashes, std::size_t threadCount) -> std::vector<Beatmap>;
    auto download_beatmap(const filesystem::path& path, const Beatmap& beatmap) -> void;
    auto download_beatmap(const filesystem::path& path, const std::vector<Beatmap>& beatmaps) -> void;
    auto download_beatmap(const filesystem::path& path, const std::vector<Beatmap>& beatmaps, std::size_t threadCount) -> void;
}

#endif