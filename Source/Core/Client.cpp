#include "../../Include/KaedeRequest/Core/Client.hpp"

namespace kaede::api::core
{
    auto get(const Url& url, std::string* result) -> void
    {
        get(url, result, +[] (char* raw, std::size_t size, std::size_t count, std::string* data)
        {
            data->append(raw, size * count); return size * count;
        });
    }

    auto get(const Url& url, std::ofstream* result) -> void
    {
        get(url, result, +[] (char* raw, std::size_t size, std::size_t count, std::ofstream* data)
        {
            data->write(raw, size * count); return size * count;
        });
    }
}