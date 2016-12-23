/** @file
  This file provides services for Dxe SA policy library.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "DxeSaPolicyLibrary.h"

#define SA_DXE_MISC_CONFIG_GUID          { 0xc7715fbc, 0xe2ab, 0x4a33, {0x84, 0x0f, 0x5d, 0xcd, 0x01, 0x98, 0xe5, 0x52}}
#define IGD_PANEL_CONFIG_GUID            { 0x5fd88b4c, 0xb658, 0x4650, {0xb3, 0xce, 0xa5, 0x9b, 0xb9, 0x91, 0xbf, 0xd4}}

GLOBAL_REMOVE_IF_UNREFERENCED  CONFIG_BLOCK_HEADER  mBxtSaDxeIpBlocks[] = {
  //      Block GUID                 Block Size,                    Revision
  { SA_DXE_MISC_CONFIG_GUID, sizeof (SA_DXE_MISC_CONFIG),  SA_DXE_MISC_CONFIG_REVISION, { 0, 0, 0 } },
  { IGD_PANEL_CONFIG_GUID,   sizeof (IGD_PANEL_CONFIG),    IGD_PANEL_CONFIG_REVISION,   { 0, 0, 0 } }
};

//
// Function call to Load defaults for Individial IP Blocks
//
EFI_STATUS
EFIAPI
LoadIgdPanelDefault (
  IN VOID    *ConfigBlockPointer
  )
{
  IGD_PANEL_CONFIG          *IgdPanelConfig = NULL;

  //
  // Initialize the IGD Panel configuration
  //
  IgdPanelConfig = ConfigBlockPointer;
  IgdPanelConfig->PFITStatus  = 0x00;
#if (ENBDT_PF_ENABLE == 1)
  IgdPanelConfig->PanelSelect = 0x00;
#endif

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
LoadSaDxeMiscDefault (
  IN VOID    *ConfigBlockPointer
  )
{
  SA_DXE_MISC_CONFIG         *SaDxeMiscConfig = NULL;

  SaDxeMiscConfig = ConfigBlockPointer;

  DEBUG ((DEBUG_INFO, "SaDxeMiscConfig->Header.Guid = %g\n", SaDxeMiscConfig->Header.Guid));
  DEBUG ((DEBUG_INFO, "SaDxeMiscConfig->Header.Size = 0x%x\n", SaDxeMiscConfig->Header.Size));

  return EFI_SUCCESS;
}


/**
  Initialize default settings for each SA DXE Config block.

  @param[in] ConfigBlockPointer         The buffer pointer that will be initialized as specific config block.
  @param[in] BlockId                    Request to initialize defaults of specified config block by given Block ID.

  @retval    EFI_SUCCESS                The given buffer has contained the defaults of requested config block.
  @retval    EFI_NOT_FOUND              Block ID is not defined so no default Config block will be initialized.

**/
EFI_STATUS
EFIAPI
LoadSaDxeConfigBlockDefault (
  IN   VOID          *ConfigBlockPointer,
  IN   EFI_GUID      BlockGuid
  )
{
  if (CompareGuid (&BlockGuid, &gIgdPanelConfigGuid)) {
    LoadIgdPanelDefault (ConfigBlockPointer);
  } else {
    if (CompareGuid (&BlockGuid, &gSaDxeMiscConfigGuid)) {
      LoadSaDxeMiscDefault (ConfigBlockPointer);
    } else {
      return EFI_NOT_FOUND;
    }
  }

  return EFI_SUCCESS;
}


