#include <stdint.h>
#include <string.h>
#include <math.h>

#if RHI_VULKAN
    #if PLATFORM_WINDOWS
        #define VK_USE_PLATFORM_WIN32_KHR
    #endif
    #include "vulkan/vulkan.h"
#endif

/////////////
// H_FILES

#include "engine_defs.h"

#include "platform/platform.h"
#if PLATFORM_WINDOWS
    #include <windows.h>
    #include "platform/win32/platform_win32.h"
#else
    #error Platform not supported.
#endif

#include "math/math_core.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"
#include "memory/arena.h"
#include "input/input.h"

#include "renderer/renderer.h"
#if RHI_VULKAN
    #include "renderer/vulkan/renderer_vulkan.h"
#else
    #error RHI not supported
#endif

/////////////
// C_FILES

#include "platform/platform.c"
#if PLATFORM_WINDOWS
    #include "platform/win32/platform_win32.c"
#else
    #error Platform not supported.
#endif

#include "math/math_core.c"
#include "math/vector.c"
#include "math/matrix.c"
#include "math/quaternion.c"
#include "memory/arena.c"
#include "input/input.c"

#include "renderer/renderer.c"
#if RHI_VULKAN
    #include "renderer/vulkan/renderer_vulkan.c"
#else
    #error RHI not supported
#endif

