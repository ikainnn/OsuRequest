#ifndef BEATMAP
#define BEATMAP

#include <string>
#include <filesystem>

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

    auto get_beatmap_info(const std::string_view playerKey, const std::string_view beatmapHash) -> Beatmap;
    auto download_beatmap(const std::filesystem::path path, const Beatmap& beatmap) -> void;
}

#endif