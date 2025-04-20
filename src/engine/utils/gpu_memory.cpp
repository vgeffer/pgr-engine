#include "gpu_memory.hpp"
#include <cstddef>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../utils/logger.hpp"

using namespace std;
using namespace utils;

gpu_allocator::gpu_allocator(size_t base_size, GLbitfield buffer_hints) 
    : m_buffer_size(base_size), m_buffer_hints(buffer_hints) {

    logger::debug << "Allocating GPU buffer of size " << base_size << std::endl;

    glCreateBuffers(1, &m_buffer);
    glNamedBufferStorage(m_buffer, base_size, nullptr, GL_DYNAMIC_STORAGE_BIT); 
    m_chunks.emplace_front(base_size, 0, false); /* Insert root chunk */
}

gpu_allocator::~gpu_allocator() {

    glDeleteBuffers(1, &m_buffer);
}

pair<gpu_allocator::handle, size_t> gpu_allocator::alloc_buffer(size_t size) {

    /* Size must be aligned to 4 bytes */
    size += size % 4;

    bool chunk_found = false;
    handle chunk = m_chunks.begin();

    for (; chunk != m_chunks.end(); chunk++) {
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
        m_chunks.emplace_after(chunk, chunk->chunk_size - size, chunk->offset + size, false);
        chunk->chunk_size = size;
    }
    
    return make_pair(chunk, chunk->offset);
}

size_t gpu_allocator::buffer_data(const gpu_allocator::handle& handle, size_t data_size, const void* data) {

    if (handle == m_chunks.end() || !handle->used)
        throw runtime_error("Chunk not allocated, unable to write data!");

    size_t size_to_write = min(data_size, handle->chunk_size);
    if (size_to_write < data_size)
        logger::warn << "Requesting to write " << data_size << " bytes to a buffer of size " << handle->chunk_size
                     << ". Only " << size_to_write << " bytes will be written" << std::endl; 

    glNamedBufferSubData(m_buffer, handle->offset, size_to_write, data);
    logger::debug << "Buffer: " << m_buffer << ": Writing " << data_size << " bytes at offset " << handle->offset << ". " << size_to_write << " bytes written." << std::endl;
    return size_to_write;    
}

void gpu_allocator::free_buffer(const gpu_allocator::handle& handle) {

    if (handle == m_chunks.end() || !handle->used) {
        logger::error << "[In buffer: " << m_buffer << "] Double free! Attempting to free already freed block" << std::endl;
        return;
    }

    handle->used = false;

    /* Do block merging */
    uint blocks_merged;

    do {
        blocks_merged = 0;
        
        for (auto iter = m_chunks.begin(); iter != m_chunks.end(); iter++) {

            /* Conditions are not met */
            if (iter->used || iter._M_next() == m_chunks.end() || iter._M_next()->used)
                continue;

            /* Merge next to the current chunk and remove it */
            iter->chunk_size += iter._M_next()->chunk_size;
            m_chunks.erase_after(iter);
            blocks_merged++;
        }
    } while(blocks_merged > 0);
}

void gpu_allocator::free_all_and_resize(size_t new_size) {


    m_buffer_size = new_size;
    glNamedBufferStorage(m_buffer, new_size, nullptr, m_buffer_hints); 

    m_chunks.clear();
    m_chunks.emplace_front(new_size, 0, false);
}