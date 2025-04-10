#include "gpu_memory.hpp"
#include <cstddef>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include "project_settings.hpp"

using namespace std;
using namespace utils;

gpu_allocator::gpu_allocator(size_t base_size, GLbitfield buffer_hints) 
    : _buffer_size(base_size), _buffer_hints(buffer_hints) {

    std::cerr << "Allocating GPU buffer of size " << project_settings::gpu_buffer_alloc_size() << std::endl;

    glGenBuffers(1, &_buffer);
    glNamedBufferStorage(_buffer, base_size, nullptr, buffer_hints); 
    _chunks.emplace_front(base_size, 0, false); /* Insert root chunk */
}

gpu_allocator::~gpu_allocator() {

    glDeleteBuffers(1, &_buffer);
}

pair<gpu_allocator::handle, size_t> gpu_allocator::alloc_buffer(size_t size) {

    /* Size must be aligned to 4 bytes */
    size += size % 4;

    bool chunk_found = false;
    handle chunk = _chunks.begin();

    for (; chunk != _chunks.end(); chunk++) {
        if (!chunk->used && chunk->chunk_size >= size) {
            chunk_found = true;
            break;
        }
    }

    if (!chunk_found)
        throw runtime_error("Buffer out of memory, allocation of size " + to_string(size) + " bytes failed!");

    /* Mark chunk used */
    chunk->used = true;

    /* Check if leftover space is larger than single vec4 - smallest unit of data */
    if (chunk->chunk_size - size >= sizeof(glm::vec4)) {

        /* Emit new chunk with the size of chunk_size - size */
        _chunks.emplace_after(chunk, chunk->chunk_size - size, chunk->offset + size, false);
        chunk->chunk_size = size;
    }

    return make_pair(chunk, chunk->offset);
}

size_t gpu_allocator::buffer_data(const gpu_allocator::handle& handle, size_t data_size, const void* data) {

    if (handle == _chunks.end() || !handle->used)
        throw runtime_error("Chunk not allocated, unable to write data!");

    size_t size_to_write = min(data_size, handle->chunk_size);
    if (size_to_write < data_size)
        std::cerr << "Warning: Requesting to write " << data_size << " bytes to a buffer of size " << handle->chunk_size 
                  << ". Only " << size_to_write << " bytes will be written\n"; 

    glNamedBufferSubData(_buffer, handle->offset, size_to_write, data);
    return size_to_write;    
}

void gpu_allocator::free_buffer(const gpu_allocator::handle& handle) {

    if (handle == _chunks.end() || !handle->used) {
        std::cerr << "Double free! Attempting to free already freed block" << std::endl;
        return;
    }

    handle->used = false;

    /* Do block merging */
    uint blocks_merged;

    do {
        blocks_merged = 0;
        
        for (auto iter = _chunks.begin(); iter != _chunks.end(); iter++) {

            /* Conditions are not met */
            if (iter->used || iter._M_next() == _chunks.end() || iter._M_next()->used)
                continue;

            /* Merge next to the current chunk and remove it */
            iter->chunk_size += iter._M_next()->chunk_size;
            _chunks.erase_after(iter);
            blocks_merged++;
        }
    } while(blocks_merged > 0);
}

void gpu_allocator::free_all_and_resize(size_t new_size) {


    _buffer_size = new_size;
    glNamedBufferStorage(_buffer, new_size, nullptr, _buffer_hints); 

    _chunks.clear();
    _chunks.emplace_front(new_size, 0, false);
}