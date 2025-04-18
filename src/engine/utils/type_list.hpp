#pragma once 

#include <unordered_map>

namespace utils {

    template <class _Tp>
        class type_list {

        public:
            type_list() = default;
            type_list(const type_list& other) = delete;
            ~type_list() = default;

            template<typename _T>
               inline _Tp& get() { return m_storage.at(m_type_id<_T>()); }
                
            template <typename _T>
                inline bool has() const { return m_storage.find(m_type_id<_T>()) != m_storage.cend(); }

            inline std::unordered_map<int, _Tp>& get_all() { return m_storage; }

            template<typename _T>
                inline void set(const _Tp& val) { m_storage[m_type_id<_T>()] = val; }

            template<typename _T>
                inline void set(_Tp&& val) { m_storage[m_type_id<_T>()] = std::move(val); }

        private:
            inline static unsigned m_last_id = 0;
            std::unordered_map<int, _Tp> m_storage;

            template <typename T>
                unsigned m_type_id() const {

                    static unsigned id = m_last_id++;
                    return id;
                }
    };
};