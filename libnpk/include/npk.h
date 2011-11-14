/*

    npk - General-Purpose File Packing Library
    See README for copyright and license information.

*/

#ifndef _NPK_H_
#define _NPK_H_

#include "npk_conf.h"
#include "npk_base.h"
#include "npk_error.h"


#ifdef __cplusplus
extern "C" {
#endif

extern NPK_API int g_npkError;

NPK_API NPK_PACKAGE npk_package_open            ( NPK_CSTR filename, NPK_TEAKEY teakey[4] );
NPK_API NPK_PACKAGE npk_package_open_with_fd    ( NPK_CSTR name, int fd, long offset, long size, NPK_TEAKEY teakey[4] );
NPK_API bool        npk_package_close           ( NPK_PACKAGE package );
NPK_API NPK_ENTITY  npk_package_get_entity      ( NPK_PACKAGE package, NPK_CSTR entityname );
NPK_API NPK_ENTITY  npk_package_get_first_entity( NPK_PACKAGE package );
NPK_API NPK_CSTR    npk_entity_get_name         ( NPK_ENTITY entity );
NPK_API NPK_SIZE    npk_entity_get_size         ( NPK_ENTITY entity );
NPK_API NPK_SIZE    npk_entity_get_packed_size  ( NPK_ENTITY entity );
NPK_API NPK_SIZE    npk_entity_get_offset       ( NPK_ENTITY entity );
NPK_API bool        npk_entity_is_ready         ( NPK_ENTITY entity );
NPK_API NPK_ENTITY  npk_entity_next             ( NPK_ENTITY entity );
NPK_API bool        npk_entity_read             ( NPK_ENTITY entity, void* buf );
NPK_API bool        npk_entity_read_partial     ( NPK_ENTITY entity, void* buf, NPK_SIZE offset, NPK_SIZE size );
NPK_API void        npk_enable_callback         ( NPK_CALLBACK cb, NPK_SIZE cb_size );
NPK_API void        npk_disable_callback        ();
NPK_API NPK_STR     npk_error_to_str            ( NPK_RESULT res );

#ifdef NPK_PLATFORM_WINDOWS
NPK_API void        npk_enable_criticalsection  ();
NPK_API void        npk_disable_criticalsection ();
#endif

#ifdef __cplusplus
}
#endif

#endif /* _NPK_H_ */

