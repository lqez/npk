/*

    npk - General-Purpose File Packing Library
    See README for copyright and license information.

    configuration of npk

*/

#ifndef _NPK_DEF_H_
#define _NPK_DEF_H_

#ifndef __cplusplus
    #ifndef bool
    #define bool int
    #endif

    #ifndef true
    #define true 1
    #endif

    #ifndef false
    #define false 0
    #endif
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define NPK_PLATFORM_WINDOWS
    #define NPK_PLATFORM_NAME "Windows"
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define NPK_PLATFORM_LINUX
    #define NPK_PLATFORM_NAME "Linux"
#elif defined(__FreeBSD__)
    #define NPK_PLATFORM_BSD
    #define NPK_PLATFORM_NAME "BSD"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define NPK_PLATFORM_MACOS
    #define NPK_PLATFORM_NAME "Mac OS"
#else
    // TODO: So, what is your platform?
#endif

#ifdef NPK_PLATFORM_WINDOWS
    #ifdef NPK_DLL
        #ifdef NPK_EXPORTS
            #define NPK_API __declspec(dllexport)
            #ifdef NPK_DEV
                #define NPK_DEV_API __declspec(dllexport)
            #else
                #define NPK_DEV_API
            #endif
        #else
            #define NPK_API __declspec(dllimport)
            #ifdef NPK_DEV
                #define NPK_DEV_API __declspec(dllimport)
            #else
                #define NPK_DEV_API
            #endif

            #ifndef _DEBUG
                #pragma comment(lib, "libnpk.lib")
            #else
                #pragma comment(lib, "libnpkd.lib")
            #endif
        #endif
    #else
        #define NPK_API
        #define NPK_DEV_API
    #endif /* NPK_DLL */
#else /* other platforms */
    #define NPK_API
    #define NPK_DEV_API
#endif /* NPK_PLATFORM_WINDOWS */

#endif /* _NPK_DEF_H_ */
