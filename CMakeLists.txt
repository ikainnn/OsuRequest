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
        add_subdirectory("${PROJECT_SOURCE_DIR}/External/${var}")
    else()
        message(STATUS "${var} is already added. nothing changed.")
    endif()
endmacro()

add_subdirectory_safe("fmt")
add_subdirectory_safe("curlcpp")

set(SOURCE_FILES "${PROJECT_SOURCE_DIR}/Source/Beatmap.cpp" "${PROJECT_SOURCE_DIR}/Source/Core/Client.cpp")
set(HEADER_FILES 
    "${PROJECT_SOURCE_DIR}/External/fmt/include" 
    "${PROJECT_SOURCE_DIR}/External/json/include" 
    "${PROJECT_SOURCE_DIR}/External/thread_pool/include" 
    "${PROJECT_SOURCE_DIR}/External/curlcpp/include")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt curl ${CURL_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIR} ${HEADER_FILES})