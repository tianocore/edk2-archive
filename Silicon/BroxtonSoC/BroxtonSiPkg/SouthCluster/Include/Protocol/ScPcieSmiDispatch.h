/** @file
  APIs of PCH PCIE SMI Dispatch Protocol.

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_PCIE_SMI_DISPATCH_PROTOCOL_H_
#define _SC_PCIE_SMI_DISPATCH_PROTOCOL_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                         gScPcieSmiDispatchProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _SC_PCIE_SMI_DISPATCH_PROTOCOL    SC_PCIE_SMI_DISPATCH_PROTOCOL;

//
// Member functions
//

typedef struct {
  UINT8                                 RpIndex; ///< Root port index (0-based), 0: RP1, 1: RP2, n: RP(N+1)
  UINT8                                 BusNum;  ///< Root port pci bus number
  UINT8                                 DevNum;  ///< Root port pci device number
  UINT8                                 FuncNum; ///< Root port pci function number
} SC_PCIE_SMI_RP_CONTEXT;

/**
  Callback function for an PCH PCIE RP SMI handler dispatch.

  @param[in] DispatchHandle             The unique handle assigned to this handler by register function.
  @param[in] RpContext                  Pointer of PCH PCIE Root Port context.

**/
typedef
VOID
(EFIAPI *SC_PCIE_SMI_RP_DISPATCH_CALLBACK) (
  IN EFI_HANDLE                         DispatchHandle,
  IN SC_PCIE_SMI_RP_CONTEXT             *RpContext
  );

/**
  Register a child SMI source dispatch function for PCH PCIERP SMI events.

  @param[in]  This                      Protocol instance pointer.
  @param[in]  DispatchFunction          Pointer to dispatch function to be invoked for
                                        this SMI source
  @param[in]  RpIndex                   Refer to PCH PCIE Root Port index.
                                        0: RP1, 1: RP2, n: RP(N+1)
  @param[out] DispatchHandle            Handle of dispatch function, for when interfacing
                                        with the parent SMM driver.

  @retval     EFI_SUCCESS               The dispatch function has been successfully
                                        registered and the SMI source has been enabled.
  @retval     EFI_DEVICE_ERROR          The driver was unable to enable the SMI source.
  @retval     EFI_OUT_OF_RESOURCES      Not enough memory (system or SMM) to manage this child.
  @retval     EFI_ACCESS_DENIED         Return access denied if the EndOfDxe event has been triggered.

**/
typedef
EFI_STATUS
(EFIAPI *SC_PCIE_SMI_RP_DISPATCH_REGISTER) (
  IN  SC_PCIE_SMI_DISPATCH_PROTOCOL     *This,
  IN  SC_PCIE_SMI_RP_DISPATCH_CALLBACK  DispatchFunction,
  IN  UINTN                             RpIndex,
  OUT EFI_HANDLE                        *DispatchHandle
  );

/**
  Unregister a child SMI source dispatch function with a parent PCIE SMM driver.

  @param[in] This                       Protocol instance pointer.
  @param[in] DispatchHandle             Handle of dispatch function to deregister.

  @retval    EFI_SUCCESS                The dispatch function has been successfully
                                        unregistered and the SMI source has been disabled
                                        if there are no other registered child dispatch
                                        functions for this SMI source.
  @retval    EFI_INVALID_PARAMETER      Handle is invalid.
  @retval    EFI_ACCESS_DENIED          Return access denied if the EndOfDxe event has been triggered.

**/
typedef
EFI_STATUS
(EFIAPI *SC_PCIE_SMI_DISPATCH_UNREGISTER) (
  IN  SC_PCIE_SMI_DISPATCH_PROTOCOL     *This,
  IN  EFI_HANDLE                        DispatchHandle
  );

struct _SC_PCIE_SMI_DISPATCH_PROTOCOL {
  ///
  /// Smi unregister function for PCH PCIE SMI DISPATCH PROTOCOL.
  ///
  SC_PCIE_SMI_DISPATCH_UNREGISTER      UnRegister;
  ///
  /// PcieRpXHotPlug
  /// The event is triggered when PCIE root port Hot-Plug Presence Detect.
  ///
  SC_PCIE_SMI_RP_DISPATCH_REGISTER     HotPlugRegister;
  ///
  /// PcieRpXLinkActive
  /// The event is triggered when Hot-Plug Link Active State Changed.
  ///
  SC_PCIE_SMI_RP_DISPATCH_REGISTER     LinkActiveRegister;
  ///
  /// PcieRpXLinkEq
  /// The event is triggered when Device Requests Software Link Equalization.
  ///
  SC_PCIE_SMI_RP_DISPATCH_REGISTER     LinkEqRegister;
};

#endif

