/** @file
  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <SmipGenerated.h>
#include <Ppi/DramPolicyPpi.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiVariableCacheLib.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <Guid/PreMemoryVariableLocationHobGuid.h>
#include <Library/SteppingLib.h>
#include <BoardFunctionsPei.h>

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG(x)

#define COUNT_LIMIT 0x13   // reasonable limit for smipblock loop
#define GPT_MARKER 0x54504724

#define NO_MRC_TRG_DATA_MSG "__NO_MRC_TRNG_DATA_AVAILABLE__"

#pragma pack(push, 1)
typedef struct {
  UINT64  Address;
  UINT32  Size:24;
  UINT32  Rsvd:8;
  UINT16  Version;
  UINT8   Type:7;
  UINT8   C_V:1;
  UINT8   Checksum;
} FitEntry;

typedef struct {
  UINT16  BlockCount;
  UINT16  TotalSize;
} SmipHeader;

typedef struct {
  UINT16  BlockType;   // 0=CSE, 1=PMC, 2=IAFW
  UINT16  BlockOffset;
  UINT16  BlockLength;
  UINT16  Reserved;
} SmipBlockHeader;
#pragma pack(pop)


/**
  DramInstallPolicyPpi installs Dram Policy Ppi.

  @param[in] DramPolicyPpi          The pointer to Dram Policy PPI instance

  @retval    EFI_SUCCESS            The policy is installed.
  @retval    EFI_OUT_OF_RESOURCES   Insufficient resources to create buffer

**/
EFI_STATUS
EFIAPI
DramInstallPolicyPpi (
  IN  DRAM_POLICY_PPI *DramPolicyPpi
  )
{
  EFI_STATUS             Status;
  EFI_PEI_PPI_DESCRIPTOR *DramPolicyPpiDesc;

  DramPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (DramPolicyPpiDesc == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  DramPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  DramPolicyPpiDesc->Guid  = &gDramPolicyPpiGuid;
  DramPolicyPpiDesc->Ppi   = DramPolicyPpi;

  //
  // Install Silicon Policy PPI
  //
  Status = PeiServicesInstallPpi (DramPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


VOID
PrintVariableData (
  IN UINT8   *Data8,
  IN UINTN   DataSize
  )
{
  UINTN      Index;

  for (Index = 0; Index < DataSize; Index++) {
    if (Index % 0x10 == 0) {
      DEBUG ((EFI_D_INFO, "\n%08X:", Index));
    }
    DEBUG ((EFI_D_INFO, " %02X", *Data8++));
  }
  DEBUG ((EFI_D_INFO, "\n"));
}


/**
  Smip Init Function.

  @retval EFI_SUCCESS         Smip init successfully.
  @retval EFI_NOT_FOUND       Smip init fail.

**/
EFI_STATUS
SmipInit (
  IN VOID   *FitHeaderPtr,
  IN UINT8  BoardId
  )
{
  UINT16                            Index;
  EFI_STATUS                        Status;
  EFI_PEI_HOB_POINTERS              Hob;
  UINTN                             MrcTrainingDataSize  = 0;
  UINTN                             MrcBootDataSize      = 0;
  UINTN                             SetupDataSize        = 0;
  UINTN                             MrcTrainingDataAddr  = 0;
  UINTN                             MrcBootDataAddr      = 0;
  PRE_MEMORY_VARIABLE_LOCATION_HOB  PreMemoryVariableLocation;
  SYSTEM_CONFIGURATION              SystemConfiguration;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariablePpi;
  SmipHeader                        *SmipHeadPointer          = NULL;
  SmipBlockHeader                   *SmipBlockPtr             = NULL;
  IafwSmipLayout                    *SmipIafwPointer          = NULL;
  FitEntry                          *FitHeader                = NULL;
  FitEntry                          *FitEntries               = NULL;
  DRAM_POLICY_PPI                   *DramPolicyPpi            = NULL;
  CHAR8                             *NoMrcTrainingDataString  = NO_MRC_TRG_DATA_MSG;
  EFI_PLATFORM_INFO_HOB             *PlatformInfoHob          = NULL;
  DRAM_CREATE_POLICY_DEFAULTS_FUNC  DramCreatePolicyDefaultsFunc;

  ZeroMem (&Hob, sizeof (Hob));
  FitHeader  = (FitEntry *) FitHeaderPtr;
  FitEntries = FitHeader;

  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariablePpi);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  DEBUG ((EFI_D_INFO, "Checking FitHeader at 0x%08x\n", FitHeader ));

  if (FitHeader->Address != SIGNATURE_64 ('_', 'F', 'I', 'T', '_', ' ', ' ', ' ')) {
    DEBUG ((EFI_D_ERROR, "FitHeader signature was invalid.\n"));
    return EFI_NOT_FOUND;
  }
  DEBUG ((EFI_D_INFO, "FitHeader signature verified.\n"));

  //
  // Get Platform Info HOB
  //
  Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (Hob.Raw != NULL);
  PlatformInfoHob = GET_GUID_HOB_DATA(Hob.Raw);

  //
  // Loop through FIT Entries until we find SMIP (start at 1 to skip header)
  //
  DEBUG ((EFI_D_INFO, "Searching for SMIP Entry in FIT...\n" ));
  for (Index = 1; Index < FitHeader->Size; Index++) {
    if (FitEntries[Index].Type == 0x10 && FitEntries[Index].Rsvd == 0x5) {
      DEBUG ((EFI_D_INFO, "Found SMIP Entry in FIT.\n" ));

      SmipHeadPointer = (SmipHeader *) (UINTN) (FitEntries[Index].Address);
      SmipBlockPtr = (SmipBlockHeader *) ((UINT8 *) SmipHeadPointer + sizeof (SmipHeadPointer));

      DEBUG ((EFI_D_INFO, "SMIP table located at: 0x%08x\n", SmipHeadPointer));
      DEBUG ((EFI_D_INFO, "SMIP table size = 0x%08x bytes\n", SmipHeadPointer->TotalSize));
    } else if (FitEntries[Index].Type == 0x10 && FitEntries[Index].Rsvd == 0x6) {
      DEBUG ((EFI_D_INFO, "Found the entry for MRC Training Data in FIT. Checking if data is present...\n" ));
      PreMemoryVariableLocation.VariableDataPtr = (VOID *) (UINTN) (FitEntries[Index].Address);
      PreMemoryVariableLocation.VariableDataSize = FitEntries[Index].Size;

      //
      // Check if Setup data is available in the MRC training data file
      //
      if (AsciiStrnCmp ((CHAR8 *) PreMemoryVariableLocation.VariableDataPtr, NoMrcTrainingDataString, AsciiStrSize (NoMrcTrainingDataString)) != 0) {
        DEBUG ((EFI_D_INFO, "MRC training data is present in FIT. Data will be loaded from the file...\n"));
        DEBUG ((EFI_D_INFO, "Training data file contents at 0x%08x. Content data size = %d bytes.\n", PreMemoryVariableLocation.VariableDataPtr, PreMemoryVariableLocation.VariableDataSize));

        //
        // Store the temporary location of the training data variable store
        //
        BuildGuidDataHob (&gPreMemoryVariableLocationHobGuid, (VOID *) &PreMemoryVariableLocation, sizeof (PRE_MEMORY_VARIABLE_LOCATION_HOB));

        //
        // Create the variable cache HOB
        //   - It is populated with the pre-memory NVM file contents and consumed by the variable PEIM
        //     on variable read requests.
        //   - The address to the training data is stored in the MemoryConfig variable.
        //
        Status = CreateVariableCacheHob ();
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "The PEI variable data cache was not established but training data is available.\n"));
          ASSERT_EFI_ERROR (Status);
        }

        SetupDataSize = sizeof (SETUP_DATA);
        Status = VariablePpi->GetVariable (VariablePpi, L"Setup", &gEfiSetupVariableGuid, NULL, &SetupDataSize, &SystemConfiguration);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_WARN, "Could not read the Setup variable after establishing the cache.\n"));
        }

        MrcTrainingDataSize = sizeof (EFI_PHYSICAL_ADDRESS);
        Status = VariablePpi->GetVariable (VariablePpi, L"MemoryConfig", &gEfiMemoryConfigVariableGuid, NULL, &MrcTrainingDataSize, &MrcTrainingDataAddr);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_WARN, "Could not read the variable to find the MemoryConfig data address.\n"));
        }

        MrcBootDataSize = sizeof (EFI_PHYSICAL_ADDRESS);
        Status = VariablePpi->GetVariable (VariablePpi, L"MemoryBootData", &gEfiMemoryConfigVariableGuid, NULL, &MrcBootDataSize, &MrcBootDataAddr);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_WARN, "Could not read the variable to find the MemoryBootData address.\n"));
        }

        //
        // Print Setup variable data
        //
        DEBUG_CODE_BEGIN ();

        //
        // Print default Setup variable.
        //
        if (!EFI_ERROR (Status)) {
          DEBUG ((EFI_D_INFO, "\nDumping Setup data:"));
          PrintVariableData ((UINT8 *) &SystemConfiguration, SetupDataSize);
        }
        DEBUG_CODE_END ();
      } else {
        DEBUG ((EFI_D_INFO, "Training data not found. This is considered a first boot.\n\n"));
      }
    }
  }
  if (SmipHeadPointer == NULL) {
    DEBUG ((EFI_D_ERROR, "No SMIP Entry found in FIT\n"));
    return EFI_NOT_FOUND;
  }

  //
  // Loop through SMIP Header and look for IAFW SMIP entry
  //
  DEBUG ((EFI_D_INFO, "Searching SMIP Header for IAFW entry...\n" ));
  for (Index = 1; Index <= SmipHeadPointer->BlockCount && Index < COUNT_LIMIT; Index++, SmipBlockPtr++) {
    if ( SmipBlockPtr->BlockType == 0x2) {
      DEBUG ((EFI_D_INFO, "Found IAFW SMIP Block at 0x%08x\n", SmipBlockPtr->BlockOffset ));
      SmipIafwPointer = (IafwSmipLayout*) ((UINT8*) SmipHeadPointer + SmipBlockPtr->BlockOffset);
      break;
    }
  }

  if (SmipIafwPointer == NULL) {
    DEBUG ((EFI_D_ERROR, "No IAFW SMIP Block found\n"));
    return EFI_NOT_FOUND;
  }

  //
  // Parse SMIP and publish data to PPI's and PCD's -- STILL UNDER DEVELOPMENT...
  //
  DEBUG ((EFI_D_INFO, "IafwSmipLayout->IafwSmipSignature = 0x%08x\n", SmipIafwPointer->IafwSmipSignature ));
  DEBUG ((DEBUG_INFO, "***** Calling DramCreatePolicyDefaults ***** \n"));
  DramCreatePolicyDefaultsFunc = (DRAM_CREATE_POLICY_DEFAULTS_FUNC) (UINTN) PcdGet64 (PcdDramCreatePolicyDefaultsFunc);

  Status = DramCreatePolicyDefaultsFunc (VariablePpi, &DramPolicyPpi, &(SmipIafwPointer->DramConfig), &MrcTrainingDataAddr, &MrcBootDataAddr, BoardId);
  ASSERT_EFI_ERROR (Status);

  Status = DramInstallPolicyPpi (DramPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


