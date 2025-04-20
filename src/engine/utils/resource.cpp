#include "resource.hpp"
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

resource::resource(const json& json)
    : _root(json), _has_file_open(true) {}


void resource::save() {
    ofstream stream = ofstream(_source_path, ios::out);
    stream << _root;
}
