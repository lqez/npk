/*

    npk - neat package system
    See README for copyright and license information.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include "npk.h"
#include "npk_dev.h"
#ifdef NPK_PLATFORM_MACOS 
#include <strings.h>
#else
#include <string.h>
#endif
#include "../external/tea/tea.h"
#include "../external/xxtea/xxtea.h"
#include "../external/zlib/zlib.h"


NPK_API int     g_npkError = 0; /* this variable has no multi-thread safety */
NPK_API int     g_useCriticalSection = 0;

#ifdef NPK_PLATFORM_WINDOWS
    #pragma warning( disable : 4996 )
#else
    #define strnicmp strncasecmp
    #define stricmp strcasecmp
#endif

NPK_CALLBACK    g_callbackfp;
NPK_SIZE        g_callbackSize;

NPK_RESULT __npk_package_add_entity( NPK_PACKAGE package, NPK_ENTITY entity, bool check );
NPK_RESULT __npk_package_open( NPK_PACKAGEBODY* pb, const NPK_CHAR* filename, long filesize, NPK_TEAKEY teakey[4] )
{
    NPK_CHAR            buf[512];
    NPK_ENTITYBODY*     eb = NULL;
    NPK_ENTITYINFO_V21  oldinfo;
    NPK_SIZE            entityCount = 0;
    NPK_CHAR*           entityheaderbuf;
    NPK_CHAR*           pos;
    long                entityheadersize = 0;
    NPK_RESULT          res;

    if( filesize == 0 )
    {
        filesize = npk_seek( pb->handle_, 0, SEEK_END );
        npk_seek( pb->handle_, 0, SEEK_SET );
    }

    if( filesize < sizeof(NPK_PACKAGEINFO) )
        return( npk_error( NPK_ERROR_PackageIsNotReady ) );

    // Read common header
    res = npk_read( pb->handle_,
                    (void*)&pb->info_,
                    sizeof(NPK_PACKAGEINFO),
                    g_callbackfp,
                    NPK_PROCESSTYPE_PACKAGEHEADER,
                    g_callbackSize,
                    filename );
    if( res != NPK_SUCCESS ) return res;

    if( strncmp( pb->info_.signature_, NPK_SIGNATURE, 4 ) != 0 )
        if( strncmp( pb->info_.signature_, NPK_OLD_SIGNATURE, 4 ) != 0 )
            return( npk_error( NPK_ERROR_NotValidPackage ) );

    // version 18 / read own tea key
    if( pb->info_.version_ < NPK_VERSION_REFACTORING )
    {
        return ( npk_error( NPK_ERROR_NotSupportedVersion ) );
    }
    else
    {
        if( teakey == NULL )
        {
            return ( npk_error( NPK_ERROR_NeedSpecifiedTeaKey ) );
        }
        memcpy( pb->teakey_, teakey, sizeof(NPK_TEAKEY) * 4 );
    }

    // version 23 / package timestamp
    if( pb->info_.version_ >= NPK_VERSION_PACKAGETIMESTAMP )
    {
        res = npk_read( pb->handle_,
                        (void*)&pb->modified_,
                        sizeof(NPK_TIME),
                        g_callbackfp,
                        NPK_PROCESSTYPE_PACKAGEHEADER,
                        g_callbackSize,
                        filename );
        if( res != NPK_SUCCESS ) return res;
    }

    entityCount = pb->info_.entityCount_;
    pb->info_.entityCount_ = 0;

    if( pb->info_.version_ >= NPK_VERSION_SINGLEPACKHEADER )
    {
        if( pb->info_.version_ >= NPK_VERSION_STREAMABLE )
        {
            if( filesize < (long)pb->info_.entityDataOffset_ )
                return( npk_error( NPK_ERROR_PackageIsNotReady ) );
            entityheadersize = (long)pb->info_.entityDataOffset_ - (long)pb->info_.entityInfoOffset_;
        }
        else
        {
            entityheadersize = filesize - (long)pb->info_.entityInfoOffset_;
            npk_seek( pb->handle_, (long)pb->info_.entityInfoOffset_+pb->offsetJump_, SEEK_SET );
        }

        entityheaderbuf = malloc( entityheadersize );
        if( !entityheaderbuf )
            return( npk_error( NPK_ERROR_NotEnoughMemory ) );

        res = npk_read_encrypt( teakey,
                                pb->handle_,
                                (void*)entityheaderbuf,
                                entityheadersize,
                                g_callbackfp,
                                NPK_PROCESSTYPE_ENTITYHEADER,
                                g_callbackSize,
                                filename,
                                pb->info_.version_ >= NPK_VERSION_ENCRYPTREMAINS,
                                pb->info_.version_ >= NPK_VERSION_USEXXTEAONHEADER
                                );
        if( res != NPK_SUCCESS ) return res;

        pos = entityheaderbuf;
        
        while( entityCount > 0 )
        {
            --entityCount;

            res = npk_entity_alloc( (NPK_ENTITY*)&eb );
            if( res != NPK_SUCCESS )
                goto __npk_package_open_return_res_with_free;

            eb->owner_ = pb;
            memcpy( &eb->info_, pos, sizeof(NPK_ENTITYINFO) );
            pos += sizeof(NPK_ENTITYINFO);

            if( pb->info_.version_ < NPK_VERSION_STREAMABLE )
                if( eb->info_.offset_ >= pb->info_.entityInfoOffset_ )
                {
                    res = npk_error( NPK_ERROR_InvalidTeaKey );
                    goto __npk_package_open_return_res_with_free;
                }

            eb->newflag_ = eb->info_.flag_;
            eb->name_ = malloc( sizeof(NPK_CHAR)*(eb->info_.nameLength_+1) );
            if( !eb->name_ )
            {
                res = npk_error( NPK_ERROR_NotEnoughMemory );
                goto __npk_package_open_return_res_with_free;
            }
            eb->name_[eb->info_.nameLength_] = '\0';
            memcpy( eb->name_, pos, eb->info_.nameLength_ );
            pos += eb->info_.nameLength_;

            __npk_package_add_entity( pb, eb, false );
        }
        NPK_SAFE_FREE( entityheaderbuf );
    }
    else    // old style entity header
    {
        npk_seek( pb->handle_, (long)pb->info_.entityInfoOffset_+pb->offsetJump_, SEEK_SET );
        while( entityCount > 0 )
        {
            --entityCount;

            res = npk_entity_alloc( (NPK_ENTITY*)&eb );
            if( res != NPK_SUCCESS )
                goto __npk_package_open_return_res_with_free;

            eb->owner_ = pb;

            // read entity info
            if( pb->info_.version_ < NPK_VERSION_UNIXTIMESUPPORT )
            {
                res = npk_read_encrypt( teakey,
                                        pb->handle_,
                                        (void*)&oldinfo,
                                        sizeof(NPK_ENTITYINFO),
                                        g_callbackfp,
                                        NPK_PROCESSTYPE_ENTITYHEADER,
                                        g_callbackSize,
                                        filename,
                                        false,
                                        false );
                if( res != NPK_SUCCESS )
                    goto __npk_package_open_return_res_with_free;

                eb->info_.offset_ = oldinfo.offset_;
                eb->info_.size_ = oldinfo.size_;
                eb->info_.originalSize_ = oldinfo.originalSize_;
                eb->info_.flag_ = oldinfo.flag_;
                npk_filetime_to_unixtime( &oldinfo.modified_, &eb->info_.modified_ );
                eb->info_.nameLength_ = oldinfo.nameLength_;
            }
            else
            {
                res = npk_read_encrypt( teakey,
                                        pb->handle_,
                                        (void*)&eb->info_,
                                        sizeof(NPK_ENTITYINFO),
                                        g_callbackfp,
                                        NPK_PROCESSTYPE_ENTITYHEADER,
                                        g_callbackSize,
                                        filename,
                                        false,
                                        false );
                if( res != NPK_SUCCESS )
                    goto __npk_package_open_return_res_with_free;
            }

            if( eb->info_.offset_ >= pb->info_.entityInfoOffset_ )
            {
                res = npk_error( NPK_ERROR_InvalidTeaKey );
                goto __npk_package_open_return_res_with_free;
            }

            
            res = npk_read_encrypt( teakey,
                                    pb->handle_,
                                    (void*)buf,
                                    sizeof(char) * eb->info_.nameLength_,
                                    g_callbackfp,
                                    NPK_PROCESSTYPE_ENTITYHEADER,
                                    g_callbackSize,
                                    filename,
                                    false,
                                    false );
            if( res != NPK_SUCCESS )
                goto __npk_package_open_return_res_with_free;

            eb->newflag_ = eb->info_.flag_;

            // copy name into entity body
            buf[eb->info_.nameLength_] = '\0';
            res = npk_alloc_copy_string( &eb->name_, buf );
            if( res != NPK_SUCCESS )
                goto __npk_package_open_return_res_with_free;

            __npk_package_add_entity( pb, eb, false );
        }
    }
    return NPK_SUCCESS;

__npk_package_open_return_res_with_free:

    NPK_SAFE_FREE( eb );
    return res;
}

