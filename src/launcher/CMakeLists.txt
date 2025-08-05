cmake_minimum_required(VERSION 3.16)
include("../../cmake/toolchain.cmake")

# -- Source files
file(GLOB_RECURSE GEM_FILES "../GEM/*.cpp")
list(APPEND LAUNCHER_FILES
    # [Launcher]
    "../launcher/launcher.cpp"
    # [Engine]
    "${GEM_FILES}"
    # [Vendor]
    "../vendor/impl/stb.cpp"
    "../vendor/impl/glad/gl.cpp"
    "../vendor/impl/glad/wgl.cpp"
)

# -- Executable
project(launcher)
add_executable(launcher ${LAUNCHER_FILES})

target_precompile_headers(launcher PRIVATE "../pch.hpp")
target_include_directories(launcher PRIVATE "../")

target_compile_definitions(launcher PRIVATE ${GEM_DEFINES})
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(launcher PRIVATE LINKER:/SUBSYSTEM:console)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(launcher PRIVATE LINKER:/SUBSYSTEM:windows,/ENTRY:mainCRTStartup)
endif()

target_link_libraries(
    launcher
    "user32"
    "gdi32"
    "opengl32"
    )
