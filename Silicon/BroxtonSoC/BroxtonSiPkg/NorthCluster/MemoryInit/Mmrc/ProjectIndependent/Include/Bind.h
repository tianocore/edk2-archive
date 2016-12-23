/** @file
  Data type definition for MRC.

  Copyright (c) 2005 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _BIND_H_
#define _BIND_H_
#pragma warning (disable : 4142 )

#pragma pack()

#if _MSC_EXTENSIONS

//
// Disable warning that make it impossible to compile at /W4
// This only works for Microsoft* tools
//

//
// Disabling bitfield type checking warnings.
//
#pragma warning (disable : 4214 )

//
// Disabling  warnings.
//
#pragma warning (disable : 4213)

//
// Disabling the unreferenced formal parameter warnings.
//
#pragma warning (disable : 4100 )

//
// Disable slightly different base types warning as CHAR8 * can not be set
// to a constant string.
//
#pragma warning (disable : 4057 )

//
// ASSERT(FALSE) or while (TRUE) are legal constructes so supress this warning
//
#pragma warning (disable : 4127 )

//
// Int64ShllMod32 unreferenced inline function
//
#pragma warning (disable : 4514 )

//
// Unreferenced formal parameter - We are object oriented, so we pass This even
//  if we  don't need them.
//
#pragma warning (disable : 4100 )

#endif

#if (__STDC_VERSION__ < 199901L)
//
// No ANSI C 2000 stdint.h integer width declarations, so define equivalents
//
#if _MSC_EXTENSIONS
//
// use Microsoft* C complier dependent interger width types
//
#if defined(EFI64) || defined(EFIx64)
typedef unsigned __int64    uint64_t;
typedef __int64             int64_t;
#endif

typedef unsigned long       uint32_t;
typedef long                int32_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned char       uint8_t;
typedef char                int8_t;
#if SIM

typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned char       uint8_t;
typedef char                my_int8_t;
#else
//
// Assume standard IA-32 alignment.
// BugBug: Need to check portability of long long
//
//    typedef unsigned long long  uint64_t;
//    typedef long long           int64_t;
typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned char       uint8_t;
typedef char                int8_t;
#endif
#else
//
// Linux
//
#if !defined SIM
typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned char       uint8_t;
typedef char                my_int8_t;
typedef char                int8_t;
#endif
#endif
#else
//
// Use ANSI C 2000 stdint.h integer width declarations
//
#include "stdint.h"
#endif

#if SIM

typedef unsigned int        uint32_t;
typedef int                 int32_t;
typedef unsigned short      uint16_t;
typedef short               int16_t;
typedef unsigned char       uint8_t;
typedef char                my_int8_t;
#endif
//
// Native integer size in stdint.h
//
#if defined(EFI64) || defined(EFIx64)
typedef int64_t intn_t;
typedef uint64_t uintn_t;
#else
typedef uint32_t  uintn_t;
typedef int32_t   intn_t;
#endif

#endif

