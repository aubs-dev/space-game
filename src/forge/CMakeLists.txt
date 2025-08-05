cmake_minimum_required(VERSION 3.16)
include("../../cmake/toolchain.cmake")

# -- Source files
list(APPEND FORGE_FILES
    # [Forge]
    "../forge/forge.cpp"
    # [Engine]
    "../GEM/logger.cpp"
    "../GEM/core/filesystem.cpp"
    "../GEM/core/memory.cpp"
    # [Vendor]
    "../vendor/impl/stb.cpp"
)

# -- Executable
project(forge)
add_executable(forge ${FORGE_FILES})

target_precompile_headers(forge PRIVATE "../pch.hpp")
target_include_directories(forge PRIVATE "../")

target_compile_definitions(forge PRIVATE ${GEM_DEFINES})
target_link_options(forge PRIVATE LINKER:/SUBSYSTEM:console)
