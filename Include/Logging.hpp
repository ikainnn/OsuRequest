#ifndef LOGGING
#define LOGGING

#include <iostream>
#include <format>

#ifdef KAEDE_DEBUG
    #define BASIC_LOG(tag, msg) std::cout << std::format("{} {} \n", tag, msg)

    #define KAEDE_WARN(msg) BASIC_LOG("[WARN]", msg)
    #define KAEDE_ERRO(msg) BASIC_LOG("[ERRO]", msg)
    #define KAEDE_INFO(msg) BASIC_LOG("[INFO]", msg)
#else
    #define KAEDE_WARN(msg)
    #define KAEDE_ERRO(msg)
    #define KAEDE_INFO(msg)
#endif

#endif