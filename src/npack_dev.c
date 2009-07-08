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


#ifdef NPACK_DEV

bool		g_use_gluetime;
NPACK_TIME	g_gluetime;

void npack_log( NPACK_CSTR format, ... )
{
	NPACK_CHAR buf[512];

	va_list args;
	va_start( args, format );
	vsnprintf( buf, sizeof(buf) / sizeof(buf[0]), format, args);
	va_end( args );

	printf( "%s\n", buf );
}

NPACK_RESULT npack_get_filetime( NPACK_CSTR filename, NPACK_TIME* pft )
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
				return( npack_error( NPACK_ERROR_FileNotFound ) );
			}
			return( npack_error( NPACK_ERROR_FailToGetFiletime ) );
		}
		*pft = (NPACK_TIME)__sbuf.st_mtime;
	}

	return NPACK_SUCCESS;
}

NPACK_RESULT npack_set_filetime( NPACK_CSTR filename, const NPACK_TIME pft )
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
			return( npack_error( NPACK_ERROR_FileNotFound ) );
		}
		return( npack_error( NPACK_ERROR_FailToGetFiletime ) );
	}
	__ubuf.actime = __sbuf.st_atime;
	__ubuf.modtime = pft;
	utime( filename, &__ubuf );

	return NPACK_SUCCESS;
}

void npack_enable_gluetime( NPACK_TIME time )
{
	g_use_gluetime = true;
	g_gluetime = time;
}

void npack_disable_gluetime()
{
	g_use_gluetime = false;
}


NPACK_RESULT npack_flush( NPACK_HANDLE handle )
{
	if( handle != 0 )
	{
#ifdef NPACK_PLATFORM_WINDOWS
	_commit( handle );
#else
	fsync( handle );
#endif
	}

	return NPACK_SUCCESS;
}

long npack_tell( NPACK_HANDLE handle )
{
#ifdef NPACK_PLATFORM_WINDOWS
	return _lseek( handle, 0, SEEK_CUR );
#else
	return lseek( handle, 0, SEEK_CUR );
#endif
}

NPACK_RESULT npack_write( NPACK_HANDLE handle, const void* buf, NPACK_SIZE size,
						NPACK_CALLBACK cb, int cbprocesstype, NPACK_SIZE cbsize, NPACK_CSTR cbidentifier )
{
	NPACK_SIZE currentwritten;
	NPACK_SIZE totalwritten = 0;
	NPACK_SIZE unit = cbsize;

	if( cb )
	{
		if( unit <= 0 )
			unit = size;

		do
		{
			if( (cb)( NPACK_ACCESSTYPE_WRITE, cbprocesstype, cbidentifier, totalwritten, size ) == false )
				return( npack_error( NPACK_ERROR_CancelByCallback ) );

			if( (int)( size - totalwritten ) < unit )
				unit = size - totalwritten;

			currentwritten = write( handle, (NPACK_STR)buf + totalwritten, (unsigned int)unit );

			if( currentwritten < unit )
			{
				if( errno == EACCES )
					return( npack_error( NPACK_ERROR_PermissionDenied ) );
				else if( errno == ENOSPC )
					return( npack_error( NPACK_ERROR_NotEnoughDiscSpace ) );
				else
					return( npack_error( NPACK_ERROR_FileSaveError ) );
			}

			totalwritten += currentwritten;

		} while( totalwritten < size );

		if( (cb)( NPACK_ACCESSTYPE_WRITE, cbprocesstype, cbidentifier, totalwritten, size ) == false )
			return( npack_error( NPACK_ERROR_CancelByCallback ) );
	}
	else
	{
		currentwritten = write( handle, (NPACK_STR)buf, size );

		if( currentwritten < size )
		{
			if( errno == EACCES )
				return( npack_error( NPACK_ERROR_PermissionDenied ) );
			else if( errno == ENOSPC )
				return( npack_error( NPACK_ERROR_NotEnoughDiscSpace ) );
			else
				return( npack_error( NPACK_ERROR_FileSaveError ) );
		}
	}
	return NPACK_SUCCESS;
}

