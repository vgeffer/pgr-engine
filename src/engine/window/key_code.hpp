#pragma once 
#include <GLFW/glfw3.h>

enum class key_code : uint16_t {
    A = GLFW_KEY_A,
    B = GLFW_KEY_B,
    C = GLFW_KEY_C,
    D = GLFW_KEY_D,
    E = GLFW_KEY_E,
    F = GLFW_KEY_F,
    G = GLFW_KEY_G,
    H = GLFW_KEY_H,
    I = GLFW_KEY_I,
    J = GLFW_KEY_J,
    K = GLFW_KEY_K,
    L = GLFW_KEY_L,
    M = GLFW_KEY_M,
    N = GLFW_KEY_N,
    O = GLFW_KEY_O,
    P = GLFW_KEY_P,
    Q = GLFW_KEY_Q,
    R = GLFW_KEY_R,
    S = GLFW_KEY_S,
    T = GLFW_KEY_T,
    U = GLFW_KEY_U,
    V = GLFW_KEY_V,
    W = GLFW_KEY_W,
    X = GLFW_KEY_X,
    Y = GLFW_KEY_Y,
    Z = GLFW_KEY_Z,
    
    UP_ARROW     = GLFW_KEY_UP,    
    DOWN_ARROW   = GLFW_KEY_DOWN,
    LEFT_ARROW   = GLFW_KEY_LEFT,
    RIGHT_ARROW  = GLFW_KEY_RIGHT,
    
    NUM0 = GLFW_KEY_0,
    NUM1 = GLFW_KEY_1,
    NUM2 = GLFW_KEY_2,
    NUM3 = GLFW_KEY_3,
    NUM4 = GLFW_KEY_4,
    NUM5 = GLFW_KEY_5,
    NUM6 = GLFW_KEY_6,
    NUM7 = GLFW_KEY_7,
    NUM8 = GLFW_KEY_8,
    NUM9 = GLFW_KEY_9,
    
    BACKSPACE = GLFW_KEY_BACKSPACE,
    SPACE = GLFW_KEY_SPACE,
    LSHIFT = GLFW_KEY_LEFT_SHIFT,
    RSHIFT = GLFW_KEY_RIGHT_SHIFT,
    LCTRL = GLFW_KEY_LEFT_CONTROL,
    RCTRL = GLFW_KEY_RIGHT_CONTROL,
    LALT = GLFW_KEY_LEFT_ALT,
    RALT = GLFW_KEY_RIGHT_ALT,
    ENTER = GLFW_KEY_ENTER,
    DEL = GLFW_KEY_DELETE,
    ESC = GLFW_KEY_ESCAPE
};

enum class mouse_code : uint16_t {
    right_button = GLFW_MOUSE_BUTTON_RIGHT,
    left_button = GLFW_MOUSE_BUTTON_LEFT,
    middle_button = GLFW_MOUSE_BUTTON_MIDDLE
};
