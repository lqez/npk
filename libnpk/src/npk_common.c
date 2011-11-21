/*

    npk - General-Purpose File Packing Library
    See README for copyright and license information.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
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


NPK_RESULT npk_error( NPK_RESULT res )
{
    g_npkError = res;
    return res;
}

NPK_STR npk_error_to_str( NPK_RESULT res )
{
    switch( res )
    {
    case NPK_SUCCESS:
        return "SUCCESS";
    case NPK_ERROR_FileNotFound:
        return "ERROR: File not found.";
    case NPK_ERROR_FileOpenError:
        return "ERROR: File open error.";
    case NPK_ERROR_FileSaveError:
        return "ERROR: File save error.";
    case NPK_ERROR_FileReadError:
        return "ERROR: File read error.";
    case NPK_ERROR_PermissionDenied:
        return "ERROR: Permission denied.";
    case NPK_ERROR_ReadOnlyFile:
        return "ERROR: Read only file.";
    case NPK_ERROR_FailToGetFiletime:
        return "ERROR: Fail to get filetime.";
    case NPK_ERROR_NotValidFileName:
        return "ERROR: Not valid filename.";
    case NPK_ERROR_NotValidPackage:
        return "ERROR: Not valid package.";
    case NPK_ERROR_CannotFindEntity:
        return "ERROR: Cannot find an entity.";
    case NPK_ERROR_CannotCreateFile:
        return "ERROR: Cannot create a file.";
    case NPK_ERROR_PackageHasNoName:
        return "ERROR: Package has no name.";
    case NPK_ERROR_PackageHasNoVersion:
        return "ERROR: Package has no version.";
    case NPK_ERROR_ZeroFileSize:
        return "ERROR: File size is zero.";
    case NPK_ERROR_SameEntityExist:
        return "ERROR: Same entity name.";
    case NPK_ERROR_FailToSetFiletime:
        return "ERROR: Fail to set filetime.";
    case NPK_ERROR_NotValidEntity:
        return "ERROR: Not valid entity.";
    case NPK_ERROR_NotValidEntityName:
        return "ERROR: Not valid entity name.";
    case NPK_ERROR_OpenedPackage:
        return "ERROR: Package is already opened.";
    case NPK_ERROR_NotOpenedPackage:
        return "ERROR: Package is not opened.";
    case NPK_ERROR_SamePackageExist:
        return "ERROR: Same package exists.";
    case NPK_ERROR_NonAllocatedBuffer:
        return "ERROR: Buffer is not allocated.";
    case NPK_ERROR_NullPointerBuffer:
        return "ERROR: Buffer is null pointer.";
    case NPK_ERROR_FailToDecompress:
        return "ERROR: Fail to decompress.";
    case NPK_ERROR_AlreadyAllocated:
        return "ERROR: Buffer is already allocated.";
    case NPK_ERROR_NonStaticPackage:
        return "ERROR: This is non-static package.";
    case NPK_ERROR_NeedSpecifiedTeaKey:
        return "ERROR: Need teakey.";
    case NPK_ERROR_EntityIsNull:
        return "ERROR: Entity pointer is null.";
    case NPK_ERROR_PackageIsNull:
        return "ERROR: Package pointer is null.";
    case NPK_ERROR_EntityIsNotInThePackage:
        return "ERROR: Entity is not in the package.";
    case NPK_ERROR_CantReadCompressedEntityByPartial:
        return "ERROR: Cannot read compressed entity partially.";
    case NPK_ERROR_ReadingEncryptedEntityByPartialShouldBeAligned :
        return "ERROR: Offset and size must be aligned by 8 bytes.";
    case NPK_ERROR_FileAlreadyExists:
        return "ERROR: File already exists.";
    case NPK_ERROR_NoEntityInPackage:
        return "ERROR: No entity in the package.";
    case NPK_ERROR_InvalidTeaKey:
        return "ERROR: Invalid tea key.";
    case NPK_ERROR_EntityIsNotReady:
        return "ERROR: Entity is not ready.";
    case NPK_ERROR_PackageIsNotReady:
        return "ERROR: Package is not ready.";
    case NPK_ERROR_SourceStringisNull:
        return "ERROR: Cannot copy string. Source pointer is null.";
    case NPK_ERROR_CannotCopyToItself:
        return "ERROR: Cannot copy string to itself.";
    case NPK_ERROR_NotEnoughMemory:
        return "ERROR: Not enough memory.";
    case NPK_ERROR_NotEnoughDiscSpace:
        return "ERROR: Not enough disk space.";
    case NPK_ERROR_CannotFindPackage:
        return "ERROR: Cannot find the package.";
    case NPK_ERROR_CancelByCallback:
        return "ERROR: Canceled by user input.";
    }
    return "ERROR: Unrecognized error number.";
}

NPK_RESULT npk_alloc_copy_string( NPK_STR* dst, NPK_CSTR src )
{
    NPK_SIZE len = (NPK_SIZE)strlen(src);

    if( src == NULL )
        return( npk_error( NPK_ERROR_SourceStringisNull ) );

    if( *dst == src )
        return( npk_error( NPK_ERROR_CannotCopyToItself ) );

    if( *dst )
        free( *dst );

    *dst = malloc( sizeof(NPK_CHAR)*(len+1) );
    if( *dst == NULL )
        return( npk_error( NPK_ERROR_NotEnoughMemory ) );

    strncpy( *dst, src, len );
    (*dst)[len] = '\0';
    return NPK_SUCCESS;
}

void npk_filetime_to_unixtime( NPK_64BIT* pft, NPK_TIME* pt )
{
    *pt = (NPK_TIME)((*pft - 116444736000000000LL)/10000000LL);
}

NPK_RESULT npk_open( int* handle, NPK_CSTR fileName, bool createfile, bool bcheckexist )
{
    if( createfile )
    {
        if( bcheckexist )
        {
            *handle = open( fileName, O_CREAT | O_EXCL | O_RDWR | O_BINARY, S_IREAD | S_IWRITE );
        }
        else
        {
            *handle = creat( fileName, S_IREAD | S_IWRITE );
            if( errno == EACCES )
                return( npk_error( NPK_ERROR_ReadOnlyFile ) );
            close( *handle );

            *handle = open( fileName, O_CREAT | O_RDWR | O_BINARY, S_IREAD | S_IWRITE );
        }
    }
    else
        *handle = open( fileName, O_BINARY | O_RDONLY );

    if( *handle == -1 )
    {
        if( errno == ENOENT )
            return( npk_error( NPK_ERROR_FileNotFound ) );
        else if( errno == EEXIST )
            return( npk_error( NPK_ERROR_FileAlreadyExists ) );
        else
            return( npk_error( NPK_ERROR_FileOpenError ) );
    }

    return NPK_SUCCESS;
}

NPK_RESULT npk_close( NPK_HANDLE handle )
{
    if( handle != 0 )
        close( handle );

    return NPK_SUCCESS;
}

long npk_seek( NPK_HANDLE handle, long offset, int origin )
{
#ifdef NPK_PLATFORM_WINDOWS
    return _lseek( handle, offset, origin );
#else
    return lseek( handle, offset, origin );
#endif
}

NPK_RESULT npk_read( NPK_HANDLE handle, void* buf, NPK_SIZE size,
                        NPK_CALLBACK cb, int cbprocesstype, NPK_SIZE cbsize, NPK_CSTR cbidentifier )
{
    NPK_SIZE currentread;
    NPK_SIZE totalread = 0;
    NPK_SIZE unit = cbsize;

    if( cb )
    {
        if( unit <= 0 )
            unit = size;

        do 
        {
            if( (cb)( NPK_ACCESSTYPE_READ, cbprocesstype, cbidentifier, totalread, size ) == false )
                return( npk_error( NPK_ERROR_CancelByCallback ) );

            if( ( size - totalread ) < unit )
                unit = size - totalread;

            currentread = read( handle, (NPK_STR)buf + totalread, (unsigned int)unit );

            if( currentread < unit )
            {
                if( errno == EACCES )
                    return( npk_error( NPK_ERROR_PermissionDenied ) );
                else
                    return( npk_error( NPK_ERROR_FileReadError ) );
            }

                totalread += currentread;

        } while( totalread < size );

        if( (cb)( NPK_ACCESSTYPE_READ, cbprocesstype, cbidentifier, totalread, size ) == false )
            return( npk_error( NPK_ERROR_CancelByCallback ) );
    }
    else
    {
        currentread = read( handle, (NPK_STR)buf, size );

        if( currentread < size )
        {
            if( errno == EACCES )
                return( npk_error( NPK_ERROR_PermissionDenied ) );
            else
                return( npk_error( NPK_ERROR_FileReadError ) );
        }
    }

    return NPK_SUCCESS;
}

NPK_RESULT npk_read_encrypt( NPK_TEAKEY* key, NPK_HANDLE handle, void* buf, NPK_SIZE size,
                        NPK_CALLBACK cb, int cbprocesstype, NPK_SIZE cbsize, NPK_CSTR cbidentifier,
                        bool cipherRemains, bool useXXTEA )
{
    NPK_RESULT res = npk_read( handle, buf, size, cb, cbprocesstype, cbsize, cbidentifier );

    if( res == NPK_SUCCESS )
    {
        if( useXXTEA )
            xxtea_decode_buffer( (NPK_STR)buf, size, key, cipherRemains );
        else
            tea_decode_buffer( (NPK_STR)buf, size, key, cipherRemains );
    }

    return res;
}

NPK_RESULT npk_package_alloc( NPK_PACKAGE* lpPackage, NPK_TEAKEY teakey[4] )
{
    NPK_PACKAGEBODY* pb;
    NPK_RESULT res;
    int i;

    if( teakey == NULL )
        return npk_error( NPK_ERROR_NeedSpecifiedTeaKey );

    pb = malloc( sizeof(NPK_PACKAGEBODY) );

    if( !pb )
        return npk_error( NPK_ERROR_NotEnoughMemory );

    for( i = 0; i < NPK_HASH_BUCKETS; ++i )
    {
        pb->bucket_[i] = malloc( sizeof(NPK_BUCKET) );
        if( !pb->bucket_[i] )
            return npk_error( NPK_ERROR_NotEnoughMemory );
    }

    if( ( res = npk_package_init( pb ) ) != NPK_SUCCESS )
    {
        NPK_SAFE_FREE( pb );
        return res;
    }

    memcpy( pb->teakey_, teakey, sizeof(NPK_TEAKEY) * 4 );

    *lpPackage = pb;
    return NPK_SUCCESS;
}

NPK_RESULT npk_entity_alloc( NPK_ENTITY* lpEntity )
{
    NPK_ENTITYBODY* eb;
    NPK_RESULT res;

    eb = malloc( sizeof(NPK_ENTITYBODY) );

    if( !eb )
        return npk_error( NPK_ERROR_NotEnoughMemory );

    if( ( res = npk_entity_init( eb ) ) != NPK_SUCCESS )
    {
        NPK_SAFE_FREE( eb );
        return res;
    }

    *lpEntity = eb;
    return NPK_SUCCESS;
}

NPK_RESULT npk_entity_init( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = entity;

    if( !entity )
        return npk_error( NPK_ERROR_EntityIsNull );

    memset( eb, 0, sizeof(NPK_ENTITYBODY) );

    eb->newflag_                = NPK_ENTITY_NULL;
    eb->name_                   = NULL;
    eb->localname_              = NULL;
    eb->owner_                  = NULL;
    eb->prev_                   = NULL;
    eb->next_                   = NULL;

    if( NPK_VERSION_CURRENT >= NPK_VERSION_REFACTORING )
    {
        eb->info_.flag_ = NPK_ENTITY_REVERSE;
        eb->newflag_ = NPK_ENTITY_REVERSE;
    }

    return NPK_SUCCESS;
}

NPK_RESULT npk_package_init( NPK_PACKAGE package )
{
    NPK_PACKAGEBODY* pb = package;
    int i = 0;

    if( !package )
        return npk_error( NPK_ERROR_PackageIsNull );

    pb->info_.entityCount_      = 0;
    pb->info_.entityDataOffset_ = 0;
    pb->info_.entityInfoOffset_ = 0;
    pb->info_.version_          = 0;

    pb->handle_                 = 0;
    pb->pEntityHead_            = NULL;
    pb->pEntityTail_            = NULL;

#ifdef NPK_PLATFORM_WINDOWS
    InitializeCriticalSection( &pb->cs_ );
#endif

    for( i = 0; i < NPK_HASH_BUCKETS; ++i )
    {
        pb->bucket_[i]->pEntityHead_ = NULL;
        pb->bucket_[i]->pEntityTail_ = NULL;
    }

    pb->usingHashmap_           = false;
    pb->usingFdopen_            = false;
    pb->offsetJump_             = 0;
    return NPK_SUCCESS;
}

NPK_RESULT __npk_package_add_entity( NPK_PACKAGE package, NPK_ENTITY entity, bool check )
{
    NPK_ENTITYBODY* eb = entity;
    NPK_PACKAGEBODY* pb = package;
    NPK_BUCKET* bucket = NULL;

    if( check )
    {
        if( !entity )
            return npk_error( NPK_ERROR_EntityIsNull );
        if( !package )
            return npk_error( NPK_ERROR_PackageIsNull );
        if( npk_package_get_entity( package, eb->name_ ) != NULL )
            return npk_error( NPK_ERROR_SameEntityExist );
    }

    pb->pEntityLatest_ = entity;
    eb->owner_ = pb;
    bucket = pb->bucket_[npk_get_bucket(eb->name_)];

    if( pb->pEntityHead_ == NULL )
    {
        pb->pEntityHead_ = eb;
        pb->pEntityTail_ = eb;
    }
    else
    {
        pb->pEntityTail_->next_ = eb;
        eb->prev_ = pb->pEntityTail_;
        pb->pEntityTail_ = eb;
    }

    if( bucket->pEntityHead_ == NULL )
    {
        bucket->pEntityHead_ = eb;
        bucket->pEntityTail_ = eb;
    }
    else
    {
        bucket->pEntityTail_->nextInBucket_ = eb;
        eb->prevInBucket_ = bucket->pEntityTail_;
        bucket->pEntityTail_ = eb;
    }

    ++pb->info_.entityCount_;

    if( pb->info_.entityCount_ >= NPK_HASH_BUCKETS )
        pb->usingHashmap_ = true;

    return NPK_SUCCESS;
}

NPK_RESULT npk_package_add_entity( NPK_PACKAGE package, NPK_ENTITY entity )
{
    return __npk_package_add_entity( package, entity, true );
}

/* adler32 algorithm from http://en.wikipedia.org/wiki/Adler-32 */
NPK_HASHKEY npk_get_bucket( NPK_CSTR name )
{
    const int MOD_ADLER = 65521;
    NPK_HASHKEY a = 1, b = 0;

    while( *name )
    {
        a = (a + *name) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
        ++name;
    }
    return ( (b << 16) | a ) % NPK_HASH_BUCKETS;
}

NPK_RESULT npk_prepare_entityname( NPK_CSTR src, NPK_STR dst, size_t dstLen )
{
    size_t i;
    size_t l = strlen(src);
    if( l >= dstLen )
        return npk_error( NPK_ERROR_NotValidEntityName );

    for( i = 0 ; i < l; ++i )
        if( src[i] == '\\' ) dst[i] = '/'; else dst[i] = src[i];
    dst[i] = '\0';

    return NPK_SUCCESS;
}
