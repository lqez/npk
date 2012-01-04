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

#ifdef NPK_PREFIX
    #define CONCAT_X(t1,t2) t1##t2
    #define CONCAT(t1,t2) CONCAT_X(t1,t2)

    #define g_npkError                      CONCAT(NPK_PREFIX_NAME,g_npkError)
    #define g_callbackfp                    CONCAT(NPK_PREFIX_NAME,g_callbackfp)
    #define g_callbackSize                  CONCAT(NPK_PREFIX_NAME,g_callbackSize)

    #define npk_package_open                CONCAT(NPK_PREFIX_NAME,npk_package_open)
    #define npk_package_open_with_fd        CONCAT(NPK_PREFIX_NAME,npk_package_open_with_fd)
    #define npk_package_close               CONCAT(NPK_PREFIX_NAME,npk_package_close)
    #define npk_package_get_entity          CONCAT(NPK_PREFIX_NAME,npk_package_get_entity)
    #define npk_package_get_first_entity    CONCAT(NPK_PREFIX_NAME,npk_package_get_first_entity)
    #define npk_entity_get_name             CONCAT(NPK_PREFIX_NAME,npk_entity_get_name)
    #define npk_entity_get_size             CONCAT(NPK_PREFIX_NAME,npk_entity_get_size)
    #define npk_entity_get_packed_size      CONCAT(NPK_PREFIX_NAME,npk_entity_get_packed_size)
    #define npk_entity_get_offset           CONCAT(NPK_PREFIX_NAME,npk_entity_get_offset)
    #define npk_entity_is_ready             CONCAT(NPK_PREFIX_NAME,npk_entity_is_ready)
    #define npk_entity_next                 CONCAT(NPK_PREFIX_NAME,npk_entity_next)
    #define npk_entity_read                 CONCAT(NPK_PREFIX_NAME,npk_entity_read)
    #define npk_entity_read_partial         CONCAT(NPK_PREFIX_NAME,npk_entity_read_partial)
    #define npk_enable_callback             CONCAT(NPK_PREFIX_NAME,npk_enable_callback)
    #define npk_disable_callback            CONCAT(NPK_PREFIX_NAME,npk_disable_callback)
    #define npk_error_to_str                CONCAT(NPK_PREFIX_NAME,npk_error_to_str)
    #define npk_enable_criticalsection      CONCAT(NPK_PREFIX_NAME,npk_enable_criticalsection)
    #define npk_disable_criticalsection     CONCAT(NPK_PREFIX_NAME,npk_disable_criticalsection)

    #define npk_log                         CONCAT(NPK_PREFIX_NAME,npk_log)
    #define npk_error                       CONCAT(NPK_PREFIX_NAME,npk_error)
    #define npk_alloc_copy_string           CONCAT(NPK_PREFIX_NAME,npk_alloc_copy_string)
    #define npk_get_filetime                CONCAT(NPK_PREFIX_NAME,npk_get_filetime)
    #define npk_set_filetime                CONCAT(NPK_PREFIX_NAME,npk_set_filetime)
    #define npk_filetime_to_unixtime        CONCAT(NPK_PREFIX_NAME,npk_filetime_to_unixtime)
    #define npk_enable_gluetime             CONCAT(NPK_PREFIX_NAME,npk_enable_gluetime)
    #define npk_disable_gluetime            CONCAT(NPK_PREFIX_NAME,npk_disable_gluetime)
    #define npk_get_bucket                  CONCAT(NPK_PREFIX_NAME,npk_get_bucket)
    #define npk_prepare_entityname          CONCAT(NPK_PREFIX_NAME,npk_prepare_entityname)

    #define npk_open                        CONCAT(NPK_PREFIX_NAME,npk_open)
    #define npk_flush                       CONCAT(NPK_PREFIX_NAME,npk_flush)
    #define npk_close                       CONCAT(NPK_PREFIX_NAME,npk_close)

    #define npk_seek                        CONCAT(NPK_PREFIX_NAME,npk_seek)
    #define npk_tell                        CONCAT(NPK_PREFIX_NAME,npk_tell)
    #define npk_read                        CONCAT(NPK_PREFIX_NAME,npk_read)
    #define npk_write                       CONCAT(NPK_PREFIX_NAME,npk_write)
    #define npk_read_encrypt                CONCAT(NPK_PREFIX_NAME,npk_read_encrypt)
    #define npk_write_encrypt               CONCAT(NPK_PREFIX_NAME,npk_write_encrypt)

    #define npk_entity_alloc                CONCAT(NPK_PREFIX_NAME,npk_entity_alloc)
    #define npk_entity_init                 CONCAT(NPK_PREFIX_NAME,npk_entity_init)
    #define npk_entity_get_current_flag     CONCAT(NPK_PREFIX_NAME,npk_entity_get_current_flag)
    #define npk_entity_get_new_flag         CONCAT(NPK_PREFIX_NAME,npk_entity_get_new_flag)
    #define npk_entity_set_flag             CONCAT(NPK_PREFIX_NAME,npk_entity_set_flag)
    #define npk_entity_add_flag             CONCAT(NPK_PREFIX_NAME,npk_entity_add_flag)
    #define npk_entity_sub_flag             CONCAT(NPK_PREFIX_NAME,npk_entity_sub_flag)
    #define npk_entity_write                CONCAT(NPK_PREFIX_NAME,npk_entity_write)
    #define npk_entity_export               CONCAT(NPK_PREFIX_NAME,npk_entity_export)

    #define npk_package_alloc               CONCAT(NPK_PREFIX_NAME,npk_package_alloc)
    #define npk_package_init                CONCAT(NPK_PREFIX_NAME,npk_package_init)
    #define npk_package_save                CONCAT(NPK_PREFIX_NAME,npk_package_save)
    #define npk_package_clear               CONCAT(NPK_PREFIX_NAME,npk_package_clear)
    #define npk_package_add_file            CONCAT(NPK_PREFIX_NAME,npk_package_add_file)
    #define npk_package_add_entity          CONCAT(NPK_PREFIX_NAME,npk_package_add_entity)
    #define npk_package_remove_entity       CONCAT(NPK_PREFIX_NAME,npk_package_remove_entity)
    #define npk_package_detach_entity       CONCAT(NPK_PREFIX_NAME,npk_package_detach_entity)
    #define npk_package_remove_all_entity   CONCAT(NPK_PREFIX_NAME,npk_package_remove_all_entity)
    #define npk_package_detach_all_entity   CONCAT(NPK_PREFIX_NAME,npk_package_detach_all_entity)
    #define __npk_package_add_entity        CONCAT(NPK_PREFIX_NAME,__npk_package_add_entity)
    #define __npk_package_open              CONCAT(NPK_PREFIX_NAME,__npk_package_open)
    #define __npk_package_remove_all_entity CONCAT(NPK_PREFIX_NAME,__npk_package_remove_all_entity)


#endif /* NPK_PREFIX */

#endif /* _NPK_DEF_H_ */
