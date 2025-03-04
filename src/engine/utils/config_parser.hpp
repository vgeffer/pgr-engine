#pragma once
#include <string>

namespace utils {
    class config_parser {
        public:
            config_parser(std::string file, bool no_throw_on_empty = false);
            ~config_parser();

            template<typename  T>
                T& get_value(std::string key);

            template<typename T>
                void set_value(std::string key, T& val);

            void save();
    };
};