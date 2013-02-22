/*

    npk - neat package system
    See README for copyright and license information.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "npk.h"
#include "npk_dev.h"

#ifdef NPK_PLATFORM_WINDOWS
#include <io.h>
#include <sys/utime.h>
#pragma warning( disable : 4996 )
#else
#include <utime.h>
#include <unistd.h>
#endif

#include "../external/tea/tea.h"
#include "../external/xxtea/xxtea.h"
#include "../external/zlib/zlib.h"


#ifdef NPK_DEV

bool        g_use_gluetime;
NPK_TIME    g_gluetime;

void npk_log( NPK_CSTR format, ... )
{
    NPK_CHAR buf[512];

    va_list args;
    va_start( args, format );
    vsnprintf( buf, sizeof(buf) / sizeof(buf[0]), format, args);
    va_end( args );

    printf( "%s\n", buf );
}

NPK_RESULT npk_get_filetime( NPK_CSTR filename, NPK_TIME* pft )
{
    struct stat __sbuf;
    int result;

    if( g_use_gluetime )
        *pft = g_gluetime;
    else
    {
        result = stat( filename, &__sbuf );
        if( result != 0 )
        {
            switch( errno )
            {
            case ENOENT:
                return( npk_error( NPK_ERROR_FileNotFound ) );
            }
            return( npk_error( NPK_ERROR_FailToGetFiletime ) );
        }
        *pft = (NPK_TIME)__sbuf.st_mtime;
    }

    return NPK_SUCCESS;
}

NPK_RESULT npk_set_filetime( NPK_CSTR filename, const NPK_TIME pft )
{
    struct stat __sbuf;
    struct utimbuf __ubuf;
    int result;
    
    result = stat( filename, &__sbuf );
    if( result != 0 )
    {
        switch( errno )
        {
        case ENOENT:
            return( npk_error( NPK_ERROR_FileNotFound ) );
        }
        return( npk_error( NPK_ERROR_FailToGetFiletime ) );
    }
    __ubuf.actime = __sbuf.st_atime;
    __ubuf.modtime = pft;
    utime( filename, &__ubuf );

    return NPK_SUCCESS;
}

void npk_enable_gluetime( NPK_TIME time )
{
    g_use_gluetime = true;
    g_gluetime = time;
}

void npk_disable_gluetime()
{
    g_use_gluetime = false;
}

NPK_RESULT npk_flush( NPK_HANDLE handle )
{
    if( handle != 0 )
    {
        if(!__use_commit)
#ifdef NPK_PLATFORM_WINDOWS
            _commit( handle );
#else
            fsync( handle );
#endif
        else
            __commit( (void*)handle );
    }
    return NPK_SUCCESS;
}

long npk_tell( NPK_HANDLE handle )
{
    if(!__use_seek)
#ifdef NPK_PLATFORM_WINDOWS
        return _lseek( handle, 0, SEEK_CUR );
#else
        return lseek( handle, 0, SEEK_CUR );
#endif
    else
        return __seek( (void*)handle, 0, SEEK_CUR );
}

NPK_RESULT npk_write( NPK_HANDLE handle, const void* buf, NPK_SIZE size,
                        NPK_CALLBACK cb, int cbprocesstype, NPK_SIZE cbsize, NPK_CSTR cbidentifier )
{
    NPK_SIZE currentwritten;
    NPK_SIZE totalwritten = 0;
    NPK_SIZE unit = cbsize;

    if( cb )
    {
        if( unit <= 0 )
            unit = size;

        do
        {
            if( (cb)( NPK_ACCESSTYPE_WRITE, cbprocesstype, cbidentifier, totalwritten, size ) == false )
                return( npk_error( NPK_ERROR_CancelByCallback ) );

            if( (int)( size - totalwritten ) < unit )
                unit = size - totalwritten;

            if(!__use_write)
                currentwritten = write( handle, (NPK_STR)buf + totalwritten, (unsigned int)unit );
            else
                currentwritten = __write( (NPK_STR)buf + totalwritten, sizeof(char*), (unsigned int)unit, (void*)handle );

            if( currentwritten < unit )
            {
                if( errno == EACCES )
                    return( npk_error( NPK_ERROR_PermissionDenied ) );
                else if( errno == ENOSPC )
                    return( npk_error( NPK_ERROR_NotEnoughDiscSpace ) );
                else
                    return( npk_error( NPK_ERROR_FileSaveError ) );
            }

            totalwritten += currentwritten;

        } while( totalwritten < size );

        if( (cb)( NPK_ACCESSTYPE_WRITE, cbprocesstype, cbidentifier, totalwritten, size ) == false )
            return( npk_error( NPK_ERROR_CancelByCallback ) );
    }
    else
    {
        currentwritten = write( handle, (NPK_STR)buf, size );

        if( currentwritten < size )
        {
            if( errno == EACCES )
                return( npk_error( NPK_ERROR_PermissionDenied ) );
            else if( errno == ENOSPC )
                return( npk_error( NPK_ERROR_NotEnoughDiscSpace ) );
            else
                return( npk_error( NPK_ERROR_FileSaveError ) );
        }
    }
    return NPK_SUCCESS;
}

NPK_RESULT npk_write_encrypt( NPK_TEAKEY* key, NPK_HANDLE handle, const void* buf, NPK_SIZE size,
                        NPK_CALLBACK cb, int cbprocesstype, NPK_SIZE cbsize, NPK_CSTR cbidentifier,
                        bool cipherRemains, bool useXXTEA )
{
    NPK_RESULT res;
    void* bufferforencode = malloc( sizeof(char) * size );

    if( !bufferforencode )
        return npk_error( NPK_ERROR_NotEnoughMemory );

    memcpy( bufferforencode, buf, sizeof(char) * size );
    if( useXXTEA )
        xxtea_encode_buffer( (NPK_STR)bufferforencode, size, key, cipherRemains );
    else
        tea_encode_buffer( (NPK_STR)bufferforencode, size, key, cipherRemains );

    res = npk_write( handle, bufferforencode, size, cb, cbprocesstype, cbsize, cbidentifier );
    free( bufferforencode );
    return res;
}

NPK_RESULT npk_entity_get_new_flag( NPK_ENTITY entity, NPK_FLAG* flag )
{
    NPK_ENTITYBODY* eb = entity;
    if( !eb )
        return npk_error( NPK_ERROR_EntityIsNull );

    *flag = eb->newflag_;
    return NPK_SUCCESS;
}

NPK_RESULT npk_entity_set_flag( NPK_ENTITY entity, NPK_FLAG flag )
{
    NPK_ENTITYBODY* eb = entity;
    if( !eb )
        return npk_error( NPK_ERROR_EntityIsNull );

    eb->newflag_ = flag;
    return NPK_SUCCESS;
}

NPK_RESULT npk_entity_add_flag( NPK_ENTITY entity, NPK_FLAG flag )
{
    NPK_ENTITYBODY* eb = entity;
    if( !eb )
        return npk_error( NPK_ERROR_EntityIsNull );

    eb->newflag_ |= flag;
    return NPK_SUCCESS;
}

NPK_RESULT npk_entity_sub_flag( NPK_ENTITY entity, NPK_FLAG flag )
{
    NPK_ENTITYBODY* eb = entity;
    if( !eb )
        return npk_error( NPK_ERROR_EntityIsNull );

    eb->newflag_ &= ~flag;
    return NPK_SUCCESS;
}

NPK_RESULT npk_entity_write( NPK_ENTITY entity, NPK_HANDLE handle, bool forceProcessing )
{
    NPK_PACKAGEBODY*    pb;
    NPK_ENTITYBODY*     eb = entity;
    NPK_RESULT          res;
    bool                skipProcessing;

    void*               buf = NULL;
    void*               buf_for_zlib = NULL;
    NPK_SIZE            size, endpos, startpos;
    NPK_HANDLE          filehandle;
    int                 z_res;

    if( !eb )
        return npk_error( NPK_ERROR_EntityIsNull );

    pb = eb->owner_;

    skipProcessing = false;
    if( eb->localname_ != NULL )
    {   // read native file and write
        if( ( res = npk_open( &filehandle, eb->localname_, false, false ) ) != NPK_SUCCESS )
            return res;

        endpos      = npk_seek( filehandle, 0, SEEK_END );
        startpos    = npk_seek( filehandle, 0, SEEK_SET );
        size        = endpos - startpos;

        if( size == 0 )
            return npk_error( NPK_ERROR_ZeroFileSize );

        eb->info_.originalSize_ = size;
        buf = malloc( size );

        if( ( res = npk_read( filehandle,
                        buf,
                        size,
                        g_callbackfp,
                        NPK_PROCESSTYPE_ENTITY,
                        g_callbackSize,
                        eb->name_ ) ) != NPK_SUCCESS )
            goto npk_entity_write_return_with_free;

        npk_close( filehandle );
        npk_get_filetime( eb->localname_, &eb->info_.modified_ );
        NPK_SAFE_FREE( eb->localname_ );
    }
    else
    {
        if( eb->newflag_ != eb->info_.flag_ || forceProcessing )
        {   // read entity and write
            size = eb->info_.originalSize_;
            buf = malloc( size );
            npk_entity_read( eb, buf );
        }
        else
        {   // just copy
            size = eb->info_.size_;
            buf = malloc( size );
            npk_seek( pb->handle_, (long)eb->info_.offset_+pb->offsetJump_, SEEK_SET );

            if( ( res = npk_read( pb->handle_,
                            buf,
                            size,
                            g_callbackfp,
                            NPK_PROCESSTYPE_ENTITY,
                            g_callbackSize,
                            eb->name_ ) ) != NPK_SUCCESS )
                goto npk_entity_write_return_with_free;
            skipProcessing = true;
        }
    }

    if( !skipProcessing )
    {
        // Encode before compress, before v21
        if( ( eb->newflag_ & NPK_ENTITY_ENCRYPT_TEA ) && !( eb->newflag_ & NPK_ENTITY_REVERSE ) )
            tea_encode_buffer((char*)buf, (int)size, pb->teakey_, false );

        if( eb->newflag_ & NPK_ENTITY_COMPRESS_ZLIB )
        {
            if( size >= NPK_MIN_SIZE_ZIPABLE )
            {
                unsigned long compressedSize = (unsigned long)(sizeof(char) * size * 1.1 + 12); // margin rules from zlib/compress.c
                buf_for_zlib = malloc( compressedSize );
#ifdef Z_PREFIX
                z_res = z_compress( (Bytef*)buf_for_zlib, (z_uLong*)&compressedSize, (const Bytef*)buf, (z_uLong)size );
#else
                z_res = compress( (Bytef*)buf_for_zlib, (uLong*)&compressedSize, (const Bytef*)buf, (uLong)size );
#endif
                if( ( z_res == Z_OK ) && ( compressedSize < size ) )
                {
                    free( buf );
                    buf = buf_for_zlib;
                    buf_for_zlib = NULL;
                    size = compressedSize;
                }
                else    // not suitable to compress
                {
                    free( buf_for_zlib );
                    eb->newflag_ &= !NPK_ENTITY_COMPRESS_ZLIB;
                }
            }
        }

        // Encode after compress, after v21
        if( ( eb->newflag_ & NPK_ENTITY_ENCRYPT_TEA ) && ( eb->newflag_ & NPK_ENTITY_REVERSE ) )
            tea_encode_buffer((char*)buf, (int)size, pb->teakey_, (NPK_VERSION_CURRENT >= NPK_VERSION_ENCRYPTREMAINS) );

        if( eb->newflag_ & NPK_ENTITY_ENCRYPT_XXTEA )
            xxtea_encode_buffer((char*)buf, (int)size, pb->teakey_, (NPK_VERSION_CURRENT >= NPK_VERSION_ENCRYPTREMAINS) );
    }

    eb->info_.size_ = size;
    eb->info_.offset_ = npk_tell( handle );
    if( ( res = npk_write( handle,
                    buf,
                    size,
                    g_callbackfp,
                    NPK_PROCESSTYPE_ENTITY,
                    g_callbackSize,
                    eb->name_ ) ) != NPK_SUCCESS )
        goto npk_entity_write_return_with_free;

    free( buf );

    eb->info_.flag_ = eb->newflag_;

    return NPK_SUCCESS;

npk_entity_write_return_with_free:
    NPK_SAFE_FREE( buf );
    NPK_SAFE_FREE( buf_for_zlib );
    return res;
}

NPK_RESULT npk_entity_export( NPK_ENTITY entity, NPK_CSTR filename, bool forceOverwrite )
{
    void* buf;
    NPK_HANDLE handle;
    NPK_ENTITYBODY* eb = entity;
    NPK_RESULT res;

    if( !entity )
        return npk_error( NPK_ERROR_EntityIsNull );

    buf = malloc( eb->info_.originalSize_ );
    if( !buf )
        return npk_error( NPK_ERROR_NotEnoughMemory );

    if( !( res = npk_entity_read( eb, buf ) ) )
        return res;

    if( ( res = npk_open( &handle, filename, true, true ) ) != NPK_SUCCESS )
    {
        if( !forceOverwrite )
            return res;
    
        if( ( res = npk_open( &handle, filename, true, false ) ) != NPK_SUCCESS )
            return res;
    }

    if( ( res = npk_write( handle,
                    buf,
                    eb->info_.originalSize_,
                    g_callbackfp,
                    NPK_PROCESSTYPE_ENTITY,
                    g_callbackSize,
                    eb->name_ ) ) != NPK_SUCCESS )
        return res;

    if( ( res = npk_close( handle ) ) != NPK_SUCCESS )
        return res;

    npk_set_filetime( filename, eb->info_.modified_ );

    free( buf );
    return NPK_SUCCESS;
}


NPK_RESULT npk_package_clear( NPK_PACKAGE package )
{
    NPK_RESULT res;
    if( !package )
        return npk_error( NPK_ERROR_PackageIsNull );

    if( ( res = npk_package_remove_all_entity( package ) ) != NPK_SUCCESS )
        return res;

    if( ( res = npk_package_init( package ) ) != NPK_SUCCESS )
        return res;

    return NPK_SUCCESS;
}

NPK_RESULT npk_package_save( NPK_PACKAGE package, NPK_CSTR filename, bool forceOverwrite )
{
    NPK_PACKAGEBODY*    pb = package;
    NPK_ENTITYBODY*     eb = NULL;
    NPK_RESULT          res;
    bool                bVersionUp = false;
    bool                bUseTemporaryFile = false;
    NPK_SIZE            len;
    int                 savecount = 0;
    NPK_STR             savefilename = NULL;
    NPK_HANDLE          savefilehandle;
    NPK_CHAR*           buf;
    NPK_CHAR*           buf_pos;
    NPK_PACKAGEINFO_V23 header_v23;

    if( !package )
        return npk_error( NPK_ERROR_PackageIsNull );

    if( !filename )
        return npk_error( NPK_ERROR_PackageHasNoName );

    if( ( res = npk_open( &savefilehandle, filename, true, true ) ) == NPK_SUCCESS )
    {
        npk_alloc_copy_string( &savefilename, filename );
    }
    else
    {
        if( res != NPK_ERROR_FileAlreadyExists )
            return res;

        if( !forceOverwrite )
            return res;

        len = (NPK_SIZE)strlen( filename );
        savefilename = malloc( sizeof(NPK_CHAR)*(len+2) );
        if( savefilename == NULL )
            return( npk_error( NPK_ERROR_NotEnoughMemory ) );

        strncpy( savefilename, filename, len );
        savefilename[len+0] = '_';
        savefilename[len+1] = '\0';
        bUseTemporaryFile = true;

        if( ( res = npk_open( &savefilehandle, savefilename, true, false ) ) != NPK_SUCCESS )
            return res;
    }

    strncpy( pb->info_.signature_, NPK_SIGNATURE, sizeof(NPK_CHAR)*4 );
    if( pb->info_.version_ != NPK_VERSION_CURRENT )
        bVersionUp = true;

    pb->info_.entityInfoOffset_ = sizeof(NPK_PACKAGEINFO)
                                + sizeof(NPK_PACKAGEINFO_V23);

    // version 27, precalculate entity information header length 
    eb = pb->pEntityHead_;
    len = 0;
    while( eb != NULL )
    {
        len += sizeof(NPK_ENTITYINFO);
        len += sizeof(NPK_CHAR)*eb->info_.nameLength_;
        eb = eb->next_;
    }

    pb->info_.entityDataOffset_ = pb->info_.entityInfoOffset_ + len;
    npk_seek( savefilehandle, (long)pb->info_.entityDataOffset_, SEEK_SET );

    eb = pb->pEntityHead_;
    while( eb != NULL )
    {
        if( g_callbackfp )
            if( (g_callbackfp)( NPK_ACCESSTYPE_WRITE, NPK_PROCESSTYPE_PACKAGE, filename, savecount, pb->info_.entityCount_ ) == false )
                return( npk_error( NPK_ERROR_CancelByCallback ) );

        npk_entity_write( eb, savefilehandle, bVersionUp );
        ++savecount;
        eb = eb->next_;
    }

    pb->info_.entityCount_ = savecount;

    // version 24, Take single encryption to whole entity headers
    buf = malloc( (sizeof(NPK_ENTITYINFO)+260)*savecount ); // 260 = MAX_PATH on windows, isn't it enough?
    if( !buf )
        return( npk_error( NPK_ERROR_NotEnoughMemory ) );
    buf_pos = buf;

    eb = pb->pEntityHead_;
    while( eb != NULL )
    {
        memcpy( buf_pos, &eb->info_, sizeof(NPK_ENTITYINFO) );
        buf_pos += sizeof(NPK_ENTITYINFO);
        memcpy( buf_pos, eb->name_, sizeof(NPK_CHAR)*eb->info_.nameLength_ );
        buf_pos += sizeof(NPK_CHAR)*eb->info_.nameLength_;
        eb = eb->next_;
    }

    npk_seek( savefilehandle, (long)pb->info_.entityInfoOffset_, SEEK_SET );
    if( ( res = npk_write_encrypt( pb->teakey_,
                            savefilehandle,
                            buf,
                            buf_pos - buf,
                            g_callbackfp,
                            NPK_PROCESSTYPE_ENTITYHEADER,
                            g_callbackSize,
                            savefilename,
                            (NPK_VERSION_CURRENT >= NPK_VERSION_ENCRYPTREMAINS),
                            (NPK_VERSION_CURRENT >= NPK_VERSION_USEXXTEAONHEADER)
                            ) ) != NPK_SUCCESS )
        return res;
    NPK_SAFE_FREE( buf );

    pb->info_.version_ = NPK_VERSION_CURRENT;
    npk_seek( savefilehandle, 0, SEEK_SET );
    if( ( res = npk_write( savefilehandle,
                    &pb->info_,
                    sizeof(NPK_PACKAGEINFO),
                    g_callbackfp,
                    NPK_PROCESSTYPE_PACKAGEHEADER,
                    g_callbackSize,
                    savefilename ) ) != NPK_SUCCESS )
        return res;

    // version 23, Write the package timestamp for other applications
    header_v23.modified_ = (NPK_TIME)time(NULL);

    if( ( res = npk_write( savefilehandle,
                    &header_v23,
                    sizeof(NPK_PACKAGEINFO_V23),
                    g_callbackfp,
                    NPK_PROCESSTYPE_PACKAGEHEADER,
                    g_callbackSize,
                    savefilename ) ) != NPK_SUCCESS )
        return res;

    npk_flush( savefilehandle );
    npk_close( pb->handle_ );

    if( bUseTemporaryFile )
    {
        npk_close( savefilehandle );
        remove( filename );
        rename( savefilename, filename );

        if( (res = npk_open( &pb->handle_, filename, false, false ) ) != NPK_SUCCESS )
            return res;
    }
    else
    {
        pb->handle_ = savefilehandle;
    }
    NPK_SAFE_FREE( savefilename );
    return NPK_SUCCESS;
}

NPK_RESULT npk_package_add_file( NPK_PACKAGE package, NPK_CSTR filename, NPK_CSTR entityname, NPK_ENTITY* lpEntity )
{
    NPK_ENTITYBODY* eb;
    NPK_CSTR __entityname;
    NPK_RESULT res;
    NPK_CHAR namebuf[512];

    if( ( res = npk_entity_alloc( (NPK_ENTITY*)&eb ) ) != NPK_SUCCESS )
        return res;

    if( entityname == NULL )
    {
        __entityname = NULL;

        if( ( entityname = strrchr( filename, '\\' ) ) == NULL )
            if( ( entityname = strrchr( filename, '/' ) ) == NULL )
                __entityname = filename;

        if( __entityname == NULL )
            __entityname = entityname + sizeof(NPK_CHAR);
    }
    else
        __entityname = entityname;

    if( ( res = npk_prepare_entityname( __entityname, namebuf, 512 ) ) != NPK_SUCCESS )
        goto npk_package_add_file_return_with_error;

    if( ( res = npk_get_filetime( filename, &eb->info_.modified_ ) ) != NPK_SUCCESS )
        goto npk_package_add_file_return_with_error;

    if( ( res = npk_alloc_copy_string( &eb->localname_, filename ) ) != NPK_SUCCESS )
        goto npk_package_add_file_return_with_error;

    if( ( res = npk_alloc_copy_string( &eb->name_, namebuf ) ) != NPK_SUCCESS )
        goto npk_package_add_file_return_with_error;

    eb->info_.nameLength_ = (NPK_SIZE)strlen( eb->name_ );

    if( ( res = npk_package_add_entity( package, eb ) ) != NPK_SUCCESS )
        goto npk_package_add_file_return_with_error;

    if( lpEntity )
        *lpEntity = eb;

    return NPK_SUCCESS;

npk_package_add_file_return_with_error:
    NPK_SAFE_FREE( eb );
    return res;
}

NPK_ENTITY _npk_package_add_file( NPK_PACKAGE package, NPK_CSTR filename, NPK_CSTR entityname )
{
    NPK_ENTITY e = NULL;
    if( NPK_SUCCESS == npk_package_add_file( package, filename, entityname, &e ) )
        return e;

    return NULL;
} 

NPK_RESULT __npk_package_remove_entity( NPK_PACKAGE package, NPK_ENTITY entity, bool deepRemove )
{
    NPK_ENTITYBODY* eb = entity;
    NPK_PACKAGEBODY* pb = package;
    NPK_BUCKET* bucket = NULL;

    if( !entity )
        return npk_error( NPK_ERROR_EntityIsNull );
    if( !package )
        return npk_error( NPK_ERROR_PackageIsNull );
    if( eb->owner_ != package )
        return npk_error( NPK_ERROR_EntityIsNotInThePackage );

    bucket = pb->bucket_[npk_get_bucket(eb->name_)];

    if( eb->prev_ )
        eb->prev_->next_ = eb->next_;
    if( eb->next_ )
        eb->next_->prev_ = eb->prev_;

    if( eb->prevInBucket_ )
        eb->prevInBucket_->nextInBucket_ = eb->nextInBucket_;
    if( eb->nextInBucket_ )
        eb->nextInBucket_->prevInBucket_ = eb->prevInBucket_;

    if( eb == pb->pEntityHead_ )
        pb->pEntityHead_ = eb->next_;
    if( eb == pb->pEntityTail_ )
        pb->pEntityTail_ = eb->prev_;

    if( eb == bucket->pEntityHead_ )
        bucket->pEntityHead_ = eb->nextInBucket_;
    if( eb == bucket->pEntityTail_ )
        bucket->pEntityTail_ = eb->prevInBucket_;

    pb->pEntityLatest_ = eb->next_;
    --pb->info_.entityCount_;

    if( deepRemove )
    {
        NPK_SAFE_FREE( eb->name_ );
        NPK_SAFE_FREE( eb->localname_ );
        NPK_SAFE_FREE( eb );
    }
    else
    {
        eb->next_ = NULL;
        eb->prev_ = NULL;
        eb->nextInBucket_ = NULL;
        eb->prevInBucket_ = NULL;
    }

    return NPK_SUCCESS;
}

NPK_RESULT npk_package_remove_entity( NPK_PACKAGE package, NPK_ENTITY entity )
{
    return __npk_package_remove_entity( package, entity, true );
}

NPK_RESULT npk_package_detach_entity( NPK_PACKAGE package, NPK_ENTITY entity )
{
    return __npk_package_remove_entity( package, entity, false );
}

NPK_RESULT __npk_package_remove_all_entity( NPK_PACKAGE package, bool deepRemove );
NPK_RESULT npk_package_detach_all_entity( NPK_PACKAGE package )
{
    return __npk_package_remove_all_entity( package, false );
}
#endif /* NPK_DEV */

