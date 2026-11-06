#include <exception>
#include <fstream>
#include <ostream>
#include <string>

#include <Common/Common.hpp>
#include "Resource.hpp"
#include "Common/Exceptions.hpp"
#include "Common/Utils/algorithms.hpp"
#include "nlohmann/json_fwd.hpp"

using namespace pgr::Common;

Resource::Resource() : mRootNode(), mSourcePath() {}
Resource::Resource(const nlohmann::json& json) : mRootNode(json), mSourcePath() {}

Resource::Resource(const char* path) : mSourcePath(path) {

    std::ifstream sourceFile = std::ifstream(path, std::ios::in | std::ios::binary);
    if (!sourceFile.is_open()) 
        throw Exceptions::IOError("Unable to open requested resource " + std::string(path)); /* Throw */

    mRootNode = nlohmann::json::parse(
        std::istreambuf_iterator<char>(sourceFile),
        std::istreambuf_iterator<char>()
    );    
}

void Resource::ToFile(const char* path) {

    assert(mSourcePath.empty() && path == nullptr);

    const char* usedPath = path == nullptr ? mSourcePath.c_str() : path; /* At least one needs to be present */
    std::ofstream stream = std::ofstream(usedPath, std::ios::out);

    stream << mRootNode << std::flush;
    stream.close();
}

nlohmann::json Resource::InternalGet(const char* path) const {

    nlohmann::json walkNode = mRootNode;

    // TODO: Relative paths
    if (*path != '/') assert(false);
    else ++path;    // Move along after consuming the starting /

    for (const auto& component : Utils::Split(path, '/')) {
        try                   { walkNode = walkNode.at(component); }
        catch(std::exception) { return nullptr; } //If key is not found, error and let app deal with it
    }
    
    return walkNode;
}

void Resource::InternalSet(const char* path, const nlohmann::json& value) {

    nlohmann::json& walkNode = mRootNode;

    // TODO: Relative paths
    if (*path != '/') assert(false);
    else ++path;    // Move along after consuming the starting /
    
    size_t i = 0; /* Traverse through the structure */
    const auto& components = Utils::Split(path, '/');
            
    for (; i < components.size() - 1; i++)    
        walkNode = walkNode[components[i]];
                
    walkNode[components[i]] = value;
}