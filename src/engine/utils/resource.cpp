#include "resource.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace utils;
using namespace nlohmann;

resource::resource(string path) 
    : m_source_path(path), m_has_file_open(false) {

    ifstream source_file = ifstream(path, ios::in | ios::binary);

    if (!source_file.is_open())
        return;

    m_root = json::parse(
        istreambuf_iterator<char>(source_file),
        istreambuf_iterator<char>()
    );    

    m_has_file_open = true;
}

resource::resource(const json& json)
    : m_root(json), m_has_file_open(true) {}


void resource::save() {
    ofstream stream = ofstream(m_source_path, ios::out);
    stream << m_root;
}
