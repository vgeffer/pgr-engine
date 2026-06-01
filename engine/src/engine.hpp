#pragma once 
#include <string>

namespace pgreng {
    class engine {

        public:
            [[noreturn]] static void start(const std::string& app_config_path); /* TODO: Enable other ways of loading the app */
    };
}