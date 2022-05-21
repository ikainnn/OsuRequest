#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <fstream>

#include "curl_easy.h"

#include "../../Logging.hpp"

namespace osu_request::api::core
{
    template <class T>
    using WriteFunction = std::size_t (*)(char*, std::size_t, std::size_t, T*);

    using Url = std::string_view;

    template <class T>
    auto get(const Url& _url, T* _result, const WriteFunction<T> _writeFunction) -> void
    {
        curl::curl_easy curl { };

        curl.add<CURLOPT_URL>(_url.data());
        curl.add<CURLOPT_FOLLOWLOCATION>(true);
        curl_easy_setopt(curl.get_curl(), CURLOPT_WRITEDATA, _result);
        curl_easy_setopt(curl.get_curl(), CURLOPT_WRITEFUNCTION, _writeFunction);

        try
        {
            curl.perform();
        }
        catch (const std::exception& exception)
        {
            osu_request::logging::error(exception.what());
        }
    }

    auto get(const Url& url, std::string* result) -> void;
    auto get(const Url& url, std::ofstream* result) -> void;
}