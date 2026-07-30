#pragma once

#include <array>
#include <iostream>
#include <ostream>


#if defined(_NDEBUG)
    #define DEBUGIF(condition) if (false)
#else
    #define DEBUGIF(condition) if (condition)
#endif


namespace pgreng::common {
    class debug {
        public:
            enum class level : uint8_t {
                PROJECT_RELEASE,    ///< Least verbose logs, only errors reported
                PROJECT_DEBUG,      ///< Errors and warnings reported
                ENGINE_DEBUG        ///< Most verbose logs, every vulkan message passes through
            };

        public:
            static inline std::ostream& log   = std::cout;
            static inline std::ostream& info  = std::cout;
            static inline std::ostream& warn  = std::cerr;
            static inline std::ostream& error = std::cerr;

        public:
            debug();
            ~debug();

        private: 
            inline static debug* s_instance = nullptr;               
            level m_dbg_level;       
    };
}