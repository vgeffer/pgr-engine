#include <cstddef>

namespace utils {

    /// @brief A simple RAII buffer wrapping memory allocation
    template <typename T> 
    class buffer {

        public:
            /// @brief Constructor
            /// @param count Number of items of size @c T to be allocated
            explicit buffer(size_t count) { m_raw_memory = new T[count]; }
            buffer(const buffer<T>&) = default;
            ~buffer() { delete [] m_raw_memory; }
        
            T& operator*() { return *m_raw_memory; }
            T operator=(T* other) { return m_raw_memory = other; }
            T* operator->() { return m_raw_memory; }
            operator T*() { return m_raw_memory; }
        
        private:
            T* m_raw_memory;
    };
};