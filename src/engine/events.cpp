#include "events.hpp"
#include "game_window.hpp"
#include "window/key_code.hpp"
    

events::events(game_window& win) {
    public void setDefaultCallbacks(EventHandler e) {
        e.addKeyCallback(2, (int key, int action, EventHandler caller) -> {
                         
             if (action == GLFW_PRESS) 
                 keyStates.put(key, DOWN);
             
             if (action == GLFW_RELEASE)
                 keyStates.put(key, UP);
         });
   
         e.addButtonCallback(2, (int button, int action, EventHandler caller) -> {
             
             if (action == GLFW_PRESS)
                 mouseStates.put(button, DOWN);
             
             if (action == GLFW_RELEASE)
                 mouseStates.put(button, UP);
         });
   
         e.addMoveCallback(2, (double posX, double posY, EventHandler caller) -> {
         
             cursorPos = new V2f((float)posX, (float)posY);
         });
     }
     
}

void events::process_frame() {
    prevCursorPos = cursorPos;
        
    for (int keyCode : keyStates.keySet()) {
        if (keyStates.get(keyCode) == DOWN) 
            keyStates.put(keyCode, HELD_DOWN);

        if (keyStates.get(keyCode) == UP)
            keyStates.put(keyCode, HELD_UP);
    }

    for (int button : mouseStates.keySet()) {
        if (mouseStates.get(button) == DOWN) 
            mouseStates.put(button, HELD_DOWN);

        if (mouseStates.get(button) == UP)
            mouseStates.put(button, HELD_UP);
    }

}

static bool events::is_key_pressed(key_code key) {        
        
    if (!keyStates.containsKey(keyCode.getKeyCode()))
        return false;
    
    return keyStates.get(keyCode.getKeyCode()) == DOWN; 
}
    
static bool events::is_key_held(key_code key) {
        
    /* Key has never been pressed */
    if (!keyStates.containsKey(keyCode.getKeyCode()))
        return false;
        
    return keyStates.get(keyCode.getKeyCode()) == HELD_DOWN; 
}

static bool events::is_key_released(key_code key) {
    
    /* Key has never been pressed */
    if (!keyStates.containsKey(keyCode.getKeyCode()))
        return false;
    
    return keyStates.get(keyCode.getKeyCode()) == UP; 
}
    
    /**
     * Returns if the mouse button has been pressed (only occurs on the first frame of the button being pressed)
     * @param button Button we're checking for
     * @return Is button pressed
     */  
    public boolean isMouseButtonPressed(Mouse button) {
        
        /* Key has never been pressed */
        if (!mouseStates.containsKey(button.getButtonCode()))
            return false;
        
        return mouseStates.get(button.getButtonCode()) == DOWN; 
    }
    
    /**
     * Returns if the mouse button has been held down (occurs from the second frame of the button being pressed)
     * @param button Button code we're checking for
     * @return Is button held down
     */    
    public boolean isMouseButtonHeldDown(Mouse button) {
        
        /* Key has never been pressed */
        if (!mouseStates.containsKey(button.getButtonCode()))
            return false;
            
        return mouseStates.get(button.getButtonCode()) == HELD_DOWN; 
    }
    
     /**
     * Returns if the button has been released (only occurs on the first frame of the button being released)
     * @param button Button code we're checking for
     * @return Is button released
     */  
    public boolean isMouseButtonReleased(Mouse button) {
        
        /* Key has never been pressed */
        if (!mouseStates.containsKey(button.getButtonCode()))
            return false;
        
        return mouseStates.get(button.getButtonCode()) == UP; 
    }
    
    public V2f getMousePos() {
        return cursorPos;
    }
    
    public V2f getMouseDelta() {
        return V2f.sub(cursorPos, prevCursorPos);
    }
}
