dofile("glsdk_0_5_2/links.lua")

solution "TAG"
    configurations {"Debug", "Release"}
    defines {"_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS"}
    targetdir "bin"
    
project "TAG_Test"
    kind "ConsoleApp"
    language "c++"
    includedirs {"GLDR/src", "catch", "src"}
    files {
        "test.cpp"
    }
    
    UseLibs {"glload", "glimage", "glutil", "glmesh", "glm", "freeglut", "glfw", "boost"}

    configuration "windows"
        defines "WIN32"
        links {"glu32", "opengl32", "gdi32", "winmm", "user32"}
        
    configuration "linux"
        links {"GL", "GLU", "X11", "Xrandr", "pthread"}
        
    configuration "Debug"
        targetsuffix "D"
        defines "_DEBUG"
        flags {"Symbols"}
        buildoptions {"-std=c++11 -Wall"}

    configuration "Release"
        defines "NDEBUG"
        flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"}
        buildoptions {"-std=c++11"};

project "TAG"
    kind "ConsoleApp"
    language "c++"
    includedirs {"GLDR/src"}
    files {
        "src/**.cpp",
        "src/**.h"
    }
    
    UseLibs {"glload", "glimage", "glutil", "glmesh", "glm", "freeglut", "glfw", "boost"}

    configuration "windows"
        defines "WIN32"
        links {"glu32", "opengl32", "gdi32", "winmm", "user32"}
        
    configuration "linux"
        links {"GL", "GLU", "X11", "Xrandr", "pthread"}
        
    configuration "Debug"
        targetsuffix "D"
        defines "_DEBUG"
        flags {"Symbols"}
        buildoptions {"-std=c++11 -Wall"}

    configuration "Release"
        defines "NDEBUG"
        flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"}
        buildoptions {"-std=c++11"};

