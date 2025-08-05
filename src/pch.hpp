#pragma once

// -------------------------------------------
// System
// -------------------------------------------

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include <type_traits>

// -------------------------------------------
// Vendor
// -------------------------------------------

#include "vendor/stb_image.h"
#include "vendor/stb_image_write.h"
#include "vendor/stb_rect_pack.h"

// TODO: Implement my own version of this in 'filesystem'
#include "vendor/dirent.h"

#include "vendor/glad/gl.h"
#include "vendor/glad/wgl.h"

// -------------------------------------------
// GEM
// -------------------------------------------

#include "GEM/base/keycodes.hpp"
#include "GEM/base/macros.hpp"
#include "GEM/base/types.hpp"
