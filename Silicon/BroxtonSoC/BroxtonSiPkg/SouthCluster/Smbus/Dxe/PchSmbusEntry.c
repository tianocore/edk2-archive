/** @file
  PCH SMBus Driver.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "PchSmbus.h"
#include <Library/ConfigBlockLib.h>

extern EFI_GUID gEfiSmbusArpMapGuid;

SMBUS_INSTANCE  *mSmbusContext;

/**
  Execute an SMBus operation

  @param[in] This                 The protocol instance
  @param[in] SlaveAddress         The address of the SMBUS slave device
  @param[in] Command              The SMBUS command
  @param[in] Operation            Which SMBus protocol will be issued
  @param[in] PecCheck             If Packet Error Code Checking is to be used
  @param[in, out] Length          Length of data
  @param[in, out] Buffer          Data buffer

  @retval EFI_SUCCESS             The SMBUS operation is successful
  @retval Others                  Something error occurred
**/
EFI_STATUS
EFIAPI
SmbusExecute (
  IN      CONST EFI_SMBUS_HC_PROTOCOL   *This,
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND      Command,
  IN      EFI_SMBUS_OPERATION           Operation,
  IN      BOOLEAN                       PecCheck,
  IN OUT  UINTN                         *Length,
  IN OUT  VOID                          *Buffer
  )
{
  DEBUG ((DEBUG_INFO, "SmbusExecute() Start, SmbusDeviceAddress=%x, Command=%x, Operation=%x\n", (SlaveAddress.SmbusDeviceAddress << 1), Command, Operation));
  InitializeSmbusRegisters ();

  return SmbusExec (
          SlaveAddress,
          Command,
          Operation,
          PecCheck,
          Length,
          Buffer
          );
}

/**
  <b>SMBus DXE Module Entry Point</b>\n
  - <b>Introduction</b>\n
    The SMBus module is a DXE driver that provides a standard way to execute an SMBus command.

  - @pre
    - @link SC_POLICY SC_POLICY_HOB @endlink
      - This module uses the parameters in SmbusConfig for platform reserved (non ARP capable) addresses.

  - @result
    The SMBus driver produces EFI_SMBUS_HC_PROTOCOL which is documented in the SMBus Host Controller
    Protocol Specification.

  @param[in] ImageHandle          ImageHandle of this module
  @param[in] SystemTable          EFI System Table

  @retval EFI_SUCCESS             Driver initializes successfully
  @retval Others                  Some error occurred
**/
EFI_STATUS
EFIAPI
InitializeScSmbus (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable
  )
{
  SC_POLICY_HOB           *ScPolicy;
  EFI_STATUS               Status;
  UINTN                    DataSize;
  VOID                     *Data;
  EFI_PEI_HOB_POINTERS     HobList;
  SC_SMBUS_CONFIG          *SmbusConfig;

  DEBUG ((DEBUG_INFO, "InitializeScSmbus() Start\n"));

  HobList.Guid = GetFirstGuidHob (&gScPolicyHobGuid);
  ASSERT (HobList.Guid != NULL);
  ScPolicy = GET_GUID_HOB_DATA (HobList.Guid);
  Status   = GetConfigBlock ((VOID *) ScPolicy, &gSmbusConfigGuid, (VOID *) &SmbusConfig);
  ASSERT_EFI_ERROR (Status);

  mSmbusContext = AllocateZeroPool (sizeof (SMBUS_INSTANCE));
  if (mSmbusContext == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  mSmbusContext->Signature                  = PCH_SMBUS_PRIVATE_DATA_SIGNATURE;
  mSmbusContext->IoDone                     = IoDone;
  mSmbusContext->SmbusIoRead                = SmbusIoRead;
  mSmbusContext->SmbusIoWrite               = SmbusIoWrite;
  mSmbusContext->SmbusController.Execute    = SmbusExecute;
  mSmbusContext->SmbusController.ArpDevice  = SmbusArpDevice;
  mSmbusContext->SmbusController.GetArpMap  = SmbusGetArpMap;
  mSmbusContext->SmbusController.Notify     = SmbusNotify;
  mSmbusContext->PlatformNumRsvd            = SmbusConfig->NumRsvdSmbusAddresses;
  mSmbusContext->PlatformRsvdAddr           = SmbusConfig->RsvdSmbusAddressTable;
  ASSERT (mSmbusContext->PlatformNumRsvd <= SC_MAX_SMBUS_RESERVED_ADDRESS);

  //
  // See if PEI already ARPed any devices, and if so, update our device map.
  //
  // Get Hob list
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  ASSERT_EFI_ERROR (Status);

  HobList.Raw = GetNextGuidHob (&gEfiSmbusArpMapGuid, HobList.Raw);
  //
  // If we found the right hob, store the information. Otherwise, continue.
  //
  if (HobList.Raw != NULL) {
    Data      = (VOID *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));
    DataSize  = HobList.Header->HobLength - sizeof (EFI_HOB_GUID_TYPE);
    CopyMem (mSmbusContext->DeviceMap, Data, DataSize);
    mSmbusContext->DeviceMapEntries = (UINT8) (DataSize / sizeof (EFI_SMBUS_DEVICE_MAP));
  }
  //
  // Initialize the NotifyFunctionList
  //
  InitializeListHead (&mSmbusContext->NotifyFunctionList);

  //
  // Install the SMBus interface
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmbusContext->Handle,
                  &gEfiSmbusHcProtocolGuid,
                  &mSmbusContext->SmbusController,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "InitializeScSmbus() End\n"));

  return EFI_SUCCESS;
}
