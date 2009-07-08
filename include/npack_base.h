/*

	npack - General-Purpose File Packing Library
	Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)

	base type and definitions of npack

	See README for copyright and license information.

*/
#include "npack_conf.h"

#ifndef _NPACK_BASE_H_
#define _NPACK_BASE_H_

// Type
typedef int								NPACK_RESULT;
typedef void*							NPACK_PACKAGE;
typedef void*							NPACK_ENTITY;
typedef int								NPACK_HANDLE;
typedef unsigned int					NPACK_FLAG;
typedef long							NPACK_TEAKEY;
typedef char							NPACK_CHAR;
typedef const NPACK_CHAR*				NPACK_CSTR;
typedef NPACK_CHAR*						NPACK_STR;
typedef int								NPACK_SIZE;
typedef unsigned short					NPACK_NAMESIZE;
typedef char							NPACK_BYTE;
typedef unsigned long long				NPACK_64BIT;
typedef int								NPACK_TIME;

// NPACK
#define NPACK_SIGNATURE					"NPAK"
#define NPACK_SIGNATURE_LENGTH			4
#define NPACK_MAXFOLDERNFILENAME		512
#define NPACK_MIN_SIZE_ZIPABLE			256

// Version Information
#define NPACK_VERSION_COMPRESSION		3
#define NPACK_VERSION_ENCRYPTION		10
#define NPACK_VERSION_MANAGED			12
#define NPACK_VERSION_DYNAMIC_SIZE		16
#define NPACK_VERSION_HASTEAKEY			18
#define NPACK_VERSION_FIXTEAHACK		19
#define NPACK_VERSION_ENCRYPTEDHEADER	20
#define NPACK_VERSION_REFACTORING		21
#define NPACK_VERSION_UNIXTIMESUPPORT	22
#define NPACK_VERSION_PACKAGETIMESTAMP	23
#define NPACK_VERSION_CURRENT			NPACK_VERSION_PACKAGETIMESTAMP

// Entity Flag
#define NPACK_ENTITY_NULL				0
#define NPACK_ENTITY_TEXTFILE			0x00000001	// NOT USED
#define NPACK_ENTITY_ENCRYPT			0x00000100	// Encrypted.
#define NPACK_ENTITY_COMPRESS			0x00001000	// Zip-compressed.
#define NPACK_ENTITY_REVERSE			0x00100000	// Encryption & Compression order.(0=E->C,1=C->E)
#define NPACK_ENTITY_INHERIT			0xF0000000

// Callback
#define NPACK_ACCESSTYPE_READ			0
#define NPACK_ACCESSTYPE_WRITE			1

#define NPACK_PROCESSTYPE_FILE			0
#define NPACK_PROCESSTYPE_ENTITYHEADER	1
#define NPACK_PROCESSTYPE_ENTITY		2
#define NPACK_PROCESSTYPE_PACKAGEHEADER	3
#define NPACK_PROCESSTYPE_PACKAGE		4

typedef	int( *NPACK_CALLBACK )( int accessType,
								int processType,
								NPACK_CSTR identifier,
								NPACK_SIZE current,
								NPACK_SIZE total
								);

#endif /* _NPACK_BASE_H_ */
