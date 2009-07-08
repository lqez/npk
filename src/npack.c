/*

	npack - General-Purpose File Packing Library
	Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)

	See README for copyright and license information.

*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "npack.h"
#include "npack_dev.h"
#ifdef NPACK_PLATFORM_MACOS 
#include <strings.h>
#else
#include <string.h>
#endif
#include "../external/tea/tea.h"
#include "../external/zlib/zlib.h"


NPACK_API int		g_npackError = 0;
#ifdef NPACK_PLATFORM_WINDOWS
int					g_useCriticalSection = 0;
#pragma warning( disable : 4996 )
#endif

NPACK_CALLBACK		g_callbackfp;
NPACK_SIZE			g_callbackSize;

NPACK_PACKAGE npack_package_open( NPACK_CSTR filename, NPACK_TEAKEY* teakey )
{
	NPACK_CHAR				buf[512];
	NPACK_PACKAGEBODY*		pb			= NULL;
	NPACK_ENTITYBODY*		eb			= NULL;
	NPACK_ENTITYINFO_V21	oldinfo;
	NPACK_SIZE				entityCount	= 0;

	pb = malloc( sizeof(NPACK_PACKAGEBODY) );

	if( !pb )
	{
		npack_error( NPACK_ERROR_NotEnoughMemory );
		goto npack_package_open_return_null_with_free;
	}

	if( npack_package_init( pb ) != NPACK_SUCCESS )
		goto npack_package_open_return_null_with_free;

	if( npack_open( &pb->handle_, filename, false, false ) != NPACK_SUCCESS )
		goto npack_package_open_return_null_with_free;

	// Read common header
	if( npack_read( pb->handle_,
					(void*)&pb->info_,
					sizeof(NPACK_PACKAGEINFO),
					g_callbackfp,
					NPACK_PROCESSTYPE_PACKAGEHEADER,
					g_callbackSize,
					filename ) != NPACK_SUCCESS )
		goto npack_package_open_return_null_with_free;

	if( strncmp( pb->info_.signature_, NPACK_SIGNATURE, 4 ) != 0 )
		goto npack_package_open_return_null_with_free;

	// version 18 / read own tea key
	if( pb->info_.version_ == NPACK_VERSION_HASTEAKEY )
	{
		if( npack_read( pb->handle_,
						(void*)&pb->teakey_,
						sizeof(NPACK_PACKAGEINFO_V18),
						g_callbackfp,
						NPACK_PROCESSTYPE_PACKAGEHEADER,
						g_callbackSize,
						filename ) != NPACK_SUCCESS )
			goto npack_package_open_return_null_with_free;
	}
	else if( pb->info_.version_ >= NPACK_VERSION_FIXTEAHACK )
	{
		if( teakey == NULL )
		{
			npack_error( NPACK_ERROR_NeedSpecifiedTeaKey );
			goto npack_package_open_return_null_with_free;
		}
		memcpy( pb->teakey_, teakey, sizeof(long) * 4 );
	}

	// version 23 / package timestamp
	if( pb->info_.version_ >= NPACK_VERSION_PACKAGETIMESTAMP )
	{
		NPACK_TIME packageTimestamp;

		if( npack_read( pb->handle_,
						(void*)&packageTimestamp,
						sizeof(NPACK_PACKAGEINFO_V23),
						g_callbackfp,
						NPACK_PROCESSTYPE_PACKAGEHEADER,
						g_callbackSize,
						filename ) != NPACK_SUCCESS )
			goto npack_package_open_return_null_with_free;
	}

	entityCount = pb->info_.entityCount_;
	pb->info_.entityCount_ = 0;

	npack_seek( pb->handle_, (long)pb->info_.entityInfoOffset_, SEEK_SET );

	while( entityCount > 0 )
	{
		--entityCount;

		if(	npack_entity_alloc( (NPACK_ENTITY*)&eb ) != NPACK_SUCCESS )
			goto npack_package_open_return_null_with_free;

		eb->owner_ = pb;

		// read entity info
		if( pb->info_.version_ >= NPACK_VERSION_ENCRYPTEDHEADER )
		{
			if( pb->info_.version_ < NPACK_VERSION_UNIXTIMESUPPORT )
			{
				if( npack_read_encrypt( teakey,
										pb->handle_,
										(void*)&oldinfo,
										sizeof(NPACK_ENTITYINFO),
										g_callbackfp,
										NPACK_PROCESSTYPE_ENTITYHEADER,
										g_callbackSize,
										filename ) != NPACK_SUCCESS )
					goto npack_package_open_return_null_with_free;

				eb->info_.offset_ = oldinfo.offset_;
				eb->info_.size_ = oldinfo.size_;
				eb->info_.originalSize_ = oldinfo.originalSize_;
				eb->info_.flag_ = oldinfo.flag_;
				npack_win32filetime_to_timet( &oldinfo.modified_, &eb->info_.modified_ );
				eb->info_.nameLength_ = oldinfo.nameLength_;
			}
			else
			{
				if( npack_read_encrypt( teakey,
										pb->handle_,
										(void*)&eb->info_,
										sizeof(NPACK_ENTITYINFO),
										g_callbackfp,
										NPACK_PROCESSTYPE_ENTITYHEADER,
										g_callbackSize,
										filename ) != NPACK_SUCCESS )
					goto npack_package_open_return_null_with_free;
			}

			if( eb->info_.offset_ >= pb->info_.entityInfoOffset_ )
			{
				npack_error( NPACK_ERROR_InvalidTeaKey );
				goto npack_package_open_return_null_with_free;
			}

			if( npack_read_encrypt( teakey,
									pb->handle_,
									(void*)buf,
									sizeof(char) * eb->info_.nameLength_,
									g_callbackfp,
									NPACK_PROCESSTYPE_ENTITYHEADER,
									g_callbackSize,
									filename ) != NPACK_SUCCESS )
				goto npack_package_open_return_null_with_free;
		}
		else
		{
			if( npack_read( pb->handle_,
							(void*)&oldinfo,
							sizeof(NPACK_ENTITYINFO_V21),
							g_callbackfp,
							NPACK_PROCESSTYPE_ENTITYHEADER,
							g_callbackSize,
							filename ) != NPACK_SUCCESS )
				goto npack_package_open_return_null_with_free;

			eb->info_.offset_ = oldinfo.offset_;
			eb->info_.size_ = oldinfo.size_;
			eb->info_.originalSize_ = oldinfo.originalSize_;
			eb->info_.flag_ = oldinfo.flag_;
			npack_win32filetime_to_timet( &oldinfo.modified_, &eb->info_.modified_ );
			eb->info_.nameLength_ = oldinfo.nameLength_;

			if( npack_read( pb->handle_,
							(void*)buf,
							sizeof(char) * eb->info_.nameLength_,
							g_callbackfp,
							NPACK_PROCESSTYPE_ENTITYHEADER,
							g_callbackSize,
							filename ) != NPACK_SUCCESS )
				goto npack_package_open_return_null_with_free;
		}
		eb->newflag_ = eb->info_.flag_;

		// copy name into entity body
		buf[eb->info_.nameLength_] = '\0';
		if( npack_alloc_copy_string( &eb->name_, buf ) != NPACK_SUCCESS )
			goto npack_package_open_return_null_with_free;

		npack_package_add_entity( pb, eb );
	}

	/*
	if( entityCount != pb->info_.entityCount_ )
	{
		npack_error( NPACK_ERROR_NotValidPackage );
		goto npack_package_open_return_null_with_free;
	}
	*/
	return (NPACK_PACKAGE*)pb;

