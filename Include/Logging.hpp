#ifndef LOGGING
#define LOGGING

#include "spdlog/spdlog.h"

#define KAEDE_WARN(msg) spdlog::warn(msg);
#define KAEDE_ERRO(msg) spdlog::error(msg);
#define KAEDE_INFO(msg) spdlog::info(msg);

#endif