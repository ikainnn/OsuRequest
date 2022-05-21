#include "../../Include/OsuRequest/Core/Client.hpp"

namespace osu_request::api::core
{
    auto get(const Url& _url, std::string* _result) -> void
    {
        get(_url, _result, +[] (char* raw, std::size_t size, std::size_t count, std::string* data)
        {
            data->append(raw, size * count); return size * count;
        });
    }

    auto get(const Url& _url, std::ofstream* _result) -> void
    {
        get(_url, _result, +[] (char* raw, std::size_t size, std::size_t count, std::ofstream* data)
        {
            data->write(raw, size * count); return size * count;
        });
    }
}