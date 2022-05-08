#ifndef CLIENT
#define CLIENT

#include <string>
#include <string_view>
#include <vector>
#include <fstream>

#include "curlcpp/curl_easy.h"

#include "../../Logging.hpp"

namespace kaede::api::core
{
    template <class UnknownType>
    using WriteFunction = std::size_t (*)(char*, std::size_t, std::size_t, UnknownType*);

    using Url = std::string_view;

    template <class UnknownType>
    auto get(const Url& url, UnknownType* result, const WriteFunction<UnknownType> writeFunction) -> void
    {
        curl::curl_easy curl { };

        curl.add<CURLOPT_URL>(url.data());
        curl.add<CURLOPT_FOLLOWLOCATION>(true);
        curl_easy_setopt(curl.get_curl(), CURLOPT_WRITEDATA, result);
        curl_easy_setopt(curl.get_curl(), CURLOPT_WRITEFUNCTION, writeFunction);

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
    auto get(const Url& url, UnknownType* result) -> void;

    template <>
    auto get(const Url& url, std::string* result) -> void;
    
    template <>
    auto get(const Url& url, std::ofstream* result) -> void;
}

#endif
