/*

    npk - neat package system
    See README for copyright and license information.

    base type and definitions of npk

*/
#include <stddef.h>
#include "npk_conf.h"

#ifndef _NPK_BASE_H_
#define _NPK_BASE_H_

// Type
typedef int                             NPK_RESULT;
typedef void*                           NPK_PACKAGE;
typedef void*                           NPK_ENTITY;
typedef ptrdiff_t                       NPK_HANDLE;
typedef unsigned int                    NPK_FLAG;
typedef unsigned int                    NPK_HASHKEY;
typedef int                             NPK_TEAKEY;
typedef char                            NPK_CHAR;
typedef const NPK_CHAR*                 NPK_CSTR;
typedef NPK_CHAR*                       NPK_STR;
typedef unsigned int                    NPK_SIZE;
typedef unsigned short                  NPK_NAMESIZE;
typedef char                            NPK_BYTE;
typedef unsigned long long              NPK_64BIT;
typedef int                             NPK_TIME;

// NPK
#define NPK_SIGNATURE                   "NPK!"
#define NPK_OLD_SIGNATURE               "NPAK"
#define NPK_SIGNATURE_LENGTH            4
#define NPK_MAXFOLDERNFILENAME          512
#define NPK_MIN_SIZE_ZIPABLE            256
#define NPK_HASH_BUCKETS                257

// Version Information
#define NPK_VERSION_REFACTORING         21
#define NPK_VERSION_UNIXTIMESUPPORT     22
#define NPK_VERSION_PACKAGETIMESTAMP    23
#define NPK_VERSION_SINGLEPACKHEADER    24
#define NPK_VERSION_ENCRYPTREMAINS      25
#define NPK_VERSION_USEXXTEAONHEADER    26
#define NPK_VERSION_STREAMABLE          27
#define NPK_VERSION_CURRENT             NPK_VERSION_STREAMABLE

// Entity Flag
#define NPK_ENTITY_NULL                 0
#define NPK_ENTITY_TEXTFILE             0x00000001  // NOT USED
#define NPK_ENTITY_ENCRYPT_XXTEA        0x00000010  // Encrypted by XXTEA
#define NPK_ENTITY_ENCRYPT_TEA          0x00000100  // Encrypted by TEA
#define NPK_ENTITY_COMPRESS_ZLIB        0x00001000  // Compressed by zlib
#define NPK_ENTITY_COMPRESS_BZIP2       0x00010000  // Compressed by bzip2
#define NPK_ENTITY_REVERSE              0x00100000  // Encryption & Compression order.(0=E->C,1=C->E)
#define NPK_ENTITY_INHERIT              0xF0000000

// Callback
#define NPK_ACCESSTYPE_READ             0
#define NPK_ACCESSTYPE_WRITE            1

#define NPK_PROCESSTYPE_FILE            0
#define NPK_PROCESSTYPE_ENTITYHEADER    1
#define NPK_PROCESSTYPE_ENTITY          2
#define NPK_PROCESSTYPE_PACKAGEHEADER   3
#define NPK_PROCESSTYPE_PACKAGE         4

typedef int( *NPK_CALLBACK )(   int accessType,
                                int processType,
                                NPK_CSTR identifier,
                                NPK_SIZE current,
                                NPK_SIZE total
                                );

#endif /* _NPK_BASE_H_ */
