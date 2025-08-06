@echo off
setlocal enabledelayedexpansion
cd /D "%~dp0"

:: --- Check targets
if "%1"=="game"  set TARGET_CHOSEN=1
if "%1"=="forge" set TARGET_CHOSEN=1
if "%1"=="clean" set TARGET_CHOSEN=1

if not defined TARGET_CHOSEN (
    echo WARN: no build target selected: use one of the following "game", "forge" or "clean".
    exit /B 1
)

:: --- Remove build directories
if "%1"=="clean" (
    echo [ Target: clean ]
    echo Removing build directories...
    if exist .cache rmdir .cache /S /Q
    if exist bin rmdir bin /S /Q
    if exist build rmdir build /S /Q

    echo Removing generated files...
    if exist src\GEM (
        pushd src\GEM
        if exist assets_generated.cpp del assets_generated.cpp
        if exist assets_generated.hpp del assets_generated.hpp
        popd
    )

    exit /B 0
)

:: --- Handle arguments
set HAS_ARGUMENT=

set BUILD_TYPE=Debug
if "%2"=="release" (
    set BUILD_TYPE=Release
    set HAS_ARGUMENT=1
) else (
    if "%3"=="release" (
        set BUILD_TYPE=Release
        set HAS_ARGUMENT=1
    )
)

set DO_FULL=
if "%2"=="full" (
    set DO_FULL=1
    set HAS_ARGUMENT=1
) else (
    if "%3"=="full" (
        set DO_FULL=1
        set HAS_ARGUMENT=1
    )
)

if not defined HAS_ARGUMENT (
    if not "%2"=="" (
        echo WARN: incorrect argument provided: use one/both of the following "release" or "full".
        exit /B 1
    )
)

:: --- Define common variables
set BUILD_DIR=build\!BUILD_TYPE!
set OUTPUT_DIR=bin\!BUILD_TYPE!

set CMAKE_ARGS=-G "Ninja" -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=%~dp0\!OUTPUT_DIR! -DCMAKE_BUILD_TYPE=!BUILD_TYPE!
set START_ARGS=/WAIT /B /D !OUTPUT_DIR!
set XCOPY_ARGS=/S /E /H /I /Y

:: Asset Forge
set FORGE_SRC_PATH=src\forge
set FORGE_BUILD_PATH=!BUILD_DIR!\forge
set FORGE_EXE_PATH=!OUTPUT_DIR!\forge.exe
set FORGE_REBUILD=
if defined DO_FULL (
    set FORGE_REBUILD=1
) else (
    if not exist !FORGE_BUILD_PATH! (
        set FORGE_REBUILD=1
    )
)

:: Application Launcher
set LAUNCHER_SRC_PATH=src\launcher
set LAUNCHER_BUILD_PATH=!BUILD_DIR!\launcher
set LAUNCHER_EXE_PATH=!OUTPUT_DIR!\launcher.exe
set LAUNCHER_REBUILD=1

:: --- Create directories
if not exist !OUTPUT_DIR! mkdir !OUTPUT_DIR!

pushd !OUTPUT_DIR!
if not exist temp mkdir temp
if not exist resources (
    mkdir resources\atlas
    mkdir resources\font
    mkdir resources\music
    mkdir resources\sound
)
popd

:: -- Run targets
if "%1"=="forge" (
    echo [ Target: forge ]
    if defined FORGE_REBUILD (
        set FORGE_FLAGS=--force
        cmake -S %FORGE_SRC_PATH% -B !FORGE_BUILD_PATH! !CMAKE_ARGS!
    )

    ninja -C !FORGE_BUILD_PATH!

    if exist !FORGE_EXE_PATH! (
        echo.
        echo [ Running: !FORGE_EXE_PATH! ]
        start !START_ARGS! !FORGE_EXE_PATH! !FORGE_FLAGS!

        echo -------------------------------------------

        REM If asset files were changed, copy everything over to resources
        if ERRORLEVEL 2 (
            echo Copied Assets: generated
            xcopy assets\font\*.fnt  !OUTPUT_DIR!\resources\font  %XCOPY_ARGS% > nul 2>&1
            xcopy assets\music\*.ogg !OUTPUT_DIR!\resources\music %XCOPY_ARGS% > nul 2>&1
            xcopy assets\sound\*.wav !OUTPUT_DIR!\resources\sound %XCOPY_ARGS% > nul 2>&1
        )

        echo Copied Assets: manual
        xcopy assets\shader !OUTPUT_DIR!\resources\shader %XCOPY_ARGS% > nul 2>&1
    ) else (
        echo WARN: file '!FORGE_EXE_PATH!' was not found!
    )
)

if "%1"=="game" (
    echo [ Target: game]
    cmake -S %LAUNCHER_SRC_PATH% -B !LAUNCHER_BUILD_PATH! !CMAKE_ARGS!
    ninja -C !LAUNCHER_BUILD_PATH!

    if exist !LAUNCHER_EXE_PATH! (
        echo.
        echo [ Running: !LAUNCHER_EXE_PATH! ]
        start %START_ARGS% !LAUNCHER_EXE_PATH!
    ) else (
        echo WARN: file '!LAUNCHER_EXE_PATH!' was not found!
    )
)