NPK_PACKAGE npk_package_open_with_fd( NPK_CSTR name, int fd, long offset, long size, NPK_TEAKEY teakey[4] )
{
    NPK_PACKAGEBODY*    pb = NULL;

    if( NPK_SUCCESS != npk_package_alloc( (NPK_PACKAGE*)&pb, teakey ))
        return NULL;

    pb->handle_ = fd;
    pb->usingFdopen_ = true;
    pb->offsetJump_ = offset;

    npk_seek( fd, offset, SEEK_CUR );
    
    if( NPK_SUCCESS != __npk_package_open( pb, name, size, teakey ))
        goto npk_package_open_return_null_with_free;

    return (NPK_PACKAGE*)pb;

npk_package_open_return_null_with_free:
    if( pb )
        npk_package_close( pb );

    return NULL;
}

NPK_PACKAGE npk_package_open( NPK_CSTR filename, NPK_TEAKEY teakey[4] )
{
    NPK_PACKAGEBODY*    pb = NULL;

    if( NPK_SUCCESS != npk_package_alloc( (NPK_PACKAGE*)&pb, teakey ) )
        return NULL;

    if( NPK_SUCCESS != npk_open( &pb->handle_, filename, false, false ) )
        goto npk_package_open_return_null_with_free;
    
    if( NPK_SUCCESS != __npk_package_open( pb, filename, 0, teakey ))
        goto npk_package_open_return_null_with_free;

    return (NPK_PACKAGE*)pb;

npk_package_open_return_null_with_free:
    if( pb )
        npk_package_close( pb );

    return NULL;
}