npack_package_open_return_null_with_free:
	if( pb )
		if( pb->handle_ > 0 )
			npack_close( pb->handle_ );

	NPACK_SAFE_FREE( eb );
	NPACK_SAFE_FREE( pb );
	return NULL;
}

bool npack_package_close( NPACK_PACKAGE package )
{
	NPACK_PACKAGEBODY* pb = (NPACK_PACKAGEBODY*)package;
	if( !package )
	{
		npack_error( NPACK_ERROR_PackageIsNull );
		return false;
	}

	if( NPACK_SUCCESS != npack_package_remove_all_entity( pb ) )
		return g_npackError;

#ifdef NPACK_PLATFORM_WINDOWS
	DeleteCriticalSection( &pb->cs_ );
#endif

	npack_close( pb->handle_ );

	NPACK_SAFE_FREE( pb );
	return true;
}

NPACK_ENTITY npack_package_get_entity( NPACK_PACKAGE package, NPACK_CSTR entityname )
{
	NPACK_ENTITYBODY* eb = NULL;
	NPACK_PACKAGEBODY* pb = package;

	if( !package )
	{
		npack_error( NPACK_ERROR_PackageIsNull );
		return NULL;
	}

	eb = pb->pEntityHead_;
	if( eb == NULL )
	{
		/* This is not an error. Package is just empty.*/
		return NULL;
	}

	while( eb != NULL )
	{
#ifdef NPACK_PLATFORM_WINDOWS
		if( stricmp( eb->name_, entityname ) == 0 )
#else
		if( strcasecmp( eb->name_, entityname ) == 0 )
#endif
		{
			pb->pEntityLatest_ = eb;
			return eb;
		}
		eb = eb->next_;
	}
	npack_error( NPACK_ERROR_CannotFindEntity );
	return NULL;
}

