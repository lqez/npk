/*

	npack - General-Purpose File Packing Library
	Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)

	See README for copyright and license information.

*/

#ifndef _NPACK_H_
#define _NPACK_H_

#include "npack_conf.h"
#include "npack_base.h"
#include "npack_error.h"


#ifdef __cplusplus
extern "C" {
#endif

extern NPACK_API int g_npackError;

NPACK_API NPACK_PACKAGE	npack_package_open				( NPACK_CSTR filename, NPACK_TEAKEY* teakey );
NPACK_API bool			npack_package_close				( NPACK_PACKAGE package );
NPACK_API NPACK_ENTITY	npack_package_get_entity		( NPACK_PACKAGE package, NPACK_CSTR entityname );
NPACK_API NPACK_ENTITY	npack_package_get_first_entity	( NPACK_PACKAGE package );
NPACK_API NPACK_CSTR	npack_entity_get_name			( NPACK_ENTITY entity );
NPACK_API NPACK_SIZE	npack_entity_get_size			( NPACK_ENTITY entity );
NPACK_API NPACK_ENTITY	npack_entity_next				( NPACK_ENTITY entity );
NPACK_API bool			npack_entity_read				( NPACK_ENTITY entity, void* buf );
NPACK_API bool			npack_entity_read_partial		( NPACK_ENTITY entity, void* buf, NPACK_SIZE offset, NPACK_SIZE size );
NPACK_API void			npack_enable_callback			( NPACK_CALLBACK cb, NPACK_SIZE cb_size );
NPACK_API void			npack_disable_callback			();
NPACK_API NPACK_STR		npack_error_to_str				( NPACK_RESULT res );

#ifdef NPACK_PLATFORM_WINDOWS
NPACK_API void			npack_enable_criticalsection	();
NPACK_API void			npack_disable_criticalsection	();
#endif

#ifdef __cplusplus
}
#endif

#endif /* _NPACK_H_ */

