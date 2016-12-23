/** @file
  The Header file of the Pci Host Bridge Driver.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PCI_HOST_BRIDGE_H_
#define _PCI_HOST_BRIDGE_H_

//
// Disable the warning since LIST_ENTRY was defined in EDKII
// and EFI_LIST_ENTRY was defined in PCI_HOST_BRIDGE_INSTANCE
//
#ifndef __GNUC__
#pragma warning(disable: 4133)
#endif

//
// Hard code the host bridge number in the platform.
// In this chipset, there is only one host bridge.
//
#define HOST_BRIDGE_NUMBER        1

#define PCI_HOST_BRIDGE_SIGNATURE SIGNATURE_32 ('e', 'h', 's', 't')
typedef struct {
  UINTN                                             Signature;
  EFI_HANDLE                                        HostBridgeHandle;
  UINTN                                             RootBridgeNumber;
  LIST_ENTRY                                        Head;
  BOOLEAN                                           ResourceSubmited;
  BOOLEAN                                           CanRestarted;
  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  ResAlloc;
} PCI_HOST_BRIDGE_INSTANCE;

#define INSTANCE_FROM_RESOURCE_ALLOCATION_THIS(a) CR (a, PCI_HOST_BRIDGE_INSTANCE, ResAlloc, PCI_HOST_BRIDGE_SIGNATURE)

//
//  HostBridge Resource Allocation interface
//
/**
  Enter a certain phase of the PCI enumeration process.

  @param[in] This                     The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance
  @param[in] Phase                    The phase during enumeration.

  @retval    EFI_SUCCESS              Succeed.
  @retval    EFI_INVALID_PARAMETER    Wrong phase parameter passed in.
  @retval    EFI_NOT_READY            Resources have not been submitted yet.

**/
EFI_STATUS
EFIAPI
NotifyPhase (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase
  );

/**
  Return the device handle of the next PCI root bridge that is associated with
  this Host Bridge.

  @param[in]     This                     The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in,out] RootBridgeHandle         Returns the device handle of the next PCI Root Bridge.
                                          On input, it holds the RootBridgeHandle returned by the most
                                          recent call to GetNextRootBridge().The handle for the first
                                          PCI Root Bridge is returned if RootBridgeHandle is NULL on input.

  @retval        EFI_SUCCESS              Succeed.
  @retval        EFI_NOT_FOUND            Next PCI root bridge not found.
  @retval        EFI_INVALID_PARAMETER    Wrong parameter passed in.

**/
EFI_STATUS
EFIAPI
GetNextRootBridge (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN OUT EFI_HANDLE                                   *RootBridgeHandle
  );

/**
  Returns the attributes of a PCI Root Bridge.

  @param[in]  This                     The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in]  RootBridgeHandle         The device handle of the PCI Root Bridge.
                                       that the caller is interested in
  @param[out] Attributes               The pointer to attributes of the PCI Root Bridge.

  @retval     EFI_SUCCESS              Succeed.
  @retval     EFI_INVALID_PARAMETER    Attributes parameter passed in is NULL or
                                       RootBridgeHandle is not an EFI_HANDLE
                                       that was returned on a previous call to
                                       GetNextRootBridge().

**/
EFI_STATUS
EFIAPI
GetAttributes (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT UINT64                                          *Attributes
  );

/**
  This is the request from the PCI enumerator to set up
  the specified PCI Root Bridge for bus enumeration process.

  @param[in]  This                     The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in]  RootBridgeHandle         The PCI Root Bridge to be set up.
  @param[out] Configuration            Pointer to the pointer to the PCI bus resource descriptor.

  @retval     EFI_SUCCESS              Succeed.
  @retval     EFI_OUT_OF_RESOURCES     Not enough pool to be allocated.
  @retval     EFI_INVALID_PARAMETER    RootBridgeHandle is not a valid handle.

**/
EFI_STATUS
EFIAPI
StartBusEnumeration (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  );

/**
  This function programs the PCI Root Bridge hardware so that
  it decodes the specified PCI bus range.

  @param[in] This                     The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle         The PCI Root Bridge whose bus range is to be programmed.
  @param[in] Configuration            The pointer to the PCI bus resource descriptor.

  @retval    EFI_SUCCESS              Succeed.
  @retval    EFI_INVALID_PARAMETER    Wrong parameters passed in.

**/
EFI_STATUS
EFIAPI
SetBusNumbers (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  );

/**
  Submits the I/O and memory resource requirements for the specified PCI Root Bridge.

  @param[in] This                     The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle         The PCI Root Bridge whose I/O and memory resource requirements
                                      are being submitted.
  @param[in] Configuration            The pointer to the PCI I/O and PCI memory resource descriptor.

  @retval    EFI_SUCCESS              Succeed.
  @retval    EFI_INVALID_PARAMETER    Wrong parameters passed in.

**/
EFI_STATUS
EFIAPI
SubmitResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  );

/**
  This function returns the proposed resource settings for the specified.
  PCI Root Bridge.

  @param[in]  This                     The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in]  RootBridgeHandle         The PCI Root Bridge handle.
  @param[out] Configuration            The pointer to the pointer to the PCI I/O.
                                       and memory resource descriptor.

  @retval     EFI_SUCCESS              Succeed.
  @retval     EFI_OUT_OF_RESOURCES     Not enough pool to be allocated.
  @retval     EFI_INVALID_PARAMETER    RootBridgeHandle is not a valid handle.

**/
EFI_STATUS
EFIAPI
GetProposedResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  );

/**
  This function is called for all the PCI controllers that the PCI
  bus driver finds. Can be used to Preprogram the controller.

  @param[in] This                     The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in] RootBridgeHandle         The PCI Root Bridge handle.
  @param[in] PciAddress               Address of the controller on the PCI bus.
  @param[in] Phase                    The Phase during resource allocation.

  @retval    EFI_SUCCESS              Succeed.
  @retval    EFI_INVALID_PARAMETER    RootBridgeHandle is not a valid handle.

**/
EFI_STATUS
EFIAPI
PreprocessController (
  IN  struct _EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *This,
  IN  EFI_HANDLE                                                RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS               PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE              Phase
  );

/**
  Calculate max memory of power 2.

  @param[in]  MemoryLength              Input memory length.

  @retval Returned Maximum length.

**/
UINT64
Power2MaxMemory (
  IN UINT64                     MemoryLength
  );

#endif