bool npk_package_close( NPK_PACKAGE package )
{
    NPK_PACKAGEBODY* pb = (NPK_PACKAGEBODY*)package;
    NPK_RESULT res;
    int i;

    if( !package )
    {
        npk_error( NPK_ERROR_PackageIsNull );
        return false;
    }

    res = npk_package_remove_all_entity( pb );
    if( NPK_SUCCESS != res )
        return res;

#ifdef NPK_PLATFORM_WINDOWS
    DeleteCriticalSection( &pb->cs_ );
#endif

    if( false == pb->usingFdopen_ )
        npk_close( pb->handle_ );

    for( i = 0; i < NPK_HASH_BUCKETS; ++i )
        NPK_SAFE_FREE( pb->bucket_[i] );

    NPK_SAFE_FREE( pb );
    return true;
}

NPK_ENTITY npk_package_get_entity( NPK_PACKAGE package, NPK_CSTR entityname )
{
    NPK_ENTITYBODY* eb = NULL;
    NPK_PACKAGEBODY* pb = package;
    NPK_BUCKET* bucket = NULL;
    NPK_CHAR buf[512];

    if( !package )
    {
        npk_error( NPK_ERROR_PackageIsNull );
        return NULL;
    }

    if( NPK_SUCCESS != npk_prepare_entityname( entityname, buf, 512 ) )
        return NULL;

    if( pb->usingHashmap_ )
    {
        bucket = pb->bucket_[npk_get_bucket(buf)];
        if( bucket != NULL )
        {
            eb = bucket->pEntityHead_;
            while( eb != NULL )
            {
#ifdef NPK_CASESENSITIVE
                if( strcmp( eb->name_, buf ) == 0 )
#else
                if( stricmp( eb->name_, buf ) == 0 )
#endif
                {
                    pb->pEntityLatest_ = eb;
                    return eb;
                }
                eb = eb->nextInBucket_;
            }
        }
    }
    else /* not usingHashmap_ */
    {
        eb = pb->pEntityHead_;
        while( eb != NULL )
        {
#ifdef NPK_CASESENSITIVE
            if( strcmp( eb->name_, buf ) == 0 )
#else
            if( stricmp( eb->name_, buf ) == 0 )
#endif
            {
                pb->pEntityLatest_ = eb;
                return eb;
            }
            eb = eb->next_;
        }

    }
    npk_error( NPK_ERROR_CannotFindEntity );
    return NULL;
}

