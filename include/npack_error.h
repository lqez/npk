/*

	npack - General-Purpose File Packing Library
	Copyright (c) 2009 Park Hyun woo(ez@amiryo.com)

	npack error code

	See README for copyright and license information.

*/

#ifndef _NPACK_ERROR_H_
#define _NPACK_ERROR_H_

// Success
#define NPACK_SUCCESS											1


// Error
#define NPACK_ERROR												0
#define	NPACK_ERROR_FileNotFound								-1
#define	NPACK_ERROR_FileOpenError								-2
#define	NPACK_ERROR_FileSaveError								-3
#define	NPACK_ERROR_FileReadError								-4
#define	NPACK_ERROR_PermissionDenied							-5
#define	NPACK_ERROR_ReadOnlyFile								-6
#define	NPACK_ERROR_FailToGetFiletime							-7
#define	NPACK_ERROR_NotValidFileName							-8
#define	NPACK_ERROR_NotValidPackage								-9
#define	NPACK_ERROR_CannotFindEntity							-10
#define	NPACK_ERROR_CannotCreateFile							-11
#define	NPACK_ERROR_PackageHasNoName							-12
#define	NPACK_ERROR_PackageHasNoVersion							-13
#define	NPACK_ERROR_ZeroFileSize								-14
#define	NPACK_ERROR_SameEntityExist								-15
#define	NPACK_ERROR_FailToSetFiletime							-17
#define NPACK_ERROR_NotValidEntity								-18
#define	NPACK_ERROR_OpenedPackage								-25
#define	NPACK_ERROR_NotOpenedPackage							-26
#define	NPACK_ERROR_SamePackageExist							-27
#define	NPACK_ERROR_NonAllocatedBuffer							-28
#define	NPACK_ERROR_NullPointerBuffer							-29
#define	NPACK_ERROR_FailToDecompress							-30
#define	NPACK_ERROR_AlreadyAllocated							-31
#define	NPACK_ERROR_NonStaticPackage							-32
#define	NPACK_ERROR_NeedSpecifiedTeaKey							-33
#define NPACK_ERROR_EntityIsNull								-40
#define NPACK_ERROR_PackageIsNull								-41
#define NPACK_ERROR_EntityIsNotInThePackage						-42
#define NPACK_ERROR_CantReadCompressOrEncryptEntityByPartial	-43
#define NPACK_ERROR_SourceStringisNull							-44
#define NPACK_ERROR_CannotCopyToItself							-45
#define	NPACK_ERROR_FileAlreadyExists							-46
#define	NPACK_ERROR_NoEntityInPackage							-47
#define NPACK_ERROR_InvalidTeaKey								-50
#define	NPACK_ERROR_NotEnoughMemory								-100
#define	NPACK_ERROR_NotEnoughDiscSpace							-101
#define	NPACK_ERROR_CannotFindPackage							-103
#define	NPACK_ERROR_CancelByCallback							-200

#endif /*_NPACK_ERROR_H_*/

