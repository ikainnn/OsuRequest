#ifndef CLIENT
#define CLIENT

#include <string>
#include <string_view>
#include <vector>
#include <fstream>

#include "curlcpp/curl_easy.h"
#include "fmt/format.h"

#include "../../Logging.hpp"

namespace kaede::api::core
{
    template <class UnknownType>
    using WriteFunction = std::size_t (*)(char*, std::size_t, std::size_t, UnknownType*);

    template <class UnknownType>
    auto get(const std::string_view url, UnknownType* result, const WriteFunction<UnknownType> writeFunction) -> void
    {
        curl::curl_easy curl { };

        curl.add<CURLOPT_URL>(url.data());
        curl.add<CURLOPT_FOLLOWLOCATION>(true);
        curl_easy_setopt(curl.get_curl(), CURLOPT_WRITEDATA, result);
        curl_easy_setopt(curl.get_curl(), CURLOPT_WRITEFUNCTION, writeFunction);

        KAEDE_INFO(fmt::format("Performing HTTP GET request at {}", url.data()));

        try
        {
            curl.perform();
        }
        catch (const std::exception& exception)
        {
            KAEDE_ERRO(exception.what());
        }
    }

    template <class UnknownType>
    auto get(const std::string_view url, UnknownType* result) -> void;

    template <>
    auto get(const std::string_view url, std::string* result) -> void;
    
    template <>
    auto get(const std::string_view url, std::ofstream* result) -> void;
}

#endif
