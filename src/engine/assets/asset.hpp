#pragma once
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace assets {
    class asset {

        public:
            template<class T>
                static std::shared_ptr<T> load(std::string path) {

                /* Compile-time type checking */
                static_assert(std::is_base_of<asset, T>::value, "");

                /* Check if asset exists in cache */
                if (auto it = _cache.find(path); it != _cache.end()) {
                
                    return std::shared_ptr<T>(it->second);
                }

                /* Loading it for a first time */
                std::shared_ptr<T> ptr = std::make_shared<T>(ptr);
                _cache[path] = ptr;
                return ptr;
            }


        private:
            static std::unordered_map<std::string, std::shared_ptr<asset>> _cache;
    };
}