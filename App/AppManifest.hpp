#pragma once 

#include "Common/SystemMacros.hpp"
#include <cstdint>
#include <Common/Common.hpp>
#include <cstring>

namespace pgr::App {
    struct AppManifest {
        
        enum class FlagBits : uint64_t {

            ManualDeviceSelect = 0,
            UseValidations = 1,
        };

        struct {
            uint16_t Major;
            uint16_t Minor; 
            uint16_t Patch; 
            uint16_t Build; 
        } Version;
           
        struct {
            uint64_t ManualDeviceSelection : 1;
            uint64_t UseValidations : 1;
            uint64_t Unused : 62;
        } Flags;

        char Name[32];
        char AppRoot[256];

        PGRAPI PGR_NODISCARD AppManifest& SetVersion(uint16_t major, uint16_t minor) noexcept 
            { Version = {major, minor, 0, 0}; return *this; }
        PGRAPI PGR_NODISCARD AppManifest& SetVersion(uint16_t major, uint16_t minor, uint16_t patch, uint16_t build) noexcept 
            { Version = {major, minor, patch, build}; return *this; }

        /// @todo maaybe not 
        PGRAPI PGR_NODISCARD AppManifest& SetFlags(uint64_t flags) 
            { *reinterpret_cast<uint64_t*>(&Flags) = flags; return *this; } 
        
        PGRAPI PGR_NODISCARD AppManifest& SetName(const char* name) { std::strncpy(Name, name, 31); Name[31] = '\0'; return  *this; }
        PGRAPI PGR_NODISCARD AppManifest& SetAppRoot(const char* path) { std::strncpy(AppRoot, path, 255); AppRoot[255] = '\0'; return *this; }
        PGRAPI PGR_NODISCARD AppManifest FromFile(const char* fileName) { return {}; }

    };
}
