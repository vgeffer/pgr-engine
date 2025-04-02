#include <cstddef>

namespace utils {

    template <typename T> 
        class buffer {

        public:
            explicit buffer(size_t count) { _raw_memory = new T[count]; }
            buffer(const buffer<T>&) = default;
            ~buffer() { delete [] _raw_memory; }
        
            T& operator*() { return *_raw_memory; }
            T operator=(T* other) { return _raw_memory = other; }
            T* operator->() { return _raw_memory; }
            operator T*() { return _raw_memory; }
        
        private:
            T* _raw_memory;
    };
};