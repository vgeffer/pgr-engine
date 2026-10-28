///
/// @file Application.hpp
/// @author geffevil
/// @copyright (c) geffevil 2025 - 2026
/// @brief Core class of the engine
///
/// This Source Code Form is subject to the terms of the Mozilla Public
/// License, v. 2.0. If a copy of the MPL was not distributed with this
/// file, You can obtain one at http://mozilla.org/MPL/2.0/.
///

#pragma once

#include <vulkan/vulkan.hpp>

#include <Common/Common.hpp>
#include <Core/Caching/Cache.hpp>
#include <App/AppManifest.hpp>
#include <Core/Rendering/Device.hpp>
#include <Core/Debug/VulkanValidation.hpp>

namespace pgr::App {

    class Application {

        public:
            enum Status { APP_RELOAD = -1, OK, ERROR };

        public:
            PGRAPI Application(const AppManifest& manifest);
            PGRAPI ~Application();
            PGRAPI PGR_NODISCARD Status Start();

            /* Static wrappers */
            PGR_NODISCARD inline static const double&                  GlobalClock()  { assert(self); return self->mGlobalClock; }
            //PGR_NODISCARD inline static const AppManifest*             Manifest()     { assert(self); return self->mManifest;    }
            PGR_NODISCARD inline static const Core::Rendering::Device& UsedDevice()   { assert(self); return self->mUsedDevice;  }
            //PGR_NODISCARD inline static       Core::Caching::Cache&    Cache()        { assert(self); return self->mCache;       }
        
        private:
            inline static Application* self = nullptr;

            /* All things here live forever */
            double                   mGlobalClock;
            Core::Rendering::Device  mUsedDevice;
            //Core::Caching::Cache     mCache;

            /* These are not exposed */
            vk::Instance             mVkInstance;
            Debug::VulkanValidation  mValidations;
            AppManifest*             mManifest;
        };
}
