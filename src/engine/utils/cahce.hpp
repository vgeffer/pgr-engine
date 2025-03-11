#pragma once
#include <string>

#define PERSISTENT_CAHCE_PATH "./.cache"

namespace utils {
    class cache {

        public:
            explicit cache(const char* persistence_path = PERSISTENT_CAHCE_PATH);
            ~cache();
            static void add_item(std::string key, bool is_persistant, void* data, size_t data_size);
            static void* get_item(std::string key, size_t* data_size);
            static bool has_item(std::string key);
            static void del_item(std::string key);

        private:
            static cache* instance;
    };
}   