NPK_SIZE npk_entity_get_size( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = entity;
    if( !entity )
    {
        npk_error( NPK_ERROR_EntityIsNull );
        return 0;
    }
    return eb->info_.originalSize_;
}

NPK_SIZE npk_entity_get_packed_size( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = entity;
    if( !entity )
    {
        npk_error( NPK_ERROR_EntityIsNull );
        return 0;
    }
    return eb->info_.size_;
}

NPK_SIZE npk_entity_get_offset( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = entity;
    if( !entity )
    {
        npk_error( NPK_ERROR_EntityIsNull );
        return 0;
    }
    return eb->info_.offset_;
}

NPK_CSTR npk_entity_get_name( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = entity;
    if( !entity )
    {
        npk_error( NPK_ERROR_EntityIsNull );
        return 0;
    }
    return eb->name_;
}

bool npk_entity_is_ready( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = entity;
    NPK_PACKAGEBODY* pb = NULL;
    int res;
    struct stat buf;

    if( !entity )
    {
        npk_error( NPK_ERROR_EntityIsNull );
        return false;
    }

    pb = eb->owner_;
    res = fstat( pb->handle_, &buf );
    if( (long)( pb->offsetJump_ + eb->info_.offset_ + eb->info_.size_ ) <= buf.st_size )
        return true;

    return false;
}

bool npk_entity_read( NPK_ENTITY entity, void* buf )
{
    NPK_ENTITYBODY* eb = entity;
    NPK_PACKAGEBODY* pb = NULL;
    void** lplpTarget = &buf;
    void* lpDecompressBuffer = NULL;
    //NPK_SIZE uncompLen = 0;
    unsigned long uncompLen = 0;
    NPK_RESULT res;

    if( !entity )
    {
        npk_error( NPK_ERROR_EntityIsNull );
        return false;
    }

    if( eb->info_.flag_ & ( NPK_ENTITY_COMPRESS_ZLIB | NPK_ENTITY_COMPRESS_BZIP2 ) )
    {
        lpDecompressBuffer = malloc( sizeof(char) * eb->info_.size_ );
        lplpTarget = &lpDecompressBuffer;
    }

    pb = eb->owner_;

    npk_package_lock( pb );

    npk_seek( pb->handle_, (long)eb->info_.offset_+pb->offsetJump_, SEEK_SET );

    res = npk_read( pb->handle_,
                    (*lplpTarget),
                    eb->info_.size_,
                    g_callbackfp,
                    NPK_PROCESSTYPE_ENTITY,
                    g_callbackSize,
                    eb->name_ );

    npk_package_free( pb );

    if( res != NPK_SUCCESS )
        goto npk_entity_read_return_null_with_free;

    // Decode before uncompress, after v21
    if( ( eb->info_.flag_ & NPK_ENTITY_ENCRYPT_TEA ) && ( eb->info_.flag_ & NPK_ENTITY_REVERSE ) )
        tea_decode_buffer((char*)(*lplpTarget), eb->info_.size_, pb->teakey_, (pb->info_.version_ >= NPK_VERSION_ENCRYPTREMAINS));

    if( eb->info_.flag_ & NPK_ENTITY_ENCRYPT_XXTEA )
        xxtea_decode_buffer((char*)(*lplpTarget), eb->info_.size_, pb->teakey_, (pb->info_.version_ >= NPK_VERSION_ENCRYPTREMAINS));

    if( eb->info_.flag_ & NPK_ENTITY_COMPRESS_ZLIB )
    {
        uncompLen = eb->info_.originalSize_;

        if( uncompLen >= NPK_MIN_SIZE_ZIPABLE )
        {
#ifdef Z_PREFIX
            if( Z_OK != z_uncompress((Bytef*)(buf), (z_uLong*)&uncompLen, (const Bytef*)lpDecompressBuffer, (z_uLong)eb->info_.size_ ) )
#else
            if( Z_OK != uncompress((Bytef*)(buf), (uLong*)&uncompLen, (const Bytef*)lpDecompressBuffer, (uLong)eb->info_.size_ ) )
#endif
            {
                npk_error( NPK_ERROR_FailToDecompress );
                goto npk_entity_read_return_null_with_free;
            }

            if( eb->info_.originalSize_ != uncompLen )
            {
                npk_error( NPK_ERROR_FailToDecompress );
                goto npk_entity_read_return_null_with_free;
            }
        }
        else
            memcpy( buf, lpDecompressBuffer, eb->info_.size_ );

        NPK_SAFE_FREE( lpDecompressBuffer );
        lplpTarget = &buf;
    }

    // Decode after uncompress, before v21
    if( ( eb->info_.flag_ & NPK_ENTITY_ENCRYPT_TEA ) && !( eb->info_.flag_ & NPK_ENTITY_REVERSE ) )
        tea_decode_buffer((char*)(*lplpTarget), eb->info_.originalSize_, pb->teakey_, false);

    return true;

npk_entity_read_return_null_with_free:
    NPK_SAFE_FREE( lpDecompressBuffer );
    return false;
}

