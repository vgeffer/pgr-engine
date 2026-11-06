#pragma once
#include <stdexcept>
#include <string>
#include <exception>

#define MAKE_ERROR(T) class T : public _Private::ErrorBase {   \
    public:                                                    \
        T(const std::string& message)                          \
            : ErrorBase(message) {}                            \
}

#define MAKE_MESSAGE(T) class T : public _Private::MessageBase {   \
    public:                                                        \
        T() : MessageBase() {}                                     \
}

namespace pgr::Common::Exceptions {

    namespace _Private {
        class ErrorBase : public std::runtime_error {
            public:
                ErrorBase(const std::string& message)
                    : std::runtime_error(message) {}
        };

        class MessageBase : public std::exception {
            public:
                MessageBase() {}
        };
    }
    
    MAKE_ERROR(InstanceExistsError);
    MAKE_ERROR(IOError);
    MAKE_ERROR(NotFoundError);

    MAKE_MESSAGE(AppReloadMesage);
    MAKE_MESSAGE(WindowReloadMessage); 
}