NPACK_SIZE npack_entity_get_size( NPACK_ENTITY entity )
{
	NPACK_ENTITYBODY* eb = entity;
	if( !entity )
	{
		npack_error( NPACK_ERROR_EntityIsNull );
		return 0;
	}
	return eb->info_.originalSize_;
}

NPACK_CSTR npack_entity_get_name( NPACK_ENTITY entity )
{
	NPACK_ENTITYBODY* eb = entity;
	if( !entity )
	{
		npack_error( NPACK_ERROR_EntityIsNull );
		return 0;
	}
	return eb->name_;
}

bool npack_entity_read( NPACK_ENTITY entity, void* buf )
{
	NPACK_ENTITYBODY* eb = entity;
	NPACK_PACKAGEBODY* pb = NULL;
	void** lplpTarget = &buf;
	void* lpDecompressBuffer = NULL;
	NPACK_SIZE uncompLen = 0;
	NPACK_RESULT res;

	if( !entity )
	{
		npack_error( NPACK_ERROR_EntityIsNull );
		return false;
	}

	if( eb->info_.flag_ & NPACK_ENTITY_COMPRESS )
	{
		lpDecompressBuffer = malloc( sizeof(char) * eb->info_.size_ );
		lplpTarget = &lpDecompressBuffer;
	}

	pb = eb->owner_;
#ifdef NPACK_PLATFORM_WINDOWS
	if( g_useCriticalSection )
		EnterCriticalSection( &pb->cs_ );
#endif
	npack_seek( pb->handle_, (long)eb->info_.offset_, SEEK_SET );

	res = npack_read( pb->handle_,
					(*lplpTarget),
					eb->info_.size_,
					g_callbackfp,
					NPACK_PROCESSTYPE_ENTITY,
					g_callbackSize,
					eb->name_ );
#ifdef NPACK_PLATFORM_WINDOWS
	if( g_useCriticalSection )
		LeaveCriticalSection( &pb->cs_ );
#endif

	if( res != NPACK_SUCCESS )
		goto npack_entity_read_return_null_with_free;

	// Decode before uncompress, after v21
	if( ( eb->info_.flag_ & NPACK_ENTITY_ENCRYPT ) && ( eb->info_.flag_ & NPACK_ENTITY_REVERSE ) )
		tea_decode_buffer((char*)(*lplpTarget), eb->info_.size_, pb->teakey_);

	if( eb->info_.flag_ & NPACK_ENTITY_COMPRESS )
	{
		uncompLen = eb->info_.originalSize_;

		if( uncompLen >= NPACK_MIN_SIZE_ZIPABLE )
		{
			if( Z_OK != z_uncompress((Bytef*)(buf), (z_uLong*)&uncompLen, (const Bytef*)lpDecompressBuffer, (z_uLong)eb->info_.size_ ) )
			{
				npack_error( NPACK_ERROR_FailToDecompress );
				goto npack_entity_read_return_null_with_free;
			}

			if( eb->info_.originalSize_ != uncompLen )
			{
				npack_error( NPACK_ERROR_FailToDecompress );
				goto npack_entity_read_return_null_with_free;
			}
		}
		else
			memcpy( buf, lpDecompressBuffer, eb->info_.size_ );

		NPACK_SAFE_FREE( lpDecompressBuffer );
		lplpTarget = &buf;
	}

	// Decode after uncompress, before v21
	if( ( eb->info_.flag_ & NPACK_ENTITY_ENCRYPT ) && !( eb->info_.flag_ & NPACK_ENTITY_REVERSE ) )
		tea_decode_buffer((char*)(*lplpTarget), eb->info_.originalSize_, pb->teakey_);

	return true;

npack_entity_read_return_null_with_free:
	NPACK_SAFE_FREE( lpDecompressBuffer );
	return false;
}

