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

    /// @brief Wrapper class around a @c nlohmann::json object
    class resource {
        public:

            /// @brief File constructor - Constructs a resource from a file
            ///
            /// Opens, reads and parses a JSON file, creating a resource out of it
            /// If the file does not exist or there is an I/O error, resource is still
            /// created, however it can not be read from. It can be written to or saved.
            /// @param path Filesystem path to the resource file
            resource(std::string path);

            /// @brief Constructs a resource from a JSOn object
            /// @param json JSON object to wrap
            resource(const nlohmann::json& json);

            /// @brief Saves underlying JSON object to a file 
            ///
            /// Saves a JSON object to a file. The filename provided by the file constructor is used
            void save();

            nlohmann::json res() const { return m_root; }
            
            /// @brief Deserializes an object of type @c T 
            ///
            /// @c path takes in a path, similar to a Linux filesystem path, allowing the descend
            /// through the structure. If the path points to a non-existing key, an exception is thrown.
            /// @example deserialize("root/object1/objec2/value");
            /// @param path Path of an object to be read
            template <typename T>
            T deserialize(std::string path) const {
                if (!m_has_file_open)
                    throw std::runtime_error("Unable to parse JSON, no file is open!");
            
                std::vector<std::string> path_components = split(path, "/");
                if (path_components.empty())
                    throw std::logic_error("Invalid JSON path provided");
            
                nlohmann::basic_json<> object = m_root.at(path_components[0]);
                for (size_t i = 1; i < path_components.size(); i++)
                    object = object.at(path_components[i]);
                
                return object.template get<T>();
            }
            
            /// @brief Deserializes an object of type @c T 
            ///
            /// @c path takes in a path, similar to a Linux filesystem path, allowing the descend
            /// through the structure. If the path points to a non-existing key, @c default_value is returned instead.
            /// @example deserialize("root/object1/objec2/value");
            ///
            /// @param path Path of an object to be rea
            /// @param default_value Default value to use when key is not found
            template <typename T>
            T deserialize(std::string path, const T& default_value) const {
                if (!m_has_file_open)
                    throw std::runtime_error("Unable to parse JSON, no file is open!");
            
                std::vector<std::string> path_components = utils::split(path, "/");
                if (path_components.empty())
                    throw std::logic_error("Invalid JSON path provided");
            
                /* First one needs to be explicitly checked*/
                auto root_iter = m_root.find(path_components[0]);
                if (root_iter == m_root.cend())
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
            
            /// @brief Serializes an object of type @c T 
            ///
            /// @c path takes in a path, similar to a Linux filesystem path, allowing the descend
            /// through the structure. All parent keys must exist, otherwise an exception is thrown
            /// @example serialize("root/object1/objec2/value", obj);
            ///
            /// @param path Path of an object to be rea
            /// @param default_value Default value to use when key is not found
            template <typename T> 
            void serialize(std::string path, const T& value) {
                std::vector<std::string> path_components = utils::split(path, "/");
                if (path_components.empty())
                    throw std::logic_error("Invalid JSON path provided");
            
                if (path_components.size() == 1)
                    m_root[path_components[0]] = value;
            
                nlohmann::basic_json<>& object = m_root[path_components[0]];
                size_t i = 1; /* Traverse through the structure */
            
                for (; i < path_components.size() - 1; i++)
                    object = object[path_components[i]];
            
                object[path_components[i]] = value;
            }

        private:
            nlohmann::json m_root; 
            std::string m_source_path;
            bool m_has_file_open;
    };
};


/* Parsers for common vector types */
MAKE_JSON_PARSABLE(glm::vec3, [] (const utils::resource& res){
    
    return glm::vec3(
        res.deserialize<float>("x", 0.0f),
        res.deserialize<float>("y", 0.0f),
        res.deserialize<float>("z", 0.0f)
    ); 
});

MAKE_JSON_PARSABLE(glm::quat, [] (const utils::resource& res){
    
    /* Create quat from euler angles */
    return glm::quat(glm::vec3(
        res.deserialize<float>("p",0.0f),
        res.deserialize<float>("y",0.0f),
        res.deserialize<float>("r",0.0f)
    )); 
});