#pragma once
#include <common/common.hpp>
#include <type_traits>

namespace pgreng::common::runtime {

    //template <typename T>
    //struct alignas(sizeof(void*)) handle { 
    //    T& operator*() const { return *static_cast<T*>(m_ptr); }
    //    T* operator->() const noexcept { return static_cast<T*>(m_ptr); }
    //    private:
    //        void* m_ptr; /* TODO: Update when memalloc is rolled out */
    //};

    template <typename T>
    struct handle {
        
        T* operator->() const noexcept { return m_ptr; }
        
        private:
            T* m_ptr;
    };
    
    struct memory_handle {

    };


    class memory {

        public:
            memory();
            memory(uint32_t init_usable_pages, bool resizable = false);
            memory(const memory_handle& memory);

            memory(const memory&) = delete;
            memory(memory&&);
            ~memory();

            PGR_NODISCARD memory_handle request(uint32_t blocks);
            PGR_NODISCARD memory_handle aux_request(uint32_t size); //Can alloc 16T of main pool and 4G of aux pool
            void release(memory_handle memory);

            PGR_NODISCARD int to_stl_allocator();

        private:
            void *m_base_ptr;
    };
}