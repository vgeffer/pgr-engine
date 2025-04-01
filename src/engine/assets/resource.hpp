#pragma once 
#include "asset.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include "../../lib/json/json.hpp"
#include "../utils/algorithms.hpp"

namespace assets {

    class resource : public asset {

        public:
            resource(std::string path);
            void save();

            template <typename T>
                T deserialize(std::string path) {
                    if (!_has_file_open)
                        throw std::runtime_error("Unable to parse JSON, no file is open!");

                    std::vector<std::string> path_components = utils::split(path, "/");
                    if (path_components.empty())
                        throw std::logic_error("Invalid JSON path provided");

                    nlohmann::basic_json<> object = _json_root.at(path_components[0]);
                    for (size_t i = 1; i < path_components.size(); i++)
                        object = object.at(path_components[i]);
                    
                    return object.template get<T>();
                }

            template <typename T> 
                void serialize(std::string path, T& value) {
                    std::vector<std::string> path_components = utils::split(path, "/");
                    if (path_components.empty())
                        throw std::logic_error("Invalid JSON path provided");

                    if (path_components.size() == 1)
                        _json_root[path_components[0]] = value;

                    nlohmann::basic_json<>& object = _json_root[path_components[0]];
                    size_t i = 1; /* Traverse through the structure */

                    for (; i < path_components.size() - 1; i++)
                        object = object[path_components[i]];

                    object[path_components[i]] = value;
                }

        private:
            nlohmann::basic_json<> _json_root;
            std::string _source_path;
            bool _has_file_open;
    };
};