NPACK_RESULT npack_write_encrypt( NPACK_TEAKEY* key, NPACK_HANDLE handle, const void* buf, NPACK_SIZE size,
						NPACK_CALLBACK cb, int cbprocesstype, NPACK_SIZE cbsize, NPACK_CSTR cbidentifier )
{
	NPACK_RESULT res;
	void* bufferforencode = malloc( sizeof(char) * size );

	if( !bufferforencode )
		return npack_error( NPACK_ERROR_NotEnoughMemory );

	memcpy( bufferforencode, buf, sizeof(char) * size );
	tea_encode_buffer( (NPACK_STR)bufferforencode, size, key );

	res = npack_write( handle, bufferforencode, size, cb, cbprocesstype, cbsize, cbidentifier );
	free( bufferforencode );
	return res;
}

NPACK_RESULT npack_entity_set_flag( NPACK_ENTITY entity, NPACK_FLAG flag )
{
	NPACK_ENTITYBODY*	eb = entity;
	if( !eb )
		return npack_error( NPACK_ERROR_EntityIsNull );

	eb->newflag_ = flag;
	return NPACK_SUCCESS;
}

NPACK_RESULT npack_entity_add_flag( NPACK_ENTITY entity, NPACK_FLAG flag )
{
	NPACK_ENTITYBODY*	eb = entity;
	if( !eb )
		return npack_error( NPACK_ERROR_EntityIsNull );

	eb->newflag_ |= flag;
	return NPACK_SUCCESS;
}

NPACK_RESULT npack_entity_sub_flag( NPACK_ENTITY entity, NPACK_FLAG flag )
{
	NPACK_ENTITYBODY*	eb = entity;
	if( !eb )
		return npack_error( NPACK_ERROR_EntityIsNull );

	eb->newflag_ &= ~flag;
	return NPACK_SUCCESS;
}

NPACK_RESULT npack_entity_write( NPACK_ENTITY entity, NPACK_HANDLE handle )
{
	NPACK_PACKAGEBODY*	pb;
	NPACK_ENTITYBODY*	eb = entity;
	bool				skipProcessing;

	void*				buf = NULL;
	void*				buf_for_zlib = NULL;
	NPACK_SIZE			compressedSize, size, endpos, startpos;
	int					filehandle;

	if( !eb )
		return npack_error( NPACK_ERROR_EntityIsNull );

	pb = eb->owner_;

	skipProcessing = false;
	if( eb->localname_ != NULL )
	{	// read native file and write
		if( npack_open( &filehandle, eb->localname_, false, false ) != NPACK_SUCCESS )
			return g_npackError;

		endpos		= npack_seek( filehandle, 0, SEEK_END );
		startpos	= npack_seek( filehandle, 0, SEEK_SET );
		size		= endpos - startpos;

		if( size == 0 )
			return npack_error( NPACK_ERROR_ZeroFileSize );

		eb->info_.originalSize_ = size;
		buf = malloc( size );

		if( npack_read( filehandle,
						buf,
						size,
						g_callbackfp,
						NPACK_PROCESSTYPE_ENTITY,
						g_callbackSize,
						eb->name_ ) != NPACK_SUCCESS )
			goto npack_entity_write_return_with_free;

		npack_close( filehandle );
		npack_get_filetime( eb->localname_, &eb->info_.modified_ );
		NPACK_SAFE_FREE( eb->localname_ );
	}
	else
	{
		if( eb->newflag_ != eb->info_.flag_ )
		{	// read entity and write
			size = eb->info_.originalSize_;
			buf = malloc( size );
			npack_entity_read( eb, buf );
		}
		else
		{	// just copy
			size = eb->info_.size_;
			buf = malloc( size );
			npack_seek( pb->handle_, (long)eb->info_.offset_, SEEK_SET );

			if( npack_read( pb->handle_,
							buf,
							size,
							g_callbackfp,
							NPACK_PROCESSTYPE_ENTITY,
							g_callbackSize,
							eb->name_ ) != NPACK_SUCCESS )
				goto npack_entity_write_return_with_free;
			skipProcessing = true;
		}
	}

	if( !skipProcessing )
	{
		// Encode before compress, before v21
		if( ( eb->newflag_ & NPACK_ENTITY_ENCRYPT ) && !( eb->newflag_ & NPACK_ENTITY_REVERSE ) )
			tea_encode_buffer((char*)buf, (int)size, pb->teakey_ );

		if( eb->newflag_ & NPACK_ENTITY_COMPRESS )
		{
			if( size >= NPACK_MIN_SIZE_ZIPABLE )
			{
				compressedSize = sizeof(char) * size + 256; // +256 for safety
				buf_for_zlib = malloc( sizeof(char) * compressedSize );
				z_compress( (Bytef*)buf_for_zlib, (uLong*)&compressedSize, (const Bytef*)buf, (uLong)size );
				free( buf );
				buf = buf_for_zlib;
				buf_for_zlib = NULL;
				size = (unsigned int)compressedSize;
			}
		}

		// Encode after compress, after v21
		if( ( eb->newflag_ & NPACK_ENTITY_ENCRYPT ) && ( eb->newflag_ & NPACK_ENTITY_REVERSE ) )
			tea_encode_buffer((char*)buf, (int)size, pb->teakey_ );
	}

	eb->info_.size_ = size;
	eb->info_.offset_ = npack_tell( handle );
	if( npack_write( handle,
					buf,
					size,
					g_callbackfp,
					NPACK_PROCESSTYPE_ENTITY,
					g_callbackSize,
					eb->name_ ) != NPACK_SUCCESS )
		goto npack_entity_write_return_with_free;

	free( buf );

	eb->info_.flag_ = eb->newflag_;

	return NPACK_SUCCESS;

npack_entity_write_return_with_free:
	NPACK_SAFE_FREE( buf );
	NPACK_SAFE_FREE( buf_for_zlib );
	return g_npackError;
}

