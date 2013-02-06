/*

    npk - neat package system
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
extern NPK_API int g_useCriticalSection;

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
NPK_API void        npk_enable_criticalsection  ();
NPK_API void        npk_disable_criticalsection ();

// custom file I/O
typedef void*  (*npk_open_func)  (const char*, const char*); // filename, mode as per fopen (ie "r" "wb+" etc)
typedef int    (*npk_close_func) (void*);
typedef size_t (*npk_read_func)  (void*, size_t, size_t, void*);
typedef size_t (*npk_write_func) (const void*, size_t, size_t, void*);
typedef int    (*npk_seek_func)  (void*, long, int);
typedef long   (*npk_tell_func)  (void*);
typedef void   (*npk_rewind_func)(void*);
typedef void   (*npk_commit_func)(void*);

NPK_API void npk_io_open_func  (npk_open_func func);
NPK_API void npk_io_close_func (npk_close_func func);
NPK_API void npk_io_read_func  (npk_read_func func);
NPK_API void npk_io_write_func (npk_write_func func);
NPK_API void npk_io_seek_func  (npk_seek_func func);
NPK_API void npk_io_tell_func  (npk_tell_func func);
NPK_API void npk_io_rewind_func(npk_rewind_func func);
NPK_API void npk_io_commit_func(npk_commit_func func);

#ifdef __cplusplus
}
#endif

#endif /* _NPK_H_ */

