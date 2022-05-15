#ifndef LOGGING
#define LOGGING

#include <iostream>
#include <format>

#pragma clang diagnostic ignored "-Winvalid-token-paste"
#pragma clang diagnostic push

#ifdef KAEDE_DEBUG
    #define KAEDE_PRINT(fmt, ...) std::cout << std::format(fmt, __VA_ARGS__) << '\n'

    #define KAEDE_WARN(msg, ...) KAEDE_PRINT("[WARN] " ##msg, __VA_ARGS__)
    #define KAEDE_ERRO(msg, ...) KAEDE_PRINT("[ERRO] " ##msg, __VA_ARGS__)
    #define KAEDE_INFO(msg, ...) KAEDE_PRINT("[INFO] " ##msg, __VA_ARGS__)
#else
    #define KAEDE_WARN(msg, ...)
    #define KAEDE_ERRO(msg, ...)
    #define KAEDE_INFO(msg, ...)
#endif

#pragma clang diagnostic pop

#endif