NPACK_RESULT npack_entity_export( NPACK_ENTITY entity, NPACK_CSTR filename, bool forceoverwrite )
{
	void* buf;
	NPACK_HANDLE handle;
	NPACK_ENTITYBODY* eb = entity;

	if( !entity )
		return npack_error( NPACK_ERROR_EntityIsNull );

	buf = malloc( eb->info_.originalSize_ );
	if( !buf )
		return npack_error( NPACK_ERROR_NotEnoughMemory );

	if( !npack_entity_read( eb, buf ) )
		return g_npackError;

	if( npack_open( &handle, filename, true, true ) != NPACK_SUCCESS )
	{
		if( !forceoverwrite )
			return g_npackError;
	
		if( npack_open( &handle, filename, true, false ) != NPACK_SUCCESS )
			return g_npackError;
	}

	if( npack_write( handle,
					buf,
					eb->info_.originalSize_,
					g_callbackfp,
					NPACK_PROCESSTYPE_ENTITY,
					g_callbackSize,
					eb->name_ ) != NPACK_SUCCESS )
		return g_npackError;

	if( npack_close( handle ) != NPACK_SUCCESS )
		return g_npackError;

	npack_set_filetime( filename, eb->info_.modified_ );

	free( buf );
	return NPACK_SUCCESS;
}


NPACK_RESULT npack_package_clear( NPACK_PACKAGE package )
{
	if( !package )
		return npack_error( NPACK_ERROR_PackageIsNull );

	if( npack_package_remove_all_entity( package ) != NPACK_SUCCESS )
		return g_npackError;

	if( npack_package_init( package ) != NPACK_SUCCESS )
		return g_npackError;

	return NPACK_SUCCESS;
}

NPACK_RESULT npack_package_new( NPACK_PACKAGE* lpPackage, NPACK_TEAKEY* teakey )
{
	NPACK_PACKAGEBODY* pb;

	if( teakey == NULL )
		return npack_error( NPACK_ERROR_NeedSpecifiedTeaKey );

	pb = malloc( sizeof(NPACK_PACKAGEBODY) );

	if( !pb )
		return npack_error( NPACK_ERROR_NotEnoughMemory );

	if( npack_package_init( pb ) != NPACK_SUCCESS )
	{
		NPACK_SAFE_FREE( pb );
		return g_npackError;
	}

	memcpy( pb->teakey_, teakey, sizeof(long) * 4 );

	*lpPackage = pb;
	return NPACK_SUCCESS;
}

