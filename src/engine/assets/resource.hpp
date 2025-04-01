#pragma once 
#include "asset.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include "../../lib/json/json.hpp"
#include "../utils/algorithms.hpp"
#include <iostream>

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

                    nlohmann::basic_json<> object = _json_root.at(path_components[0]);
                    for (size_t i = 1; i < path_components.size(); i++)
                        object = object.at(path_components[i]);
                    
                    return object.template get<T>();
                }

            template <typename T> 
                void serialize(std::string path, T& value) {
                    std::vector<std::string> path_components = utils::split(path, "/");
                    throw std::logic_error("NOT IMPLEMENTED");
                }

        private:
            nlohmann::basic_json<> _json_root;
            std::string _source_path;
            bool _has_file_open;
    };
};