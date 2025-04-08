#include "resource.hpp"
#include "algorithms.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace utils;
using namespace nlohmann;

resource::resource(string path) 
    : _source_path(path), _has_file_open(false) {

    ifstream source_file = ifstream(path, ios::in | ios::binary);

    if (!source_file.is_open())
        return;

    _root = json::parse(
        istreambuf_iterator<char>(source_file),
        istreambuf_iterator<char>()
    );    

    _has_file_open = true;
}

resource::resource(const resource::component& json)
    : _has_file_open(true), _root(json) {}

template <typename T>
T resource::deserialize(std::string path) {
    if (!_has_file_open)
        throw std::runtime_error("Unable to parse JSON, no file is open!");

    std::vector<std::string> path_components = utils::split(path, "/");
    if (path_components.empty())
        throw std::logic_error("Invalid JSON path provided");

    nlohmann::basic_json<> object = _root.at(path_components[0]);
    for (size_t i = 1; i < path_components.size(); i++)
        object = object.at(path_components[i]);
    
    return object.template get<T>();
}

template <typename T>
T resource::deserialize(std::string path, const T& default_value) {
    if (!_has_file_open)
        throw std::runtime_error("Unable to parse JSON, no file is open!");

    std::vector<std::string> path_components = utils::split(path, "/");
    if (path_components.empty())
        throw std::logic_error("Invalid JSON path provided");

    nlohmann::basic_json<> object = _root.at(path_components[0]);
    for (size_t i = 1; i < path_components.size(); i++) {
     
        if (auto iter = object.find(path_components[i]); iter == object.end())
            return default_value;

        object = object.at(path_components[i]);
    }
    
    return object.template get<T>();
}

template <typename T> 
void resource::serialize(std::string path, T& value) {
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

void resource::save() {
    ofstream stream = ofstream(_source_path, ios::out);
    stream << _root;
}

