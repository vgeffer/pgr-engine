#pragma once
#include <exception>
#include <string>

#define MAKE_ERROR(T) class T : public base_error {   \
    public:                                 \
        T(const std::string& message)       \
            : base_error(message) {}        \
}

#define MAKE_MESSAGE(T) class T : public base_message {   \
    public:                                     \
        T() : base_message() {}                 \
}

namespace utils::exceptions {

    class base_error : public std::exception {
        public:
            base_error(const std::string& message)
                : m_message(message) {}

        const char* what() const throw() { return m_message.c_str(); }

        private:
            std::string m_message;
    };
    
    class base_message : public std::exception {
        public:
            base_message() {}
    };
    
    MAKE_ERROR(shader_compile_error);
    MAKE_ERROR(shader_link_error);
    MAKE_ERROR(shader_uniform_error);
    MAKE_ERROR(resource_load_error);    

    MAKE_MESSAGE(window_reload_message);
    MAKE_MESSAGE(app_reload_message);
}

#undef MAKE_ERROR
#undef MAKE_MESSAGE