


#include <cstddef>
namespace utils {

    template <typename T> 
        class dynamic_alloc {

        public:
            explicit dynamic_alloc(size_t count) { _raw_memory = new T[count]; }
            dynamic_alloc(const dynamic_alloc<T>&) = default;
            ~dynamic_alloc() { delete [] _raw_memory; }
        
            T& operator*() { return *_raw_memory; }
            T operator=(T* other) { return _raw_memory = other; }
            T* operator->() { return _raw_memory; }
            operator T*() { return _raw_memory; }
        
        private:
            T* _raw_memory;
    };
};