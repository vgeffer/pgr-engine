workspace "pgr-engine"
    configurations { "Debug", "Release" }
    location "build"

    project "glad"
        language "C"
        kind "StaticLib"    -- Glad should be pretty much unaffected by anything, as it is an external lib
        targetdir "build/%{cfg.buildcfg}"
        optimize "On"
        files { 
            "src/lib/glad/**.h",
            "src/lib/glad/**.c", 
        }

    project "engine"
        language "C++"
        cppdialect "C++17"
        targetdir "build/%{cfg.buildcfg}"
        files {
            "src/engine/**.hpp",
            "src/engine/**.cpp", 
        }

        links { "glad:static", "glm:shared", "glfw:shared", "assimp:shared" }
        buildoptions { "-pipe", "-fPIC", "-Wall" }
        includedirs { "src/lib" }

        filter "configurations:Debug"
            kind "ConsoleApp"
            symbols "On"

        filter "configurations:Release"
            kind "WindowedApp"
            defines { "NDEBUG" }
            optimize "On"
