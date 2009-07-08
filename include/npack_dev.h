/*

	npack - General-Purpose File Packing Library
	Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)

	header for npack development mode

	See README for copyright and license information.

*/

#ifndef _NPACK_DEV_H_
#define _NPACK_DEV_H_

#include "npack_conf.h"
#include "npack_base.h"
#include "npack_error.h"
#ifdef NPACK_PLATFORM_WINDOWS
#include <windows.h>
#endif

#define NPACK_SAFE_FREE(x) if(x) { free(x); x = NULL; }

#pragma pack(push, 4)

/* Structures */
typedef struct NPACK_DEV_API NPACK_tagENTITYINFO
{
	NPACK_SIZE			offset_;			/* Data position. */
	NPACK_SIZE			size_;				/* Data size. */
	NPACK_SIZE			originalSize_;		/* Original size. */
	NPACK_FLAG			flag_;				/* Composite of EntityFlag. ( see npack_base.h ) */
	NPACK_TIME			modified_;			/* Last modified date of entity - 32bit time_t */
	NPACK_BYTE			reserved_[4];		/* Reserved for 64bit-time_t */
 	NPACK_NAMESIZE		nameLength_;		/* Length of Foldername + FileName. */
} NPACK_ENTITYINFO, *NPACK_LPENTITYINFO;

typedef struct NPACK_DEV_API NPACK_tagENTITYINFO_V21
{
	NPACK_SIZE			offset_;			/* Data position. */
	NPACK_SIZE			size_;				/* Data size. */
	NPACK_SIZE			originalSize_;		/* Original size. */
	NPACK_FLAG			flag_;				/* Composite of EntityFlag. ( see npack_base.h ) */
	NPACK_64BIT			modified_;			/* For old-npack-style 64bit FILETIME */
 	NPACK_NAMESIZE		nameLength_;		/* Length of Foldername + FileName. */
} NPACK_ENTITYINFO_V21, *NPACK_LPENTITYINFO_V21;

typedef struct NPACK_DEV_API NPACK_tagENTITYBODY
{
	NPACK_ENTITYINFO	info_;
	NPACK_FLAG			newflag_;			/* New entity flags */
	NPACK_STR			name_;				/* Foldername + FileName. */
	NPACK_STR			localname_;			/* Local Foldername + FileName. */
	NPACK_PACKAGE		owner_;

	struct NPACK_tagENTITYBODY*	prev_;
	struct NPACK_tagENTITYBODY*	next_;
} NPACK_ENTITYBODY, *NPACK_LPENTITYBODY;

typedef struct NPACK_DEV_API NPACK_tagPACKAGEINFO
{
	NPACK_CHAR			signature_[4];		/* NPAK */
	int					version_;			/* Package version. */
	NPACK_SIZE			entityCount_;		/* Quantity of entities. */
	NPACK_SIZE			entityInfoOffset_;	/* Entities' information offset. */
	NPACK_SIZE			entityDataOffset_;	/* Entities' data offset. */
} NPACK_PACKAGEINFO, *NPACK_LPPACKAGEINFO;

typedef struct NPACK_DEV_API NPACK_tagPACKAGEINFO_V18
{
	NPACK_TEAKEY		teakey_[4];
} NPACK_PACKAGEINFO_V18, *NPACK_LPPACKAGEINFO_V18;

typedef struct NPACK_DEV_API NPACK_tagPACKAGEINFO_V23
{
	NPACK_TIME			modified_;			/* Last modified date of package - 32bit time_t */
} NPACK_PACKAGEINFO_V23, *NPACK_LPPACKAGEINFO_V23;

typedef struct NPACK_DEV_API NPACK_tagPACKAGEBODY
{
	NPACK_PACKAGEINFO	info_;
	NPACK_HANDLE		handle_;			/* File handle. */
	NPACK_TEAKEY		teakey_[4];

	NPACK_LPENTITYBODY	pEntityHead_;
	NPACK_LPENTITYBODY	pEntityTail_;
	NPACK_LPENTITYBODY	pEntityLatest_;
#ifdef WIN32
	CRITICAL_SECTION	cs_;
#endif
} NPACK_PACKAGEBODY, *NPACK_LPPACKAGEBODY;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

extern NPACK_DEV_API NPACK_CALLBACK		g_callbackfp;
extern NPACK_DEV_API NPACK_SIZE			g_callbackSize;

