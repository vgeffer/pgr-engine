#pragma once 

#include <unordered_map>

namespace utils {

    template <class _T>
        class type_list {

        public:
            type_list() = default;
            type_list(const type_list& other) = delete;
            ~type_list() = default;

            template<typename T>
               inline _T& get() { return _storage.at(_type_id<T>()); }
                
            template <typename T>
                inline bool has() const { return _storage.find(_type_id<T>()) != _storage.cend(); }

            inline std::unordered_map<int, _T>& get_all() { return _storage; }

            template<typename T>
                inline void set(const _T& val) { _storage[_type_id<T>()] = val; }

            template<typename T>
                inline void set(_T&& val) { _storage[_type_id<T>()] = std::move(val); }

        private:
            inline static unsigned _last_id = 0;
            std::unordered_map<int, _T> _storage;

            template <typename T>
                unsigned _type_id() const {

                static unsigned id = _last_id++;
                return id;
            }
    };
};