#pragma once
#include <exception>
#include <string>

#define MAKE_ERROR(T) class T : public ErrorBase {   \
    public:                                 \
        T(const std::string& message)       \
            : ErrorBase(message) {}         \
}

#define MAKE_MESSAGE(T) class T : public MessageBase {   \
    public:                                     \
        T() : MessageBase() {}                  \
}

namespace pgr::Common::Exceptions {

    class ErrorBase : public std::exception {
        public:
            ErrorBase(const std::string& message)
                : mMessage(message) {}

        const char* what() const throw() { return mMessage.c_str(); }

        private:
            std::string mMessage;
    };
    
    class MessageBase : public std::exception {
        public:
            MessageBase() {}
    };
    

    MAKE_ERROR(InstanceExistsError);

    MAKE_MESSAGE(AppReloadMesage);
    MAKE_MESSAGE(WindowReloadMessage); 
}
