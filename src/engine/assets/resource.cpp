#include "resource.hpp"
#include "../../lib/json/json.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace assets;
using namespace nlohmann;

resource::resource(string path) 
    : _source_path(path), _has_file_open(false) {

    ifstream source_file = ifstream(path, ios::in | ios::binary);

    if (!source_file.is_open())
        return;

    _json_root = json::parse(
        istreambuf_iterator<char>(source_file),
        istreambuf_iterator<char>()
    );    

    _has_file_open = true;
}

void resource::save() {

    ofstream stream = ofstream(_source_path, ios::out);
    stream << _json_root;
}
