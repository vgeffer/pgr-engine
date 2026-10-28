#include <cstring>
#include <stdexcept>
#include <vector>
#include <exception>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <Common/Common.hpp>
#include <Core/Presentation/Window.hpp>

#include "Application.hpp"
#include "App/AppManifest.hpp"
#include "Common/Exceptions.hpp"
#include "Core/Debug/VulkanValidation.hpp"
#include "Core/Rendering/Device.hpp"
#include "Core/Rendering/Renderer.hpp"

using namespace pgr::App;
using namespace std::chrono;

Application::Application(const AppManifest& manifest) {

    if (self != nullptr) /* Only one app object per process */
        throw Common::Exceptions::InstanceExistsError("Another app instance is already running!");

    /* Initialize GLFW */
    if (!glfwInit())
        throw std::runtime_error("GLFW context initialization failed!");

    try {

        /* Alloc manifest in memory */
        mManifest = new AppManifest{};
        std::memcpy(mManifest, &manifest, sizeof(AppManifest)); //TODO: Memory mannager

        mManifest->Flags.ManualDeviceSelection &= Debug::VulkanValidation::IsAvailable(); /* TODO: Notify user Disable validations if not available */

        /* Extensions - autoregister window */
        std::vector<const char*> usedExtensions = Core::Presentation::Window::RequiredVkExtensions(),
                                 usedLayers;

        /* Copy debug extensions */
        if (mManifest->Flags.UseValidations) {
            for (const auto& ext : Debug::VulkanValidation::RequiredVulkanExtensions) usedExtensions.push_back(ext);
            for (const auto& layer : Debug::VulkanValidation::RequiredVulkanLayers) usedLayers.push_back(layer);
        }

        /* Setup vulkan instance */
        vk::ApplicationInfo appInfo = vk::ApplicationInfo()
            .setPApplicationName(mManifest->Name)
            .setApplicationVersion(VK_MAKE_VERSION(mManifest->Version.Major, mManifest->Version.Minor, mManifest->Version.Patch))
            .setPEngineName(Common::EngineInfo.Name)
            .setEngineVersion(VK_MAKE_VERSION(Common::EngineInfo.Version.Major, Common::EngineInfo.Version.Minor, Common::EngineInfo.Version.Patch))
            .setApiVersion(VK_API_VERSION_1_1);
           
        vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo()
            .setPApplicationInfo(&appInfo)
            .setPEnabledExtensionNames(usedExtensions)
            .setPEnabledLayerNames(usedLayers);

        mVkInstance = vk::createInstance(instanceInfo);
        if (mManifest->Flags.UseValidations) mValidations.Init(mVkInstance);
        
    } catch (std::exception& e) {

        /* Early exit due to app initialization errors */
        std::cerr << "[FATAL ERROR]: " << e.what() << std::endl;
        exit(1);
    }

    self = this;
}

Application::~Application() { 
    
    mValidations.Destroy(mVkInstance);
    mVkInstance.destroy();
    glfwTerminate();

    delete mManifest; 
    self = nullptr; 
}

Application::Status Application::Start() {

    Application::Status ret = OK;

    Core::Presentation::VideoMode mode = Core::Presentation::VideoMode();
    Core::Presentation::Window window = Core::Presentation::Window(mVkInstance, "Title", mode);
    
    /* Select device */
    const auto devices = Core::Rendering::Device::EnumerateAvailable(mVkInstance, window);
    if (devices.empty()) { 
        window.Destroy(mVkInstance); // Window is not RAII, needs to be cleaned manually
        throw std::runtime_error("No vulkan devices available");
    }

    if (mManifest->Flags.ManualDeviceSelection) {
        /* TODO: somehow present the selection function to the parent app */
        /* For now - just use simple TUI */
        std::cout << "\nAvailable devices:\n";

        for (int i = 0; i < devices.size(); i++)
            std::cout << "[" << i + 1 << "] " << devices[i].vkPhysDevice.getProperties().deviceName << "\n";
        std::cout << "\nSelect Device >> " << std::flush;
        
        int devID = -1;
        std::cin >> devID;

        if (devID - 1 < 0 || devID - 1 >= devices.size()) {
            window.Destroy(mVkInstance);
            throw std::runtime_error("Device index out of range!");
        }
        mUsedDevice = Core::Rendering::Device::Select(devices[devID - 1]);

    }
    else mUsedDevice = Core::Rendering::Device::Select(devices[0]);


    while (true) {
        /* Setup timekeeping */
        system_clock::time_point tpPrev = system_clock::now();
        window.Present();


        try {
            
            /* 
             * Renderer must always be destroyed as soon as mainloop ends.
             * This will not affect any created objects, those should be bound to device 
             * This will only occur when Window is reloaded (swapchain needs to be rebuilt), 
             * App is reloaded or there is a fatal error (go through regular cleanup anyway)
             */
            Core::Rendering::Renderer renderer = Core::Rendering::Renderer(window);

            /* Mainloop */
            while (!window.IsClosing()) {
                glfwPollEvents();

                

                /* 5. Time keeping*/
                system_clock::time_point tpNow = system_clock::now();
                float elapsed = duration_cast<milliseconds>(tpNow - tpPrev).count() / 1000.0f;
                tpPrev = tpNow;
                mGlobalClock += elapsed;
                
                /* 8. Wait for frame to be rendered */
                renderer.Dispatch();
            }
        } 
        catch (Common::Exceptions::WindowReloadMessage) { 

            /* Try preparing new window */
            /* if new window is not compatible with the old device, notify the app and issue APP_RELOAD */

            if (mUsedDevice.SupportsPresentation(window)) { /* destroy old window */ continue; }
            ret = APP_RELOAD;
        }
        catch (Common::Exceptions::AppReloadMesage)     { ret = APP_RELOAD; /* Go through usual cleanup */ }
        catch (std::exception e)                        { /*debug::error << e.what() << std::endl; */ ret = ERROR; }

        /* Cleanup */
        mUsedDevice.WaitToFinish();     // Wait until rendering is done - device should not be busy but just to be safe
        //mAssetLoader.invalidate();      // Clean up all assets

        window.Destroy(mVkInstance);
        mUsedDevice.Destroy();
        return ret;
    }
}

