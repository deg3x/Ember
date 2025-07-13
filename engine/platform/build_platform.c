#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
    #include "windows/platform_win32.c"
#else
    #error Platform not supported.
#endif