NPACK_RESULT npack_package_save( NPACK_PACKAGE package, NPACK_CSTR filename, bool forceoverwrite )
{
	NPACK_PACKAGEBODY*	pb = package;
	NPACK_ENTITYBODY*	eb = NULL;
	bool				bUseTemporaryFile = false;
	NPACK_SIZE			len;
	int					savecount = 0;
	NPACK_STR			savefilename = NULL;
	int					savefilehandle;
#ifdef NPACK_PLATFORM_WINDOWS
	SYSTEMTIME			st;
	FILETIME			ft;
#endif
	NPACK_PACKAGEINFO_V23 header_v23;

	if( !package )
		return npack_error( NPACK_ERROR_PackageIsNull );

	if( !filename )
		return npack_error( NPACK_ERROR_PackageHasNoName );

	if( npack_open( &savefilehandle, filename, true, true ) == NPACK_SUCCESS )
	{
		npack_alloc_copy_string( &savefilename, filename );
	}
	else
	{
		if( g_npackError != NPACK_ERROR_FileAlreadyExists )
			return g_npackError;

		if( !forceoverwrite )
			return g_npackError;

		len = (NPACK_SIZE)strlen( filename );
		savefilename = malloc( sizeof(NPACK_CHAR)*(len+2) );
		if( savefilename == NULL )
			return( npack_error( NPACK_ERROR_NotEnoughMemory ) );

		strncpy( savefilename, filename, len );
		savefilename[len+0] = '_';
		savefilename[len+1] = '\0';
		bUseTemporaryFile = true;

		if( npack_open( &savefilehandle, savefilename, true, false ) != NPACK_SUCCESS )
			return g_npackError;
	}

	strncpy( pb->info_.signature_, NPACK_SIGNATURE, sizeof(NPACK_CHAR)*4 );
	pb->info_.version_ = NPACK_VERSION_CURRENT;
	pb->info_.entityDataOffset_ = sizeof(NPACK_PACKAGEINFO)
								+ sizeof(NPACK_PACKAGEINFO_V23);

	npack_seek( savefilehandle, (long)pb->info_.entityDataOffset_, SEEK_SET );

	eb = pb->pEntityHead_;
	while( eb != NULL )
	{
		if( g_callbackfp )
			if( (g_callbackfp)( NPACK_ACCESSTYPE_WRITE, NPACK_PROCESSTYPE_PACKAGE, filename, savecount, pb->info_.entityCount_ ) == false )
				return( npack_error( NPACK_ERROR_CancelByCallback ) );

		npack_entity_write( eb, savefilehandle );
		++savecount;
		eb = eb->next_;
	}

	pb->info_.entityInfoOffset_ = npack_tell( savefilehandle );
	pb->info_.entityCount_ = savecount;

	eb = pb->pEntityHead_;
	while( eb != NULL )
	{
		if( npack_write_encrypt( pb->teakey_,
								savefilehandle,
								&eb->info_,
								sizeof(NPACK_ENTITYINFO),
								g_callbackfp,
								NPACK_PROCESSTYPE_ENTITYHEADER,
								g_callbackSize,
								eb->name_ ) != NPACK_SUCCESS )
			return g_npackError;

		if( npack_write_encrypt( pb->teakey_,
								savefilehandle,
								eb->name_,
								sizeof(NPACK_CHAR)*eb->info_.nameLength_,
								g_callbackfp,
								NPACK_PROCESSTYPE_ENTITYHEADER,
								g_callbackSize,
								eb->name_ ) != NPACK_SUCCESS )
			return g_npackError;

		eb = eb->next_;
	}

	npack_seek( savefilehandle, 0, SEEK_SET );
	if( npack_write( savefilehandle,
					&pb->info_,
					sizeof(NPACK_PACKAGEINFO),
					g_callbackfp,
					NPACK_PROCESSTYPE_PACKAGEHEADER,
					g_callbackSize,
					savefilename ) != NPACK_SUCCESS )
		return g_npackError;

	// V23, Write the package timestamp for other applications
#ifdef NPACK_PLATFORM_WINDOWS
	GetLocalTime( &st );
	SystemTimeToFileTime( &st, &ft );
	npack_win32filetime_to_timet( (NPACK_64BIT*)&ft, &header_v23.modified_ );
#else
	time( (time_t*)&header_v23.modified_ );
#endif
	if( npack_write( savefilehandle,
					&header_v23,
					sizeof(NPACK_PACKAGEINFO_V23),
					g_callbackfp,
					NPACK_PROCESSTYPE_PACKAGEHEADER,
					g_callbackSize,
					savefilename ) != NPACK_SUCCESS )
		return g_npackError;

	npack_flush( savefilehandle );
	npack_close( pb->handle_ );

	if( bUseTemporaryFile )
	{
		npack_close( savefilehandle );
		remove( filename );
		rename( savefilename, filename );

		if( npack_open( &pb->handle_, filename, false, false ) != NPACK_SUCCESS )
			return g_npackError;
	}
	else
	{
        pb->handle_ = savefilehandle;
	}
	NPACK_SAFE_FREE( savefilename );
	return NPACK_SUCCESS;
}

