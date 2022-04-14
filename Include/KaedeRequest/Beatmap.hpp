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

    auto get_beatmap_info(const std::string_view& playerKey, const std::string_view& beatmapHash) -> Beatmap;
    
    auto get_beatmap_info(const std::string_view& playerKey, const std::vector<std::string_view>& beatmapHashes) -> std::vector<Beatmap>;

    auto get_beatmap_info(const std::string_view& playerKey,
                          const std::vector<std::string_view>& beatmapHashes, 
                          const std::size_t threadCount) -> std::vector<Beatmap>;

    auto download_beatmap(const std::filesystem::path& path, const Beatmap& beatmap) -> void;
}

#endif