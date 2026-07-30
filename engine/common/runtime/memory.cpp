#include "memory.hpp"

#include <new>
#include <cstdlib>
#include "memory_traits.hpp"


using namespace pgreng::common::runtime;


memory::memory(uint32_t initial_usable_blocks, bool resizable) {

    malloc((static_cast<size_t>(initial_usable_blocks) + 1) * blocks::value); /* TODO: Determine Manager overhead */

    /* Lay down initial tree */

}

memory::memory(const memory_handle& memory) {

    /* Check if handle points to sufficiently large block of memory*/
    /* Lay down the tree */
}


/*
    Main alloc block structure:
    MAGIC: 8 bytes
    Size: 4 bytes
    Children: 8 bytes
    Non-Atomic refcounts: 16 bytes

    Flags: n Bytes
*/



memory_handle memory::request(uint32_t size) { 
    throw std::bad_alloc();
}
