#pragma once 
#include "Common/Exceptions.hpp"
#include <string>
#include <nlohmann/json.hpp>

#include <Common/Common.hpp>

#define MAKE_JSON_PARSABLE(T, parse)                      \
    namespace nlohmann {                                  \
        template <> struct adl_serializer<T> {            \
            static T from_json(const json& j) {           \
                return parse(pgr::Common::Resource(j));   \
            }                                             \
        };                                                 \
    }              

namespace pgr::Common {

    class Resource {
        public:
            Resource();
            Resource(const char* path);
            Resource(const nlohmann::json& json);

            void ToFile(const char* path = nullptr);

            template <typename T>
            inline T Get(const char* path) const {   
                const nlohmann::json& value = InternalGet(path);
                if (value == nullptr)
                    throw Exceptions::NotFoundError("Resource does not contain path " + std::string(path));

                return value.template get<T>(); 
            }
            
            template <typename T>
            inline T Get(const char* path, const T& defaultValue) const { 
                const nlohmann::json& value = InternalGet(path);
                if (value == nullptr)
                    return defaultValue;
                
                return value.template get<T>(); 
            }

            template <typename T>
            inline void Set(const char* path, const T& value) { InternalSet(path, value); }

        private:
            nlohmann::json InternalGet(const char* path) const;
            void InternalSet(const char* path, const nlohmann::json& object);

        private:
            nlohmann::json mRootNode; 
            std::string mSourcePath;
    };
};



/* Parsers for common vector types

MAKE_JSON_PARSABLE(glm::vec3, [] (const utils::resource& res){
    
    return glm::vec3(
        res.deserialize<float>("x", 0.0f),
        res.deserialize<float>("y", 0.0f),
        res.deserialize<float>("z", 0.0f)
    ); 
});

MAKE_JSON_PARSABLE(glm::quat, [] (const utils::resource& res){
    
    /* Create quat from euler angles 
    return glm::quat(glm::vec3(
        res.deserialize<float>("p",0.0f),
        res.deserialize<float>("y",0.0f),
        res.deserialize<float>("r",0.0f)
    )); 
});*/