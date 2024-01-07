#pragma once

#pragma push_macro("U")
#undef U
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma pop_macro("U")

#include <cstdarg>

namespace ncqt {


inline static auto& logger() {
    static auto l = spdlog::basic_logger_mt("basic_logger", "sptr.log", true);
    static bool first = true;
    if (first) {
        l->set_level(spdlog::level::debug);
        l->set_pattern("[%^%l%$] %v");
        first = false;
    }
    return *l;
}

inline void log_info(const std::string& str){
    logger().info(str);
    logger().flush();
}

inline void log_debug(const std::string& str) {
    logger().debug(str);
    logger().flush();
}

inline void log_error(const std::string& str) {
    logger().error(str);
    logger().flush();
}

}
