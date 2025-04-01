#pragma once 

#include <vector>
namespace utils {

    template <class _T>
        class type_list {

        public:
            type_list() = default;
            type_list(const type_list& other) = delete;
            ~type_list() = default;

            template<typename T>
                _T& get() { return _storage.at(_type_id<T>()); }
                
            template <typename T>
                bool has() const { return _type_id<T>() < _storage.size(); }

            std::vector<_T>& get_all() { return _storage; }

            template<typename T>
                void set(_T& val) { _storage[_type_id<T>()] = val; }

        private:
            inline static unsigned _last_id = 0;
            std::vector<_T> _storage;

            template <typename T>
                unsigned _type_id() const {

                static unsigned id = _last_id++;
                return id;
            }
    };
};