cmake_minimum_required(VERSION 3.2)

project(OsuRequest)
set(CMAKE_CXX_STANDARD 20)

add_definitions(-DUNICODE -D_UNICODE)

find_package(CURL REQUIRED)
if (NOT CURL_FOUND)
    message(FATAL_ERROR "Couldn't find CURL")
endif()

set(SOURCE_FILES "Source/Beatmap.cpp" "Source/Core/Client.cpp")
set(HEADER_FILES 
    "External/fmt/include" 
    "External/json/include" 
    "External/thread_pool/include" 
    "External/curlcpp/include")

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIR} ${HEADER_FILES})