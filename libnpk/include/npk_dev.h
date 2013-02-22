/*

    npk - neat package system
    See README for copyright and license information.

    header for npk development mode

*/

#ifndef _NPK_DEV_H_
#define _NPK_DEV_H_

#include "npk_conf.h"
#include "npk_base.h"
#include "npk_error.h"
#ifdef NPK_PLATFORM_WINDOWS
#include <windows.h>
#endif

#define NPK_SAFE_FREE(x) if(x) { free(x); x = NULL; }

#pragma pack(push, 4)
/* Structures */
typedef struct NPK_DEV_API NPK_tagENTITYINFO
{
    NPK_SIZE            offset_;            /* Data position. */
    NPK_SIZE            size_;              /* Data size. */
    NPK_SIZE            originalSize_;      /* Original size. */
    NPK_FLAG            flag_;              /* Composite of EntityFlag. ( see npk_base.h ) */
    NPK_TIME            modified_;          /* Last modified date of entity - 32bit time_t */
    NPK_BYTE            reserved_[4];       /* Reserved for 64bit-time_t */
    NPK_NAMESIZE        nameLength_;        /* Length of Foldername + FileName. */
} NPK_ENTITYINFO, *NPK_LPENTITYINFO;

typedef struct NPK_DEV_API NPK_tagENTITYINFO_V21
{
    NPK_SIZE            offset_;            /* Data position. */
    NPK_SIZE            size_;              /* Data size. */
    NPK_SIZE            originalSize_;      /* Original size. */
    NPK_FLAG            flag_;              /* Composite of EntityFlag. ( see npk_base.h ) */
    NPK_64BIT           modified_;          /* For old-npk-style 64bit FILETIME */
    NPK_NAMESIZE        nameLength_;        /* Length of Foldername + FileName. */
} NPK_ENTITYINFO_V21, *NPK_LPENTITYINFO_V21;

typedef struct NPK_DEV_API NPK_tagENTITYBODY
{
    NPK_ENTITYINFO      info_;
    NPK_FLAG            newflag_;           /* New entity flags */
    NPK_STR             name_;              /* Foldername + FileName. */
    NPK_STR             localname_;         /* Local Foldername + FileName. */
    NPK_PACKAGE         owner_;

    struct NPK_tagENTITYBODY*   prev_;
    struct NPK_tagENTITYBODY*   next_;
    struct NPK_tagENTITYBODY*   prevInBucket_;
    struct NPK_tagENTITYBODY*   nextInBucket_;
} NPK_ENTITYBODY, *NPK_LPENTITYBODY;

typedef struct NPK_DEV_API NPK_tagPACKAGEINFO
{
    NPK_CHAR            signature_[4];      /* NPAK */
    int                 version_;           /* Package version. */
    NPK_SIZE            entityCount_;       /* Quantity of entities. */
    NPK_SIZE            entityInfoOffset_;  /* Entities' information offset. */
    NPK_SIZE            entityDataOffset_;  /* Entities' data offset. */
} NPK_PACKAGEINFO, *NPK_LPPACKAGEINFO;

typedef struct NPK_DEV_API NPK_tagPACKAGEINFO_V23
{
    NPK_TIME            modified_;          /* Last modified date of package - 32bit time_t */
} NPK_PACKAGEINFO_V23, *NPK_LPPACKAGEINFO_V23;

typedef struct NPK_DEV_API NPK_tagBUCKET
{
    NPK_LPENTITYBODY    pEntityHead_;
    NPK_LPENTITYBODY    pEntityTail_;
} NPK_BUCKET, *NPK_LPBUCKET;

typedef struct NPK_DEV_API NPK_tagPACKAGEBODY
{
    NPK_PACKAGEINFO     info_;
    NPK_TIME            modified_;
    NPK_HANDLE          handle_;            /* File handle. */
    NPK_TEAKEY          teakey_[4];

    NPK_LPENTITYBODY    pEntityHead_;
    NPK_LPENTITYBODY    pEntityTail_;
    NPK_LPENTITYBODY    pEntityLatest_;
#ifdef _WIN32
    CRITICAL_SECTION    cs_;
#endif
    NPK_LPBUCKET        bucket_[NPK_HASH_BUCKETS];
    bool                usingHashmap_;
    bool                usingFdopen_;
    long                offsetJump_;
} NPK_PACKAGEBODY, *NPK_LPPACKAGEBODY;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

extern NPK_DEV_API NPK_CALLBACK     g_callbackfp;
extern NPK_DEV_API NPK_SIZE         g_callbackSize;

/* Helper Functions */
NPK_DEV_API void        npk_log( NPK_CSTR format, ... );
NPK_DEV_API NPK_RESULT  npk_error( NPK_RESULT res );
NPK_DEV_API NPK_RESULT  npk_alloc_copy_string( NPK_STR* dst, NPK_CSTR src );
NPK_DEV_API NPK_RESULT  npk_get_filetime( NPK_CSTR filename, NPK_TIME* pft );
NPK_DEV_API NPK_RESULT  npk_set_filetime( NPK_CSTR filename, const NPK_TIME pft );
NPK_DEV_API void        npk_filetime_to_unixtime( NPK_64BIT* pft, NPK_TIME* pt );
NPK_DEV_API void        npk_enable_gluetime( NPK_TIME time );
NPK_DEV_API void        npk_disable_gluetime();
NPK_DEV_API NPK_HASHKEY npk_get_bucket( NPK_CSTR name );
NPK_DEV_API NPK_RESULT  npk_prepare_entityname( NPK_CSTR src, NPK_STR dst, size_t dstLen );

