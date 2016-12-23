/** @file
  Platform Trust Technology (FTPM) PTP (Platform TPM Profile) Device Library.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PTT_PTP_DEVICE_LIB_H_
#define _PTT_PTP_DEVICE_LIB_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <PttPtpRegs.h>

///
/// Default Timeout values
///
#define PTT_HCI_TIMEOUT_A         750  * 1000   ///< 750 ms
#define PTT_HCI_TIMEOUT_B         2000 * 1000   ///<   2 s
#define PTT_HCI_TIMEOUT_C         200  * 1000   ///< 200 ms
#define PTT_HCI_TIMEOUT_D         30   * 1000   ///< 30  ms
#define PTT_HCI_TIMEOUT_E         30 * 1000 * 1000   ///< 30 s
#define PTT_HCI_POLLING_PERIOD    140           ///< Poll register every 140 microsecondss

#define PTT_HCI_RESPONSE_HEADER_SIZE  12 ///< TPM2_RESPONSE_HEADER size
#define TPM_LOCALITY_BUFFER_SIZE        0x10000 ///< Locality Buffer Size

typedef enum {
  TPM_LOCALITY_0 = 0,
  TPM_LOCALITY_1 = 1,
  TPM_LOCALITY_2 = 2,
  TPM_LOCALITY_3 = 3,
  TPM_LOCALITY_4 = 4,
} TPM_LOCALITY_ENUM;

/**
  Checks whether FTPM is enabled (FTPM_STS::FTPM_EN).

  @param[in]  None

  @retval    TRUE    FTPM is enabled.
  @retval    FALSE   FTPM is disabled. All LT writes will be dropped.
                     All LT reads will be returned with read data value of all 0's.
                     The bit can only be written once per ME power cycle.

**/
BOOLEAN
EFIAPI
PttHciPresenceCheck (
  VOID
  );

BOOLEAN
EFIAPI
PttHciStartupExecuted (
  VOID
  );

EFI_STATUS
EFIAPI
PttHciRequestCommandExec (
  VOID
  );


EFI_STATUS
EFIAPI
PttHciWaitRegisterBits (
  IN      EFI_PHYSICAL_ADDRESS    RegAddress,
  IN      UINT32    BitSet,
  IN      UINT32    BitClear,
  IN      UINT32    TimeOut
  );

EFI_STATUS
EFIAPI
PttHciSend (
  IN     UINT8      *FtpmBuffer,
  IN     UINT32     DataLength
  );

EFI_STATUS
EFIAPI
PttHciReceive (
  OUT     UINT8     *FtpmBuffer,
  OUT     UINT32    *RespSize
  );

EFI_STATUS
EFIAPI
PttHciSubmitCommand (
  IN      UINT8     *InputBuffer,
  IN      UINT32    InputBufferSize,
  OUT     UINT8     *ReturnBuffer,
  OUT     UINT32    *ReturnBufferSize
  );

/**
  Checks whether PTT is Ready

  @param[in]  None

  @retval    TRUE    PTT is ready.
  @retval    FALSE   PTT is not ready

**/
BOOLEAN
EFIAPI
PttHciReadyCheck (
  VOID
  );

#endif