bool npack_entity_read_partial( NPACK_ENTITY entity, void* buf, NPACK_SIZE offset, NPACK_SIZE size )
{
	NPACK_ENTITYBODY* eb = entity;
	NPACK_PACKAGEBODY* pb = NULL;
	NPACK_RESULT res;

	if( !entity )
	{
		npack_error( NPACK_ERROR_EntityIsNull );
		return false;
	}

	if( eb->info_.flag_ & ( NPACK_ENTITY_COMPRESS | NPACK_ENTITY_ENCRYPT ) )
	{
		npack_error( NPACK_ERROR_CantReadCompressOrEncryptEntityByPartial );
		return false;
	}

	pb = eb->owner_;
#ifdef NPACK_PLATFORM_WINDOWS
	if( g_useCriticalSection )
		EnterCriticalSection( &pb->cs_ );
#endif
	npack_seek( pb->handle_, (long)(eb->info_.offset_ + offset), SEEK_SET );

	res = npack_read( pb->handle_,
					buf,
					size,
					g_callbackfp,
					NPACK_PROCESSTYPE_ENTITY,
					g_callbackSize,
					eb->name_ );
#ifdef NPACK_PLATFORM_WINDOWS
	if( g_useCriticalSection )
		LeaveCriticalSection( &pb->cs_ );
#endif

	if( res != NPACK_SUCCESS )
		return false;

	return true;
}

void npack_enable_callback( NPACK_CALLBACK cb, NPACK_SIZE cb_size )
{
	g_callbackfp = cb;
	g_callbackSize = cb_size;
}

void npack_disable_callback()
{
	g_callbackfp = NULL;
}

#ifdef NPACK_PLATFORM_WINDOWS
void npack_enable_criticalsection()
{
	g_useCriticalSection = 1;
}

void npack_disable_criticalsection()
{
	g_useCriticalSection = 0;
}
#endif

NPACK_ENTITY npack_package_get_first_entity( NPACK_PACKAGE package )
{
	NPACK_PACKAGEBODY* pb = package;

	if( !package )
	{
		npack_error( NPACK_ERROR_PackageIsNull );
		return NULL;
	}

	return pb->pEntityHead_;
}

NPACK_ENTITY npack_entity_next( NPACK_ENTITY entity )
{
	NPACK_ENTITYBODY* eb = entity;

	if( !entity )
	{
		npack_error( NPACK_ERROR_EntityIsNull );
		return NULL;
	}

	return eb->next_;
}
