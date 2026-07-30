#pragma once
#include <common/common.hpp>
#include <common/runtime/memory.hpp>

/* Handles */

    typedef pgreng::common::runtime::handle<struct backend>  hbackend;
    typedef pgreng::common::runtime::handle<struct window>   hwindow;
    typedef pgreng::common::runtime::handle<struct graphics> hgfx;

/* Enums */
    enum class window_params : uint16_t {
        IWIDTH = 1 << 0, IHEIGHT = 1 << 1, IWIN_MODE = 1 << 2
    };


/* Well-defined types */
struct alignas(64) backend_metadata {
    char name[16];
    uint32_t device_selection : 1; ///< Wether or not the rendering device can be selected manually
    uint32_t reserved : 31;
};

struct window_event {
    enum class event_type : uint8_t {
        KEYBOARD, 
    } type;

    union alignas(4) {
        struct { uint16_t key; enum state : uint8_t { UP, DOWN } state; } kbd_event;
    };
};
