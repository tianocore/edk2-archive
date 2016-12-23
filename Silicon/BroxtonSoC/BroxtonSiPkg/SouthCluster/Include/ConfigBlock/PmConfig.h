/** @file
  Power Management policy.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PM_CONFIG_H_
#define _PM_CONFIG_H_

#define PM_CONFIG_REVISION 2

extern EFI_GUID gPmConfigGuid;

#pragma pack (push,1)

/**
  This structure allows to customize wake up capability from wake events.

**/
typedef struct {
  UINT32  PmeB0S5Dis         :  1;
  UINT32  Rsvdbits           : 31;
} SC_WAKE_CONFIG;

/**
  The SC_PM_CONFIG block describes expected miscellaneous power management settings.
  The PowerResetStatusClear field would clear the Power/Reset status bits, please
  set the bits if you want PCH Init driver to clear it, if you want to check the
  status later then clear the bits.

**/
typedef struct {
  CONFIG_BLOCK_HEADER Header;       ///< Config Block Header
  SC_WAKE_CONFIG      WakeConfig;   ///< Specify Wake Policy
  UINT32              PciClockRun          :  1;
  UINT32              Timer8254ClkGateEn   :  1;
  UINT32              PwrBtnOverridePeriod :  3;
  UINT32              DisableNativePowerButton : 1;
  UINT32              PowerButterDebounceMode  : 1;
  UINT32              Rsvdbits                 :25;
} SC_PM_CONFIG;

#pragma pack (pop)

#endif // _PM_CONFIG_H_

