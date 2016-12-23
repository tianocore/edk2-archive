/** @file
  PCH SMM IO Trap Control Protocol.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SMM_IO_TRAP_CONTROL_H_
#define _SC_SMM_IO_TRAP_CONTROL_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                                   gScSmmIoTrapControlGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _SC_SMM_IO_TRAP_CONTROL_PROTOCOL  SC_SMM_IO_TRAP_CONTROL_PROTOCOL;

//
// Related Definitions
//

//
// Member functions
//

/**
  The Prototype of Pause and Resume IoTrap callback function.

  @param[in] This                    Pointer to the PCH_SMM_IO_TRAP_CONTROL_PROTOCOL instance.
  @param[in] DispatchHandle          Handle of the child service to change state.

  @retval    EFI_SUCCESS             This operation is complete.
  @retval    EFI_INVALID_PARAMETER   The DispatchHandle is invalid.
  @retval    EFI_ACCESS_DENIED       The SMI status is alrady PAUSED/RESUMED.

**/
typedef
EFI_STATUS
(EFIAPI *SC_SMM_IO_TRAP_CONTROL_FUNCTION) (
  IN SC_SMM_IO_TRAP_CONTROL_PROTOCOL           * This,
  IN EFI_HANDLE                                 DispatchHandle
  );


struct _SC_SMM_IO_TRAP_CONTROL_PROTOCOL {
  SC_SMM_IO_TRAP_CONTROL_FUNCTION      Pause;
  SC_SMM_IO_TRAP_CONTROL_FUNCTION      Resume;
};

#endif

