/*

	npack - General-Purpose File Packing Library
	Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)

	configuration of npack

	See README for copyright and license information.

*/

#ifndef _NPACK_DEF_H_
#define _NPACK_DEF_H_

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
    #define NPACK_PLATFORM_WINDOWS
    #define NPACK_PLATFORM_NAME "Windows"
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define NPACK_PLATFORM_LINUX
    #define NPACK_PLATFORM_NAME "Linux"
#elif defined(__FreeBSD__)
    #define NPACK_PLATFORM_BSD
    #define NPACK_PLATFORM_NAME "BSD"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #define NPACK_PLATFORM_MACOS
    #define NPACK_PLATFORM_NAME "Mac OS"
#else
    // TODO: So, what is your platform?
#endif

#ifdef NPACK_PLATFORM_WINDOWS
	#ifdef NPACK_DLL
		#ifdef NPACK_EXPORTS
			#define NPACK_API __declspec(dllexport)
			#ifdef NPACK_DEV
				#define NPACK_DEV_API __declspec(dllexport)
			#else
				#define NPACK_DEV_API
			#endif
		#else
			#define NPACK_API __declspec(dllimport)
			#ifdef NPACK_DEV
				#define NPACK_DEV_API __declspec(dllimport)
			#else
				#define NPACK_DEV_API
			#endif

			#ifndef _DEBUG
				#pragma comment(lib, "libnpack.lib")
			#else
				#pragma comment(lib, "libnpackd.lib")
			#endif
		#endif
	#else
		#define NPACK_API
		#define NPACK_DEV_API
	#endif /* NPACK_DLL */
#else /* other platforms */
	#define NPACK_API
	#define NPACK_DEV_API
#endif /* NPACK_PLATFORM_WINDOWS */

#endif /* _NPACK_DEF_H_ */
