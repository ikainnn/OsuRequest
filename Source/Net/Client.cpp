#include "../../Include/KaedeRequest/Net/Client.hpp"

namespace kaede::api::core
{
    template <>
    auto get(const std::string_view url, std::string* result) -> void
    {
        get(url, result, +[] (char* raw, std::size_t size, std::size_t count, std::string* data)
        {
            data->append(raw, size * count); return size * count;
        });
    }

    template <>
    auto get(const std::string_view url, std::ofstream* result) -> void
    {
        get(url, result, +[] (char* raw, std::size_t size, std::size_t count, std::ofstream* data)
        {
            data->write(raw, size * count); return size * count;
        });
    }
}