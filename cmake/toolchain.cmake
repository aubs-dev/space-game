if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compilation Flags
set(BASE_FLAGS "-Wall -Wextra -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-unused-parameter -Wno-unused-function -Wno-missing-designated-field-initializers -Wno-microsoft-goto -fdiagnostics-absolute-paths -fdiagnostics-color=always -fansi-escape-codes")
set(BASE_FLAGS_DEBUG "-O0 -g")
set(BASE_FLAGS_RELEASE "-O2")

set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} ${BASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG} ${BASE_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} ${BASE_FLAGS_RELEASE}")

set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${BASE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${BASE_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BASE_FLAGS_RELEASE}")

# set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

# C/C++ Standards
set(CMAKE_C_STANDARD "11")
set(CMAKE_C_COMPILER "clang")
set(CMAKE_C_STANDARD_REQUIRED ON)
SET(CMAKE_C_STANDARD_LIBRARIES "")
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD "20")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_STANDARD_LIBRARIES "")
set(CMAKE_CXX_EXTENSIONS OFF)

# Defines
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND GEM_DEFINES "GEM_DEBUG")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND GEM_DEFINES "GEM_RELEASE")
endif()

list(APPEND GEM_DEFINES "_CRT_SECURE_NO_WARNINGS")
