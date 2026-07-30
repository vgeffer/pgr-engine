#pragma once

namespace pgreng::common {
    class resource {
   
        public:
            template <typename T>
            struct deserializer { const T& operator()(); };

            template <typename T>
            struct serializer { const void operator()(); };
        

        template <typename T>
        const T& get(const char* path) {
            /* Path resolution */
            deserializer<T>();
        }

    };
}