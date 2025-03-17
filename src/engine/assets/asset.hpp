#pragma once
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace assets {
    class asset {
        public:
            enum class caching_policy {
                NO_CACHE, KEEPALIVE, DESTROY_UNUSED
            };

            template<class T>
                static std::shared_ptr<T> load(std::string path, caching_policy policy = caching_policy::DESTROY_UNUSED) {

                /* Compile-time type checking */
                static_assert(std::is_base_of<asset, T>::value, "");

                /* Completly bypass cache, usefull mainly for debugging shaders */
                if (policy == caching_policy::NO_CACHE) 
                    return std::make_shared<T>(path);
           
                auto object = _cache[path].lock();
                if (!object) {

                    /* Loading it for a first time */
                    std::shared_ptr<T> ptr = std::make_shared<T>(path);
                    if (policy == caching_policy::KEEPALIVE)
                        _keepalive_list.push_back(ptr); /* Keep object alive even if all of its instances were destroyed */
                    
                    _cache[path] = std::static_pointer_cast<asset>(ptr);
                    return ptr;
                }
                return std::static_pointer_cast<T>(object);
            }

            static inline void invalidate() { _keepalive_list.clear(); }

        private:
            static std::unordered_map<std::string, std::weak_ptr<asset>> _cache;
            static std::vector<std::shared_ptr<asset>> _keepalive_list; 
    };
}