NPACK_RESULT npack_package_add_file( NPACK_PACKAGE package, NPACK_CSTR filename, NPACK_CSTR entityname, NPACK_ENTITY* lpEntity )
{
	NPACK_ENTITYBODY* eb;
	NPACK_CSTR __entityname;

	if(	npack_entity_alloc( (NPACK_ENTITY*)&eb ) != NPACK_SUCCESS )
		return g_npackError;

	if( entityname == NULL )
	{
		if( ( entityname = strrchr( filename, '\\' ) ) == NULL )
			__entityname = filename;
		else
			__entityname = entityname + sizeof(NPACK_CHAR);
	}
	else
		__entityname = entityname;

	if( npack_get_filetime( filename, &eb->info_.modified_ ) != NPACK_SUCCESS )
		goto npack_package_add_file_return_with_error;

	if( npack_alloc_copy_string( &eb->localname_, filename ) != NPACK_SUCCESS )
		goto npack_package_add_file_return_with_error;

	if( npack_alloc_copy_string( &eb->name_, __entityname ) != NPACK_SUCCESS )
		goto npack_package_add_file_return_with_error;

	eb->info_.nameLength_ = (NPACK_SIZE)strlen( eb->name_ );

	if( npack_package_add_entity( package, eb ) != NPACK_SUCCESS )
		goto npack_package_add_file_return_with_error;

	if( lpEntity )
		*lpEntity = eb;

	return NPACK_SUCCESS;
npack_package_add_file_return_with_error:
	NPACK_SAFE_FREE( eb );
	return g_npackError;
}

NPACK_RESULT npack_package_remove_entity( NPACK_PACKAGE package, NPACK_ENTITY entity )
{
	NPACK_ENTITYBODY* eb = entity;
	NPACK_PACKAGEBODY* pb = package;

	if( !entity )
		return npack_error( NPACK_ERROR_EntityIsNull );
	if( !package )
		return npack_error( NPACK_ERROR_PackageIsNull );
	if( eb->owner_ != package )
		return npack_error( NPACK_ERROR_EntityIsNotInThePackage );

	if( eb->prev_ )
		eb->prev_->next_ = eb->next_;
	if( eb->next_ )
		eb->next_->prev_ = eb->prev_;

	if( eb == pb->pEntityHead_ )
		pb->pEntityHead_ = eb->next_;
	if( eb == pb->pEntityTail_ )
		pb->pEntityTail_ = eb->prev_;

	pb->pEntityLatest_ = eb->next_;
	--pb->info_.entityCount_;

	NPACK_SAFE_FREE( eb->name_ );
	NPACK_SAFE_FREE( eb->localname_ );
	NPACK_SAFE_FREE( eb );
	return NPACK_SUCCESS;
}

#endif /* NPACK_DEV */

