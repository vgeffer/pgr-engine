#pragma once

/* Common files */
#include "Exceptions.hpp"        // IWYU pragma: keep
#include "SystemMacros.hpp"      // IWYU pragma: keep


#define PGR_NORETURN    [[noreturn]]
#define PGR_NODISCARD   [[nodiscard]]

#define ARRAY_SIZE(array) sizeof(array) / sizeof(array[0])

#define SELF std::remove_pointer<decltype(this)>::type

namespace pgr::Common { 

    static const struct {
        const char Name[16];
        const struct { 
            const uint16_t Major;
            const uint16_t Minor;
            const uint16_t Patch;
        } Version;
    } EngineInfo { "PgrEngine", { 1, 0, 0 }};

    class Component {
        public: 
            Component() = default;
            ~Component() = default;
            
            virtual bool IsValid() const noexcept { return false; }
    };
}
