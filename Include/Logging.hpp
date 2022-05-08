#ifndef LOGGING
#define LOGGING

#include <iostream>

#define BASIC_LOG(tag, msg) std::cout << tag << msg << '\n';

#define KAEDE_WARN(msg) BASIC_LOG("[WARN]", msg)
#define KAEDE_ERRO(msg) BASIC_LOG("[ERRO]", msg)
#define KAEDE_INFO(msg) BASIC_LOG("[INFO]", msg)

#endif