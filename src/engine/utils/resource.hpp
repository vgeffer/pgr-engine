#pragma once 
#include <string>
#include "../../lib/json/json.hpp"


#define MAKE_JSON_PARSABLE(T, parse)                \
    namespace nlohmann {                            \
        template <>                                 \
        struct adl_serializer<T> {                  \
                                                    \
            static T from_json(const json& j) {     \
                return parse(j);                    \
            }                                       \
        };                                          \
    }              

namespace utils {

    class resource {
        public:
            using component = nlohmann::basic_json<>;

            resource(std::string path);
            resource(const component& json);

            void save();

            template <typename T>
                T deserialize(std::string path);
            template <typename T>
                T deserialize(std::string path, const T& default_value);
            template <typename T> 
                void serialize(std::string path, T& value);

        private:
            component _root;
            std::string _source_path;
            bool _has_file_open;
    };
};