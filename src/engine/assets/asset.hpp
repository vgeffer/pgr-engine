#pragma once
#include <string>
#include <type_traits>

namespace assets {
    class asset {

        public:
            template<class T>
            static T* load(std::string path) {

                /* Compile-time type checking */
                static_assert(std::is_base_of<asset, T>::value, "");

                /* TODO: implement cache */

                T* inst = new T(path);
                return inst;
            }

        protected:
            explicit asset();
    };
}