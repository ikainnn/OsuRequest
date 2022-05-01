cmake_minimum_required(VERSION 3.2)

project(KaedeRequest)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DUNICODE -D_UNICODE -DCURL_STATICLIB)

set(BASE "Source/")

set(CURL_LIB_DIR    "${PROJECT_SOURCE_DIR}/External/curl")
set(CURLCPP_LIB_DIR "${PROJECT_SOURCE_DIR}/External/curlcpp")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CURL_LIB_DIR    "${CURL_LIB_DIR}/libcurld.lib")
    set(CURLCPP_LIB_DIR "${CURLCPP_LIB_DIR}/curlcppd.lib")
else()
    set(CURL_LIB_DIR    "${CURL_LIB_DIR}/libcurl.lib")
    set(CURLCPP_LIB_DIR "${CURLCPP_LIB_DIR}/curlcpp.lib")
endif()

macro(add_subdirectory_safe var)
    if (NOT TARGET ${var})
        add_subdirectory("External/${var}")
    else()
        message(STATUS "${var} is already added. nothing changed.")
    endif()
endmacro()

add_subdirectory_safe("spdlog")
add_subdirectory_safe("fmt")

set(SOURCE_FILES "${BASE}/Core/Client.cpp" "${BASE}/Beatmap.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    "External/curl/include"
    "External/curlcpp/include"
    "External/spdlog/include"
    "External/json/include"
    "External/fmt/include"
    "External/thread_pool/include")

target_link_libraries(${PROJECT_NAME} ${CURL_LIB_DIR} ${CURLCPP_LIB_DIR} ws2_32 crypt32 wldap32 normaliz spdlog fmt)