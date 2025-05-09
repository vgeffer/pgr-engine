#pragma once 
#include <glm/fwd.hpp>
#include <string>
#include <glm/gtc/quaternion.hpp> 
#include <glm/glm.hpp>
#include "../../lib/json/json.hpp"
#include "algorithms.hpp"


#define MAKE_JSON_PARSABLE(T, parse)                \
    namespace nlohmann {                            \
        template <>                                 \
        struct adl_serializer<T> {                  \
                                                    \
            static T from_json(const json& j) {     \
                return parse(utils::resource(j));   \
            }                                       \
        };                                          \
    }              

namespace utils {

    class resource {
        public:
            resource(std::string path);
            resource(const nlohmann::json& json);

            void save();

            nlohmann::json res() const { return _root; }
            
            template <typename T>
            T deserialize(std::string path) const {
                if (!_has_file_open)
                    throw std::runtime_error("Unable to parse JSON, no file is open!");
            
                std::vector<std::string> path_components = split(path, "/");
                if (path_components.empty())
                    throw std::logic_error("Invalid JSON path provided");
            
                nlohmann::basic_json<> object = _root.at(path_components[0]);
                for (size_t i = 1; i < path_components.size(); i++)
                    object = object.at(path_components[i]);
                
                return object.template get<T>();
            }
            
            template <typename T>
            T deserialize(std::string path, const T& default_value) const {
                if (!_has_file_open)
                    throw std::runtime_error("Unable to parse JSON, no file is open!");
            
                std::vector<std::string> path_components = utils::split(path, "/");
                if (path_components.empty())
                    throw std::logic_error("Invalid JSON path provided");
            
                /* First one needs to be explicitly checked*/
                auto root_iter = _root.find(path_components[0]);
                if (root_iter == _root.cend())
                    return default_value;
                nlohmann::json object = *root_iter;

                for (size_t i = 1; i < path_components.size(); i++) {
                 
                    auto iter = object.find(path_components[i]); 
                    if(iter == object.end())
                        return default_value;
                    object = *iter;
                }
                
                return object.template get<T>();
            }
            
            template <typename T> 
            void serialize(std::string path, T& value) {
                std::vector<std::string> path_components = utils::split(path, "/");
                if (path_components.empty())
                    throw std::logic_error("Invalid JSON path provided");
            
                if (path_components.size() == 1)
                    _root[path_components[0]] = value;
            
                nlohmann::basic_json<>& object = _root[path_components[0]];
                size_t i = 1; /* Traverse through the structure */
            
                for (; i < path_components.size() - 1; i++)
                    object = object[path_components[i]];
            
                object[path_components[i]] = value;
            }

        private:
            nlohmann::json _root;
            std::string _source_path;
            bool _has_file_open;
    };
};


/* Parsers for common vector types */
MAKE_JSON_PARSABLE(glm::vec3, [] (const utils::resource& res){
    
    return glm::vec3(
        res.deserialize<float>("x"),
        res.deserialize<float>("y"),
        res.deserialize<float>("z")
    ); 
});

MAKE_JSON_PARSABLE(glm::quat, [] (const utils::resource& res){
    
    /* Create quat from euler angles */
    return glm::quat(glm::vec3(
        res.deserialize<float>("p"),
        res.deserialize<float>("y"),
        res.deserialize<float>("r")
    )); 
});