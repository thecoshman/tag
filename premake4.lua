dofile("glsdk_0_5_2/links.lua")

solution "TAG"
    configurations {"Debug", "Release"}
    defines {"_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS"}
    targetdir "bin"

project "TAG_Test"
    kind "ConsoleApp"
    language "c++"
    includedirs {"GLDR/src", "catch", "src", "include"}
    files {
        "src/**.cpp",
        "test.cpp"
    }
    excludes {
        "src/main.cpp"
    }
    
    libdirs { "/usr/lib/x86_64-linux-gnu", "/usr/local/lib" }
    UseLibs {"glload", "glimage", "glutil", "glmesh", "glm", "freeglut", "boost"}

    configuration "windows"
        defines "WIN32"
        links {"glu32", "opengl32", "gdi32", "winmm", "user32"}
        
    configuration "linux"
        links {"GL", "GLU", "X11", "Xinerama", "Xi", "Xxf86vm", "Xcursor", "Xrandr", "pthread", "glfw"}
        
    configuration "Debug"
        targetsuffix "D"
        defines "_DEBUG"
        flags {"Symbols"}
        buildoptions {"-std=c++17 -Wall"}

    configuration "Release"
        defines "NDEBUG"
        flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"}
        buildoptions {"-std=c++17"}

project "TAG"
    kind "WindowedApp"
    language "c++"
    includedirs {"GLDR/src", "include"}
    files {
        "src/**.cpp"
    }
    
    libdirs { "/usr/lib/x86_64-linux-gnu", "/usr/local/lib" }
    UseLibs {"glload", "glimage", "glutil", "glmesh", "glm", "freeglut", "boost"}

    configuration "windows"
        defines "WIN32"
        links {"glu32", "opengl32", "gdi32", "winmm", "user32"}
        
    configuration "linux"
        links {"GL", "GLU", "X11", "Xinerama", "Xi", "Xxf86vm", "Xcursor", "Xrandr", "pthread", "glfw"}
        
    configuration "Debug"
        targetsuffix "D"
        defines "_DEBUG"
        flags {"Symbols"}
        buildoptions {"-std=c++17 -Wall"}

    configuration "Release"
        defines "NDEBUG"
        flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"}
        buildoptions {"-std=c++17"}