/**
  Creates the Config Blocks for SA DXE Policy.
  It allocates and zero out buffer, and fills in the Intel default settings.

  @param[in, out] SiSaPolicy               The pointer to get SI/SA Policy Protocol.

  @retval         EFI_SUCCESS              The policy default is initialized.
  @retval         EFI_OUT_OF_RESOURCES     Insufficient resources to create buffer.

**/
EFI_STATUS
EFIAPI
CreateSaDxeConfigBlocks(
  IN OUT  SA_POLICY_PROTOCOL         **SaPolicy
  )
{
  UINT32                            TotalBlockSize;
  UINT16                            TotalBlockCount;
  UINT16                            BlockCount;
  VOID                              *ConfigBlockPointer;
  EFI_STATUS                        Status;
  SA_POLICY_PROTOCOL                *SaDxeInitPolicy;
  UINT32                            ConfigBlockHdrSize;
  UINT32                            RequiredSize;

  SaDxeInitPolicy = NULL;
  TotalBlockSize = 0;

  TotalBlockCount = sizeof (mBxtSaDxeIpBlocks) / sizeof (CONFIG_BLOCK_HEADER);
  DEBUG ((DEBUG_INFO, "TotalBlockCount = 0x%x\n", TotalBlockCount));

  for (BlockCount = 0; BlockCount < TotalBlockCount; BlockCount++) {
    TotalBlockSize += (UINT32) mBxtSaDxeIpBlocks[BlockCount].Size;
    DEBUG ((DEBUG_INFO, "TotalBlockSize after adding  Block[0x%x]= 0x%x\n", BlockCount, TotalBlockSize));
  }
  DEBUG ((DEBUG_INFO, "TotalBlockSize Final = 0x%x\n", TotalBlockSize));

  ConfigBlockHdrSize = GetSizeOfConfigBlockTableHeaders ((UINT16) TotalBlockCount);

  RequiredSize = ConfigBlockHdrSize + TotalBlockSize;

  Status = CreateConfigBlockTable ((VOID *) &SaDxeInitPolicy, TotalBlockCount, RequiredSize);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize Policy Revision
  //
  SaDxeInitPolicy->TableHeader.Header.Revision = SA_POLICY_PROTOCOL_REVISION_1;

  //
  // Initialize ConfigBlockPointer to NULL
  //
  ConfigBlockPointer = NULL;

  //
  // Loop to identify each config block from mBxtSaIpBlocks[] Table and add each of them
  //
  for (BlockCount = 0; BlockCount < TotalBlockCount; BlockCount++) {
    ConfigBlockPointer = (VOID *) &mBxtSaDxeIpBlocks[BlockCount];
    Status = AddConfigBlock ((VOID *) SaDxeInitPolicy, (VOID *) &ConfigBlockPointer);
    ASSERT_EFI_ERROR (Status);
    LoadSaDxeConfigBlockDefault ((VOID *) ConfigBlockPointer, mBxtSaDxeIpBlocks[BlockCount].Guid);
  }

  //
  // Assignment for returning SaInitPolicy config block base address
  //
  *SaPolicy = SaDxeInitPolicy;

  return EFI_SUCCESS;
}


/**
  Install protocol for SA Policy.
  While installed, RC assumes the Policy is ready and finalized. So please update and override
  any setting before calling this function.

  @param[in] DxeSaPolicy                The pointer to SA Policy Protocol instance

  @retval    EFI_SUCCESS                The policy is installed.
  @retval    Others                     Internal error when install protocol.

**/
EFI_STATUS
EFIAPI
SaInstallPolicyProtocol (
  IN  SA_POLICY_PROTOCOL *DxeSaPolicy
  )
{
  EFI_STATUS    Status;
  EFI_HANDLE    Handle;

  //
  // Print SA DXE Policy
  //
  SaPrintPolicyProtocol (DxeSaPolicy);

  //
  // Install protocol to to allow access to this Policy.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gSaPolicyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  DxeSaPolicy
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  This function prints the SA DXE phase policy.

  @param[in]    SaPolicy              The pointer to SA Policy Protocol instance.

**/
VOID
SaPrintPolicyProtocol (
  IN  SA_POLICY_PROTOCOL      *SaPolicy
  )
{
  EFI_STATUS                   Status;
  SA_DXE_MISC_CONFIG           *SaDxeMiscConfig = NULL;
  IGD_PANEL_CONFIG             *IgdPanelConfig = NULL;

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SaPolicy, &gIgdPanelConfigGuid, (VOID *) &IgdPanelConfig);
  ASSERT_EFI_ERROR (Status);

  Status = GetConfigBlock ((CONFIG_BLOCK_TABLE_HEADER *) SaPolicy, &gSaDxeMiscConfigGuid, (VOID *) &SaDxeMiscConfig);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "\n------------------------ SA Platform Policy (DXE) dump BEGIN -----------------\n"));
  DEBUG ((DEBUG_INFO, " Revision             : %x\n", SaPolicy->TableHeader.Header.Revision));

  DEBUG ((DEBUG_INFO, "------------------------ IGD_PANEL_CONFIGURATION -----------------\n"));
  DEBUG ((DEBUG_INFO, " Panel Scaling        : %x\n", IgdPanelConfig->PFITStatus));
#if (ENBDT_PF_ENABLE == 1)
  DEBUG ((DEBUG_INFO, " Panel Selection      : %x\n", IgdPanelConfig->PanelSelect));
#endif
  DEBUG ((DEBUG_INFO, "------------------------ SA_MISC_CONFIGURATION -----------------\n"));
  DEBUG ((DEBUG_INFO, " S0ix Support          : %x\n", SaDxeMiscConfig->S0ixSupported));
  DEBUG ((DEBUG_INFO, " Audio Type Support    : %x\n", SaDxeMiscConfig->AudioTypeSupport));

  DEBUG ((DEBUG_INFO, "\n------------------------ SA Platform Policy (DXE) dump END -----------------\n"));

  return;
}

