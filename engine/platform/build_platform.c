#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
    #include "windows/platform_windows.c"
#else
    #error Platform not supported.
#endif
