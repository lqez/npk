/*

	npack - General-Purpose File Packing Library
	Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)

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
#include "npack.h"
#include "npack_dev.h"

#ifdef NPACK_PLATFORM_WINDOWS
#include <io.h>
#include <sys/utime.h>
#pragma warning( disable : 4996 )
#else
#include <utime.h>
#include <unistd.h>
#endif

#include "../external/tea/tea.h"
#include "../external/zlib/zlib.h"


NPACK_RESULT npack_error( NPACK_RESULT res )
{
	g_npackError = res;
	return res;
}

NPACK_STR npack_error_to_str( NPACK_RESULT res )
{
	switch( res )
	{
	case NPACK_SUCCESS:
		return "SUCCESS";
	case NPACK_ERROR_FileNotFound:
		return "ERROR: File not found.";
	case NPACK_ERROR_FileOpenError:
		return "ERROR: File open error.";
	case NPACK_ERROR_FileSaveError:
		return "ERROR: File save error.";
	case NPACK_ERROR_FileReadError:
		return "ERROR: File read error.";
	case NPACK_ERROR_PermissionDenied:
		return "ERROR: Permission denied.";
	case NPACK_ERROR_ReadOnlyFile:
		return "ERROR: Read only file.";
	case NPACK_ERROR_FailToGetFiletime:
		return "ERROR: Fail to get filetime.";
	case NPACK_ERROR_NotValidFileName:
		return "ERROR: Not valid filename.";
	case NPACK_ERROR_NotValidPackage:
		return "ERROR: Not valid package.";
	case NPACK_ERROR_CannotFindEntity:
		return "ERROR: Cannot find an entity.";
	case NPACK_ERROR_CannotCreateFile:
		return "ERROR: Cannot create a file.";
	case NPACK_ERROR_PackageHasNoName:
		return "ERROR: Package has no name.";
	case NPACK_ERROR_PackageHasNoVersion:
		return "ERROR: Package has no version.";
	case NPACK_ERROR_ZeroFileSize:
		return "ERROR: File size is zero.";
	case NPACK_ERROR_SameEntityExist:
		return "ERROR: Same entity name.";
	case NPACK_ERROR_FailToSetFiletime:
		return "ERROR: Fail to set filetime.";
	case NPACK_ERROR_NotValidEntity:
		return "ERROR: Not valid entity.";
	case NPACK_ERROR_OpenedPackage:
		return "ERROR: Package is already opened.";
	case NPACK_ERROR_NotOpenedPackage:
		return "ERROR: Package is not opened.";
	case NPACK_ERROR_SamePackageExist:
		return "ERROR: Same package exists.";
	case NPACK_ERROR_NonAllocatedBuffer:
		return "ERROR: Buffer is not allocated.";
	case NPACK_ERROR_NullPointerBuffer:
		return "ERROR: Buffer is null pointer.";
	case NPACK_ERROR_FailToDecompress:
		return "ERROR: Fail to decompress.";
	case NPACK_ERROR_AlreadyAllocated:
		return "ERROR: Buffer is already allocated.";
	case NPACK_ERROR_NonStaticPackage:
		return "ERROR: This is non-static package.";
	case NPACK_ERROR_NeedSpecifiedTeaKey:
		return "ERROR: Need teakey.";
	case NPACK_ERROR_EntityIsNull:
		return "ERROR: Entity pointer is null.";
	case NPACK_ERROR_PackageIsNull:
		return "ERROR: Package pointer is null.";
	case NPACK_ERROR_EntityIsNotInThePackage:
		return "ERROR: Entity is not in the package.";
	case NPACK_ERROR_CantReadCompressOrEncryptEntityByPartial:
		return "ERROR: Cannot read compressed or encrypted entity partially.";
	case NPACK_ERROR_FileAlreadyExists:
		return "ERROR: File already exists.";
	case NPACK_ERROR_NoEntityInPackage:
		return "ERROR: No entity in the package.";
	case NPACK_ERROR_InvalidTeaKey:
		return "ERROR: Invalid tea key.";
	case NPACK_ERROR_SourceStringisNull:
		return "ERROR: Cannot copy string. Source pointer is null.";
	case NPACK_ERROR_CannotCopyToItself:
		return "ERROR: Cannot copy string to itself.";
	case NPACK_ERROR_NotEnoughMemory:
		return "ERROR: Not enough memory.";
	case NPACK_ERROR_NotEnoughDiscSpace:
		return "ERROR: Not enough disk space.";
	case NPACK_ERROR_CannotFindPackage:
		return "ERROR: Cannot find the package.";
	case NPACK_ERROR_CancelByCallback:
		return "ERROR: Canceled by user input.";
	}
	return "ERROR: Unrecognized error number.";
}

NPACK_RESULT npack_alloc_copy_string( NPACK_STR* dst, NPACK_CSTR src )
{
	NPACK_SIZE len = (NPACK_SIZE)strlen(src);

	if( src == NULL )
		return( npack_error( NPACK_ERROR_SourceStringisNull ) );

	if( *dst == src )
		return( npack_error( NPACK_ERROR_CannotCopyToItself ) );

	if( *dst )
		free( *dst );

	*dst = malloc( sizeof(NPACK_CHAR)*(len+1) );
	if( *dst == NULL )
		return( npack_error( NPACK_ERROR_NotEnoughMemory ) );

	strncpy( *dst, src, len );
	(*dst)[len] = '\0';
	return NPACK_SUCCESS;
}

void npack_win32filetime_to_timet( NPACK_64BIT* pft, NPACK_TIME* pt )
{
	*pt = (NPACK_TIME)((*pft - 116444736000000000LL)/10000000LL);
}

NPACK_RESULT npack_open( int* handle, NPACK_CSTR fileName, bool createfile, bool bcheckexist )
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
				return( npack_error( NPACK_ERROR_ReadOnlyFile ) );
			close( *handle );

			*handle = open( fileName, O_CREAT | O_RDWR | O_BINARY, S_IREAD | S_IWRITE );
		}
	}
	else
		*handle = open( fileName, O_BINARY | O_RDONLY );

	if( *handle == -1 )
	{
		if( errno == ENOENT )
			return( npack_error( NPACK_ERROR_FileNotFound ) );
		else if( errno == EEXIST )
			return( npack_error( NPACK_ERROR_FileAlreadyExists ) );
		else
			return( npack_error( NPACK_ERROR_FileOpenError ) );
	}

	return NPACK_SUCCESS;
}

NPACK_RESULT npack_close( NPACK_HANDLE handle )
{
	if( handle != 0 )
		close( handle );

	return NPACK_SUCCESS;
}

long npack_seek( NPACK_HANDLE handle, long offset, int origin )
{
#ifdef NPACK_PLATFORM_WINDOWS
	return _lseek( handle, offset, origin );
#else
	return lseek( handle, offset, origin );
#endif
}

NPACK_RESULT npack_read( NPACK_HANDLE handle, void* buf, NPACK_SIZE size,
						NPACK_CALLBACK cb, int cbprocesstype, NPACK_SIZE cbsize, NPACK_CSTR cbidentifier )
{
	NPACK_SIZE currentread;
	NPACK_SIZE totalread = 0;
	NPACK_SIZE unit = cbsize;

	if( cb )
	{
		if( unit <= 0 )
			unit = size;

		do 
		{
			if( (cb)( NPACK_ACCESSTYPE_READ, cbprocesstype, cbidentifier, totalread, size ) == false )
				return( npack_error( NPACK_ERROR_CancelByCallback ) );

			if( ( size - totalread ) < unit )
				unit = size - totalread;

			currentread = read( handle, (NPACK_STR)buf + totalread, (unsigned int)unit );

			if( currentread < unit )
			{
				if( errno == EACCES )
					return( npack_error( NPACK_ERROR_PermissionDenied ) );
				else
					return( npack_error( NPACK_ERROR_FileReadError ) );
			}

				totalread += currentread;

		} while( totalread < size );

		if( (cb)( NPACK_ACCESSTYPE_READ, cbprocesstype, cbidentifier, totalread, size ) == false )
			return( npack_error( NPACK_ERROR_CancelByCallback ) );
	}
	else
	{
		currentread = read( handle, (NPACK_STR)buf, size );

		if( currentread < size )
		{
			if( errno == EACCES )
				return( npack_error( NPACK_ERROR_PermissionDenied ) );
			else
				return( npack_error( NPACK_ERROR_FileReadError ) );
		}
	}

	return NPACK_SUCCESS;
}

NPACK_RESULT npack_read_encrypt( NPACK_TEAKEY* key, NPACK_HANDLE handle, void* buf, NPACK_SIZE size,
						NPACK_CALLBACK cb, int cbprocesstype, NPACK_SIZE cbsize, NPACK_CSTR cbidentifier )
{
	NPACK_RESULT res = npack_read( handle, buf, size, cb, cbprocesstype, cbsize, cbidentifier );

	if( res == NPACK_SUCCESS )
		tea_decode_buffer( (NPACK_STR)buf, size, key );

	return res;
}

NPACK_RESULT npack_entity_alloc( NPACK_ENTITY* lpEntity )
{
	NPACK_ENTITYBODY* eb;

	eb = malloc( sizeof(NPACK_ENTITYBODY) );

	if( !eb )
		return npack_error( NPACK_ERROR_NotEnoughMemory );

	if( npack_entity_init( eb ) != NPACK_SUCCESS )
	{
		NPACK_SAFE_FREE( eb );
		return g_npackError;
	}

	*lpEntity = eb;
	return NPACK_SUCCESS;
}

NPACK_RESULT npack_entity_init( NPACK_ENTITY entity )
{
	NPACK_ENTITYBODY* eb = entity;

	if( !entity )
		return npack_error( NPACK_ERROR_EntityIsNull );

	memset( eb, 0, sizeof(NPACK_ENTITYBODY) );

	eb->newflag_				= NPACK_ENTITY_NULL;
	eb->name_					= NULL;
	eb->localname_				= NULL;
	eb->owner_					= NULL;
	eb->prev_					= NULL;
	eb->next_					= NULL;

	if( NPACK_VERSION_CURRENT >= NPACK_VERSION_REFACTORING )
	{
		eb->info_.flag_ = NPACK_ENTITY_REVERSE;
		eb->newflag_ = NPACK_ENTITY_REVERSE;
	}

	return NPACK_SUCCESS;
}

NPACK_RESULT npack_package_init( NPACK_PACKAGE package )
{
	NPACK_PACKAGEBODY* pb = package;

	if( !package )
		return npack_error( NPACK_ERROR_PackageIsNull );

	pb->info_.entityCount_		= 0;
	pb->info_.entityDataOffset_	= 0;
	pb->info_.entityInfoOffset_	= 0;
	pb->info_.version_			= 0;

	pb->handle_					= 0;
	pb->pEntityHead_			= NULL;
	pb->pEntityTail_			= NULL;

#ifdef NPACK_PLATFORM_WINDOWS
	InitializeCriticalSection( &pb->cs_ );
#endif
	return NPACK_SUCCESS;
}

NPACK_RESULT npack_package_add_entity( NPACK_PACKAGE package, NPACK_ENTITY entity )
{
	NPACK_ENTITYBODY* eb = entity;
	NPACK_PACKAGEBODY* pb = package;

	if( !entity )
		return npack_error( NPACK_ERROR_EntityIsNull );
	if( !package )
		return npack_error( NPACK_ERROR_PackageIsNull );

	if( npack_package_get_entity( package, eb->name_ ) != NULL )
		return npack_error( NPACK_ERROR_SameEntityExist );

	pb->pEntityLatest_ = entity;
	eb->owner_ = pb;

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
	++pb->info_.entityCount_;

	return NPACK_SUCCESS;
}

NPACK_RESULT npack_package_remove_all_entity( NPACK_PACKAGE package )
{
	NPACK_ENTITYBODY* eb = NULL;
	NPACK_PACKAGEBODY* pb = package;

	if( !package )
		return npack_error( NPACK_ERROR_PackageIsNull );

	while( pb->pEntityHead_ != NULL )
	{
		eb = pb->pEntityHead_;
		pb->pEntityHead_ = pb->pEntityHead_->next_;
		NPACK_SAFE_FREE( eb->name_ );
		NPACK_SAFE_FREE( eb->localname_ );
		NPACK_SAFE_FREE( eb );
	}
	pb->info_.entityCount_ = 0;
	pb->pEntityHead_ = NULL;
	pb->pEntityTail_ = NULL;
	pb->pEntityLatest_ = NULL;

	return NPACK_SUCCESS;
}

