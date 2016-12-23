/** @file
  Interface definition details between MRC and platform drivers during PEI phase.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#ifndef _SA_POLICY_PPI_H_
#define _SA_POLICY_PPI_H_

#include <Uefi.h>
#include <ConfigBlock.h>
#include <Ppi/GraphicsConfig.h>
#include <Ppi/MemoryConfig.h>
#include <Ppi/IpuConfig.h>
#include <Ppi/SaMiscConfig.h>
#include <Ppi/HybridGraphicsConfig.h>
#include <Ppi/SaPreMemConfig.h>

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gSiSaPolicyPpiGuid;
extern EFI_GUID gSiSaPreMemPolicyPpiGuid;

/**
  PPI revision number
  Any backwards compatible changes to this PPI will result in an update in the revision number
  Major changes will require publication of a new PPI

**/
#define SA_POLICY_PPI_REVISION  1

//
// Generic definitions for device enabling/disabling used by NC code.
//
#define DEVICE_ENABLE   1
#define DEVICE_DISABLE  0

#ifndef MAX_SOCKETS
#define MAX_SOCKETS 4
#endif

#define S3_TIMING_DATA_LEN          9
#define S3_READ_TRAINING_DATA_LEN   16
#define S3_WRITE_TRAINING_DATA_LEN  12

#ifndef S3_RESTORE_DATA_LEN
#define S3_RESTORE_DATA_LEN (S3_TIMING_DATA_LEN + S3_READ_TRAINING_DATA_LEN + S3_WRITE_TRAINING_DATA_LEN)
#endif // S3_RESTORE_DATA_LEN

#pragma pack(1)


/**
  SI SA Policy PPI\n
  Each config block change history should be listed here\n\n

**/
typedef struct {
  CONFIG_BLOCK_TABLE_HEADER      TableHeader;    ///< Offset 0-31
/**
  Individual Config Block Structures are added here in memory as part of AddConfigBlock()

**/
} SI_SA_POLICY_PPI;

#pragma pack()

#endif // _SA_POLICY_PPI_H_

