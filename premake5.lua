workspace "raytracer"
    configurations { "Debug","Release" }
    language "C"
    cdialect "C17"
    warnings "Default"
    architecture "x64"
    targetdir "bin/%{cfg.architecture}/%{cfg.buildcfg}"

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        symbols "On"
        optimize "Off"
		debugdir "."

    filter "configurations:Release"
        defines { "NDEBUG" }
        symbols "Off"
        optimize "On"
		debugdir "."

	project "raytracer"
		kind "ConsoleApp"
		location "premake"
		files {
			"src/**.c",
			"src/**.h"
		}