#include <cstdint>
#include <unordered_map>
#include "AppManifest.hpp"
using namespace pgr::App;

static const std::unordered_map<char, uint64_t> fileSizeMap = {
    { 'k', 1024 }, { 'M', 1'048'576 }, { 'G', 1'073'741'824 }
};
 

