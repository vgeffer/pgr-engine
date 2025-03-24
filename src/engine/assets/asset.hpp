#pragma once
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace assets {
    class asset {
        public:

            /// @brief How should assets be cached when loaded
            ///
            ///
            enum class caching_policy {
                NO_CACHE,       ///< Do not cache loaded asset, nor use cached instance (if exists)
                KEEPALIVE,      ///< Keep asset alive (in cache) even after all instances are destroyed
                DESTROY_UNUSED  ///< Free asset after the last instance is destroyed 
            };


            /// @brief Loads asset and provides asset caching
            /// 
            /// Loads asset of type T and depending on the caching policy caches appropriately. If a cached instance already exists and caching policy permits,
            /// the cached instance is used and asset is not loaded.
            /// @param path Filesystem path to the requested asset
            /// @param policy How asset cache should behave 
            /// @return Shared pointer to the loaded asset
            ///
            ///  @see caching_policy
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


            /// @brief Invalidates the keep-alive list
            /// 
            /// Invalidates the keep-alive list, destroying all the stored instances of the assets loaded with KEEPALIVE cache policy.
            /// All assets loaded with KEEPALIVE policy prior to calling this function will behave as if they were loaded with DESTROY_UNUSED policy.
            /// 
            /// @see caching_policy
            static void invalidate() { _keepalive_list.clear(); }



        private:
            static std::unordered_map<std::string, std::weak_ptr<asset>> _cache;
            static std::vector<std::shared_ptr<asset>> _keepalive_list; 
    };
}