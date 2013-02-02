/*

    npk - neat package system
    See README for copyright and license information.

    npk error code

*/

#ifndef _NPK_ERROR_H_
#define _NPK_ERROR_H_

// Success
#define NPK_SUCCESS                                              1


// Error
#define NPK_ERROR                                                0
#define NPK_ERROR_FileNotFound                                   -1
#define NPK_ERROR_FileOpenError                                  -2
#define NPK_ERROR_FileSaveError                                  -3
#define NPK_ERROR_FileReadError                                  -4
#define NPK_ERROR_PermissionDenied                               -5
#define NPK_ERROR_ReadOnlyFile                                   -6
#define NPK_ERROR_FailToGetFiletime                              -7
#define NPK_ERROR_NotValidFileName                               -8
#define NPK_ERROR_NotValidPackage                                -9
#define NPK_ERROR_CannotFindEntity                               -10
#define NPK_ERROR_CannotCreateFile                               -11
#define NPK_ERROR_PackageHasNoName                               -12
#define NPK_ERROR_PackageHasNoVersion                            -13
#define NPK_ERROR_ZeroFileSize                                   -14
#define NPK_ERROR_SameEntityExist                                -15
#define NPK_ERROR_FailToSetFiletime                              -17
#define NPK_ERROR_NotValidEntity                                 -18
#define NPK_ERROR_NotValidEntityName                             -20
#define NPK_ERROR_OpenedPackage                                  -25
#define NPK_ERROR_NotOpenedPackage                               -26
#define NPK_ERROR_SamePackageExist                               -27
#define NPK_ERROR_NonAllocatedBuffer                             -28
#define NPK_ERROR_NullPointerBuffer                              -29
#define NPK_ERROR_FailToDecompress                               -30
#define NPK_ERROR_AlreadyAllocated                               -31
#define NPK_ERROR_NonStaticPackage                               -32
#define NPK_ERROR_NeedSpecifiedTeaKey                            -33
#define NPK_ERROR_EntityIsNull                                   -40
#define NPK_ERROR_PackageIsNull                                  -41
#define NPK_ERROR_EntityIsNotInThePackage                        -42
#define NPK_ERROR_CantReadCompressedEntityByPartial              -43
#define NPK_ERROR_SourceStringisNull                             -44
#define NPK_ERROR_CannotCopyToItself                             -45
#define NPK_ERROR_FileAlreadyExists                              -46
#define NPK_ERROR_NoEntityInPackage                              -47
#define NPK_ERROR_ReadingEncryptedEntityByPartialShouldBeAligned -48
#define NPK_ERROR_InvalidTeaKey                                  -50
#define NPK_ERROR_EntityIsNotReady                               -51
#define NPK_ERROR_PackageIsNotReady                              -52
#define NPK_ERROR_NotEnoughMemory                                -100
#define NPK_ERROR_NotEnoughDiscSpace                             -101
#define NPK_ERROR_CannotFindPackage                              -103
#define NPK_ERROR_CancelByCallback                               -200
#define NPK_ERROR_NotSupportedVersion                            -255

#endif /*_NPK_ERROR_H_*/