bool npk_entity_read_partial( NPK_ENTITY entity, void* buf, NPK_SIZE offset, NPK_SIZE size )
{
    NPK_ENTITYBODY* eb = entity;
    NPK_PACKAGEBODY* pb = NULL;
    NPK_RESULT res;

    if( !entity )
    {
        npk_error( NPK_ERROR_EntityIsNull );
        return false;
    }

    if( eb->info_.flag_ & ( NPK_ENTITY_COMPRESS_ZLIB | NPK_ENTITY_COMPRESS_BZIP2 ) )
    {
        npk_error( NPK_ERROR_CantReadCompressedEntityByPartial );
        return false;
    }

    if( eb->info_.flag_ & ( NPK_ENTITY_ENCRYPT_TEA | NPK_ENTITY_ENCRYPT_XXTEA ) )
    {
        if( ( offset % 8 != 0 ) || ( ( size % 8 != 0 ) && ( offset + size != eb->info_.size_ ) ) )
        {
            npk_error( NPK_ERROR_ReadingEncryptedEntityByPartialShouldBeAligned );
            return false;
        }
    }

    pb = eb->owner_;

    npk_package_lock( pb );

    npk_seek( pb->handle_, (long)(eb->info_.offset_ + offset)+pb->offsetJump_, SEEK_SET );

    res = npk_read( pb->handle_,
                    buf,
                    size,
                    g_callbackfp,
                    NPK_PROCESSTYPE_ENTITY,
                    g_callbackSize,
                    eb->name_ );

    if( eb->info_.flag_ & NPK_ENTITY_ENCRYPT_TEA )
        tea_decode_buffer(buf, size, pb->teakey_, (pb->info_.version_ >= NPK_VERSION_ENCRYPTREMAINS));
    if( eb->info_.flag_ & NPK_ENTITY_ENCRYPT_XXTEA )
        xxtea_decode_buffer(buf, size, pb->teakey_, (pb->info_.version_ >= NPK_VERSION_ENCRYPTREMAINS));

    npk_package_free( pb );

    if( res != NPK_SUCCESS )
        return false;

    return true;
}

void npk_enable_callback( NPK_CALLBACK cb, NPK_SIZE cb_size )
{
    g_callbackfp = cb;
    g_callbackSize = cb_size;
}

void npk_disable_callback()
{
    g_callbackfp = NULL;
}

void npk_enable_criticalsection()
{
    g_useCriticalSection = 1;
}

void npk_disable_criticalsection()
{
    g_useCriticalSection = 0;
}

NPK_ENTITY npk_package_get_first_entity( NPK_PACKAGE package )
{
    NPK_PACKAGEBODY* pb = package;

    if( !package )
    {
        npk_error( NPK_ERROR_PackageIsNull );
        return NULL;
    }

    return pb->pEntityHead_;
}

NPK_ENTITY npk_entity_next( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = entity;

    if( !entity )
    {
        npk_error( NPK_ERROR_EntityIsNull );
        return NULL;
    }

    return eb->next_;
}

