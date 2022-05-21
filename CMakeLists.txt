cmake_minimum_required(VERSION 3.2)

project(OsuRequest)
set(CMAKE_CXX_STANDARD 20)

add_definitions(-DUNICODE -D_UNICODE -DCURL_STATICLIB)

find_package(CURL REQUIRED)
if (NOT CURL_FOUND)
    message(FATAL_ERROR "Couldn't find CURL")
endif()

macro(add_subdirectory_safe var)
    if (NOT TARGET ${var})
        add_subdirectory("External/${var}")
    else()
        message(STATUS "${var} is already added. nothing changed.")
    endif()
endmacro()

add_subdirectory_safe("fmt")
add_subdirectory_safe("curlcpp")

set(SOURCE_FILES "Source/Beatmap.cpp" "Source/Core/Client.cpp")
set(HEADER_FILES "External/fmt/include" "External/json/include" 
                 "External/thread_pool/include" "External/curlcpp/include")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIR} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt curl curlcpp pthread ${CURL_LIBRARIES})