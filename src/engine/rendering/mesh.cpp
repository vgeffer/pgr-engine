#include "mesh.hpp"
#include "renderer.hpp"

using namespace rendering;

mesh_instance::~mesh_instance() {
    
    /* Re-link list */
    if (_prev) _prev->_next = _next; /* Prev exists - move node */
    else renderer::instance()->draw_list(_next); /* this was head, _next becomes new head */

    if (_next) _next->_prev = _prev; /* This node was not tail */

    /* Everything else should happen automatically */
}