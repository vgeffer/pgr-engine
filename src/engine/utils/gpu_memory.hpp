#pragma once


#include <cstddef>
#include <forward_list>
#include "../../lib/glad/glad.h"

namespace utils {

    class gpu_allocator {

        private:
            struct _alloc_chunk {
                _alloc_chunk(size_t size, size_t off, bool used)
                    : chunk_size(size), offset(off), used(used) {}

                size_t chunk_size;
                size_t offset;
                bool used;
            };

        public:
            using handle = std::forward_list<_alloc_chunk>::iterator;

            gpu_allocator(size_t base_size, GLbitfield buffer_hints = 0);
            gpu_allocator(const gpu_allocator&) = delete;
            gpu_allocator(gpu_allocator&&) = delete;
            
            ~gpu_allocator();

            std::pair<handle, size_t> alloc_buffer(size_t size);
            size_t buffer_data(const handle& handle, size_t data_size, const void* data);
            void free_buffer(const handle& handle);

            void free_all_and_resize(size_t new_size);

            inline size_t buffer_size() const { return _buffer_size; }
            inline GLuint buffer() const { return  _buffer; }

        private:
            size_t _buffer_size;
            GLbitfield _buffer_hints;
            std::forward_list<_alloc_chunk> _chunks;

            GLuint _buffer;
    };
}