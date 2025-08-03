/////////////
// H_FILES

#include "platform.h"

#if PLATFORM_WINDOWS
    #include <windows.h>
    #include "win32/platform_win32.h"
#else
    #error Platform not supported.
#endif

/////////////
// C_FILES

#include "platform.c"

#if PLATFORM_WINDOWS
    #include "win32/platform_win32.c"
#else
    #error Platform not supported.
#endif