/* Helper Functions */
NPACK_DEV_API void			npack_log( NPACK_CSTR format, ... );
NPACK_DEV_API NPACK_RESULT	npack_error( NPACK_RESULT res );
NPACK_DEV_API NPACK_RESULT	npack_alloc_copy_string( NPACK_STR* dst, NPACK_CSTR src );
NPACK_DEV_API NPACK_RESULT	npack_get_filetime( NPACK_CSTR filename, NPACK_TIME* pft );
NPACK_DEV_API NPACK_RESULT	npack_set_filetime( NPACK_CSTR filename, const NPACK_TIME pft );
NPACK_DEV_API void			npack_win32filetime_to_timet( NPACK_64BIT* pft, NPACK_TIME* pt );
NPACK_DEV_API void			npack_enable_gluetime( NPACK_TIME time );
NPACK_DEV_API void			npack_disable_gluetime();

/* File I/O Functions */
NPACK_DEV_API NPACK_RESULT	npack_open( int* handle, NPACK_CSTR fileName, bool createfile, bool bcheckexist );
NPACK_DEV_API NPACK_RESULT	npack_flush( NPACK_HANDLE handle );
NPACK_DEV_API NPACK_RESULT	npack_close( NPACK_HANDLE handle );

NPACK_DEV_API long			npack_seek( NPACK_HANDLE handle, long offset, int origin );
NPACK_DEV_API long			npack_tell( NPACK_HANDLE handle );
NPACK_DEV_API NPACK_RESULT	npack_read( NPACK_HANDLE handle, void* buf, NPACK_SIZE size,
									NPACK_CALLBACK cb, int cbprocesstype, NPACK_SIZE cbsize, NPACK_CSTR cbidentifier );
NPACK_DEV_API NPACK_RESULT	npack_write( NPACK_HANDLE handle, const void* buf, NPACK_SIZE size,
									NPACK_CALLBACK cb, int cbprocesstype, NPACK_SIZE cbsize, NPACK_CSTR cbidentifier );
NPACK_DEV_API NPACK_RESULT	npack_read_encrypt( NPACK_TEAKEY* key, NPACK_HANDLE handle, void* buf, NPACK_SIZE size,
									NPACK_CALLBACK cb, int cbprocesstype, NPACK_SIZE cbsize, NPACK_CSTR cbidentifier );
NPACK_DEV_API NPACK_RESULT	npack_write_encrypt( NPACK_TEAKEY* key, NPACK_HANDLE handle, const void* buf, NPACK_SIZE size,
									NPACK_CALLBACK cb, int cbprocesstype, NPACK_SIZE cbsize, NPACK_CSTR cbidentifier );

/* Entity Functions */
NPACK_DEV_API NPACK_RESULT	npack_entity_alloc( NPACK_ENTITY* lpEntity );
NPACK_DEV_API NPACK_RESULT	npack_entity_init( NPACK_ENTITY entity );
NPACK_DEV_API NPACK_RESULT	npack_entity_set_flag( NPACK_ENTITY entity, NPACK_FLAG flag );
NPACK_DEV_API NPACK_RESULT	npack_entity_add_flag( NPACK_ENTITY entity, NPACK_FLAG flag );
NPACK_DEV_API NPACK_RESULT	npack_entity_sub_flag( NPACK_ENTITY entity, NPACK_FLAG flag );
NPACK_DEV_API NPACK_RESULT	npack_entity_write( NPACK_ENTITY entity, NPACK_HANDLE handle );
NPACK_DEV_API NPACK_RESULT	npack_entity_export( NPACK_ENTITY entity, NPACK_CSTR filename, bool forceoverwrite );

/* Package Functions */
NPACK_DEV_API NPACK_RESULT	npack_package_new( NPACK_PACKAGE* lpPackage, NPACK_TEAKEY* teakey );
NPACK_DEV_API NPACK_RESULT	npack_package_save( NPACK_PACKAGE package, NPACK_CSTR filename, bool forceoverwrite );
NPACK_DEV_API NPACK_RESULT	npack_package_init( NPACK_PACKAGE package );
NPACK_DEV_API NPACK_RESULT	npack_package_clear( NPACK_PACKAGE package );
NPACK_DEV_API NPACK_RESULT	npack_package_add_file( NPACK_PACKAGE package, NPACK_CSTR filename, NPACK_CSTR entityname, NPACK_ENTITY* lpEntity );
NPACK_DEV_API NPACK_RESULT	npack_package_add_entity( NPACK_PACKAGE package, NPACK_ENTITY entity );
NPACK_DEV_API NPACK_RESULT	npack_package_remove_entity( NPACK_PACKAGE package, NPACK_ENTITY entity );
NPACK_DEV_API NPACK_RESULT	npack_package_remove_all_entity( NPACK_PACKAGE package );

#ifdef __cplusplus
}
#endif

#endif /* _NPACK_DEV_H_ */

