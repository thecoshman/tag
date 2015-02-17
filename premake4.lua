dofile("glsdk_0_5_2/links.lua")

solution "TAG"
    configurations {"Debug", "Release"}
    defines {"_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS"}
    location "build"
    targetdir "bin" 

project "TAG"
    kind "ConsoleApp"
    language "c++"
    files {"*.cpp", "*.h"}
    
    UseLibs {"glload", "glimage", "glutil", "glmesh", "glm", "freeglut", "glfw", "boost"}
    
    configuration "windows"
        defines "WIN32"
        links {"glu32", "opengl32", "gdi32", "winmm", "user32"}
        
    configuration "linux"
        links {"GL", "GLU", "X11", "Xrandr", "pthread"}
        
    configuration "Debug"
        targetsuffix "D"
        defines "_DEBUG"
        flags "Symbols"

    configuration "Release"
        defines "NDEBUG"
        flags {"OptimizeSpeed", "NoFramePointer", "ExtraWarnings", "NoEditAndContinue"};