/* File I/O Functions */
NPK_DEV_API NPK_RESULT  npk_open( NPK_HANDLE* handle, NPK_CSTR fileName, bool createfile, bool bcheckexist );
NPK_DEV_API NPK_RESULT  npk_flush( NPK_HANDLE handle );
NPK_DEV_API NPK_RESULT  npk_close( NPK_HANDLE handle );

NPK_DEV_API long        npk_seek( NPK_HANDLE handle, long offset, int origin );
NPK_DEV_API long        npk_tell( NPK_HANDLE handle );
NPK_DEV_API NPK_RESULT  npk_read( NPK_HANDLE handle, void* buf, NPK_SIZE size,
                                NPK_CALLBACK cb, int cbprocesstype, NPK_SIZE cbsize, NPK_CSTR cbidentifier );
NPK_DEV_API NPK_RESULT  npk_write( NPK_HANDLE handle, const void* buf, NPK_SIZE size,
                                NPK_CALLBACK cb, int cbprocesstype, NPK_SIZE cbsize, NPK_CSTR cbidentifier );
NPK_DEV_API NPK_RESULT  npk_read_encrypt( NPK_TEAKEY* key, NPK_HANDLE handle, void* buf, NPK_SIZE size,
                                NPK_CALLBACK cb, int cbprocesstype, NPK_SIZE cbsize, NPK_CSTR cbidentifier,
                                bool cipherRemains, bool useXXTEA );
NPK_DEV_API NPK_RESULT  npk_write_encrypt( NPK_TEAKEY* key, NPK_HANDLE handle, const void* buf, NPK_SIZE size,
                                NPK_CALLBACK cb, int cbprocesstype, NPK_SIZE cbsize, NPK_CSTR cbidentifier,
                                bool cipherRemains, bool useXXTEA );

/* Entity Functions */
NPK_DEV_API NPK_RESULT  npk_entity_alloc( NPK_ENTITY* lpEntity );
NPK_DEV_API NPK_RESULT  npk_entity_init( NPK_ENTITY entity );
NPK_DEV_API NPK_RESULT  npk_entity_get_current_flag( NPK_ENTITY entity, NPK_FLAG* flag );
NPK_DEV_API NPK_RESULT  npk_entity_get_new_flag( NPK_ENTITY entity, NPK_FLAG* flag );
NPK_DEV_API NPK_RESULT  npk_entity_set_flag( NPK_ENTITY entity, NPK_FLAG flag );
NPK_DEV_API NPK_RESULT  npk_entity_add_flag( NPK_ENTITY entity, NPK_FLAG flag );
NPK_DEV_API NPK_RESULT  npk_entity_sub_flag( NPK_ENTITY entity, NPK_FLAG flag );
NPK_DEV_API NPK_RESULT  npk_entity_write( NPK_ENTITY entity, NPK_HANDLE handle, bool forceProcessing );
NPK_DEV_API NPK_RESULT  npk_entity_export( NPK_ENTITY entity, NPK_CSTR filename, bool forceOverwrite );

/* Package Functions */
NPK_DEV_API NPK_RESULT  npk_package_alloc( NPK_PACKAGE* lpPackage, NPK_TEAKEY teakey[4] );
NPK_DEV_API NPK_RESULT  npk_package_init( NPK_PACKAGE package );
NPK_DEV_API NPK_RESULT  npk_package_save( NPK_PACKAGE package, NPK_CSTR filename, bool forceOverwrite );
NPK_DEV_API NPK_RESULT  npk_package_clear( NPK_PACKAGE package );
NPK_DEV_API NPK_RESULT  npk_package_add_file( NPK_PACKAGE package, NPK_CSTR filename, NPK_CSTR entityname, NPK_ENTITY* lpEntity );
NPK_DEV_API NPK_RESULT  npk_package_add_entity( NPK_PACKAGE package, NPK_ENTITY entity );
NPK_DEV_API NPK_RESULT  npk_package_remove_entity( NPK_PACKAGE package, NPK_ENTITY entity );
NPK_DEV_API NPK_RESULT  npk_package_detach_entity( NPK_PACKAGE package, NPK_ENTITY entity );
NPK_DEV_API NPK_RESULT  npk_package_remove_all_entity( NPK_PACKAGE package );
NPK_DEV_API NPK_RESULT  npk_package_detach_all_entity( NPK_PACKAGE package );
NPK_DEV_API void        npk_package_lock( NPK_PACKAGE package );
NPK_DEV_API void        npk_package_free( NPK_PACKAGE package );

/* For FFI */
NPK_DEV_API NPK_ENTITY  _npk_entity_alloc();
NPK_DEV_API NPK_ENTITY  _npk_package_add_file( NPK_PACKAGE package, NPK_CSTR filename, NPK_CSTR entityname );
NPK_DEV_API NPK_PACKAGE _npk_package_alloc( NPK_TEAKEY teakey[4] );


/* Custom file handler */
extern npk_open_func   __open;
extern npk_close_func  __close;
extern npk_read_func   __read;
extern npk_write_func  __write;
extern npk_seek_func   __seek;
extern npk_tell_func   __tell;
extern npk_rewind_func __rewind;
extern npk_commit_func __commit;

extern bool __use_open;
extern bool __use_close;
extern bool __use_read;
extern bool __use_write;
extern bool __use_seek;
extern bool __use_tell;
extern bool __use_rewind;
extern bool __use_commit;

#ifdef __cplusplus
}
#endif

#endif /* _NPK_DEV_H_ */

