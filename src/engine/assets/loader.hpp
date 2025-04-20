///
/// @file asset.hpp
/// @author geffevil
/// @brief A base class for assets and a simple synchronous asset loader with caching
///
#pragma once

#include "asset.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "../utils/logger.hpp"

namespace assets {

    /// @todo [Long-Term]: make it into a base class and create designated threaded loader
    class loader {
        public:
            /// @brief How should assets be cached when loaded
            enum class caching_policy {
                NO_CACHE,       ///< Do not cache loaded asset, nor use cached instance (if exists)
                KEEPALIVE,      ///< Keep asset alive (in cache) even after all instances are destroyed
                DESTROY_UNUSED  ///< Free asset after the last instance is destroyed 
            };

        public:
            loader() { s_instance = this; }
            ~loader() { s_instance = nullptr; }

            /// @brief Loads asset and provides asset caching
            /// 
            /// Loads asset of type @c T and depending on the caching policy caches appropriately. If a cached instance already exists and caching policy permits,
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

                /* Run-time sanity checking */
                if (s_instance == nullptr)
                    throw std::logic_error("Attempting to access an uninitialized cache!");

                logger::debug << "Loading " << path;

                /* Completly bypass cache, usefull mainly for debugging shaders */
                if (policy == caching_policy::NO_CACHE) {
                    logger::debug << " - cache bypassed" << std::endl;
                    return std::make_shared<T>(path);
                }

                auto object = s_instance->m_cache[path].lock();
                if (!object) {

                    logger::debug << " - cache miss, loading from file" << std::endl;
                    
                    /* Loading it for a first time */
                    std::shared_ptr<T> ptr = std::make_shared<T>(path);
                    if (policy == caching_policy::KEEPALIVE)
                        s_instance->m_keepalive_list.push_back(std::dynamic_pointer_cast<asset>(ptr)); /* Keep object alive even if all of its instances were destroyed */
                    
                    s_instance->m_cache[path] = std::static_pointer_cast<asset>(ptr);
                    return ptr;
                }

                logger::debug << " - reusing cached" << std::endl;
                return std::static_pointer_cast<T>(object);
            }

            /// @brief Invalidates the keep-alive list
            /// 
            /// Invalidates the keep-alive list, destroying all the stored instances of the assets loaded with @c KEEPALIVE cache policy.
            /// All assets loaded with @c KEEPALIVE policy prior to calling this function will behave as if they were loaded with @c DESTROY_UNUSED policy.
            /// 
            /// @see caching_policy
            static void invalidate() { 
                if (s_instance != nullptr)              
                    s_instance->m_keepalive_list.clear(); 
            }


        private:
            inline static loader* s_instance = nullptr;

            std::unordered_map<std::string, std::weak_ptr<asset>> m_cache; ///< Cache itself
            std::vector<std::shared_ptr<asset>> m_keepalive_list;
    };
}