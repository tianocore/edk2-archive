/** @file
  Implementation of Generic Memory Test Protocol which does not perform real memory test.
  Also clears memory when the MemoryOverwriteRequest bit is set.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "NullMemoryTest.h"
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Guid/MemoryOverwriteControl.h>

UINT64                            mTestedSystemMemory = 0;
UINT64                            mTotalSystemMemory  = 0;
EFI_HANDLE                        mGenericMemoryTestHandle;

EFI_GENERIC_MEMORY_TEST_PROTOCOL  mGenericMemoryTest = {
  InitializeMemoryTest,
  GenPerformMemoryTest,
  GenMemoryTestFinished,
  GenCompatibleRangeTest
};

EFI_STATUS
GetMemoryMap (
  OUT EFI_MEMORY_DESCRIPTOR       **EfiMemoryMap,
  OUT UINTN                       *EfiMemoryMapSize,
  UINTN                           *EfiDescriptorSize
  )
{
  EFI_STATUS                  Status;
  UINTN                       EfiMapKey;
  UINT32                      EfiDescriptorVersion;
  UINT8                       TmpMemoryMap[1];

  //
  // Get System MemoryMapSize
  //
  *EfiMemoryMapSize = 1;
  Status = gBS->GetMemoryMap (
                  EfiMemoryMapSize,
                  (EFI_MEMORY_DESCRIPTOR *) TmpMemoryMap,
                  &EfiMapKey,
                  EfiDescriptorSize,
                  &EfiDescriptorVersion
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // This error is expected in first call to GetMemoryMap().
    // After the first call, GetMemoryMap() will populate EfiMemoryMapSize
    // with the correct size.
    //
    DEBUG ((EFI_D_INFO, "Buffer too small\n"));
  }

  //
  // Now that we have the size of the memory map data, allocate some memory to store it in.
  //
  (*EfiMemoryMapSize) += EFI_PAGE_SIZE;
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  *EfiMemoryMapSize,
                  (VOID **)EfiMemoryMap
                  );
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "Can not Allocate Memory\n"));
    return Status;
  }

  //
  // Get System MemoryMap
  //
  Status = gBS->GetMemoryMap (
                  EfiMemoryMapSize,
                  *EfiMemoryMap,
                  &EfiMapKey,
                  EfiDescriptorSize,
                  &EfiDescriptorVersion
                  );
  return Status;
}

BOOLEAN
AddresInRange (
  EFI_PHYSICAL_ADDRESS  PhysicalAddress
  )
{
  if (sizeof (UINTN) == sizeof (UINT64)){
    return TRUE;
  } else {
    if (RShiftU64(PhysicalAddress, 32) == 0) {
      return TRUE;
    }
    return FALSE;
  }
}

EFI_STATUS
ClearAvailableSystemMemory (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_MEMORY_DESCRIPTOR         *EfiEntry;
  EFI_MEMORY_DESCRIPTOR         *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR         *EfiMemoryMap;
  UINTN                         EfiMemoryMapSize;
  UINTN                         EfiDescriptorSize;

  Status = GetMemoryMap (&EfiMemoryMap, &EfiMemoryMapSize, &EfiDescriptorSize);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  EfiEntry        = EfiMemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  while (EfiEntry < EfiMemoryMapEnd) {
    if (AddresInRange (EfiEntry->PhysicalStart)) {
      if (EfiEntry->Type == EfiConventionalMemory) {
        DEBUG((EFI_D_INFO, "CLEAR Memory at 0x%08p; ", EfiEntry->PhysicalStart));
        DEBUG((EFI_D_INFO, "Length: 0x%08x; Type: 0x%x\n", EFI_PAGES_TO_SIZE (EfiEntry->NumberOfPages), EfiEntry->Type));
        gBS->SetMem ((VOID *) (UINTN) EfiEntry->PhysicalStart, (UINTN )EFI_PAGES_TO_SIZE (EfiEntry->NumberOfPages), 0);
      } else {
        //
        // Skip this region if it is not "EfiConventionalMemory"
        //
        DEBUG ((EFI_D_INFO, "SKIP  Memory at 0x%08p; ", EfiEntry->PhysicalStart));
        DEBUG ((EFI_D_INFO, "Length: 0x%08x; Type: 0x%x\n", EFI_PAGES_TO_SIZE(EfiEntry->NumberOfPages), EfiEntry->Type));
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }

  gBS->FreePool(EfiMemoryMap);

  return EFI_SUCCESS;
}

BOOLEAN
IsMemoryOverwriteRequested (
  )
{
  BOOLEAN     bRetValue = 0;
  EFI_STATUS  Status;
  UINT64      MemoryOverwriteReq = 0;
  UINTN       VariableSize;

  DEBUG ((EFI_D_INFO, "IsMemoryOverwriteRequested\n"));

  //
  // Get the MOR variable
  //
  VariableSize = sizeof (MemoryOverwriteReq);
  Status = gRT->GetVariable (
                  MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                  &gEfiMemoryOverwriteControlDataGuid,
                  NULL,
                  &VariableSize,
                  &MemoryOverwriteReq
                  );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      VariableSize = sizeof (UINT8);
      Status = gRT->SetVariable (
                      MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
                      &gEfiMemoryOverwriteControlDataGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      VariableSize,
                      &MemoryOverwriteReq
                      );
    } else {
      MemoryOverwriteReq = 0;
      DEBUG ((EFI_D_ERROR, "IsMemoryOverwriteRequested: Failed to Get MOR Variable - Status = %r\n", Status));
    }
    goto Exit;
  }
   if (MOR_CLEAR_MEMORY_VALUE (MemoryOverwriteReq)) {
    DEBUG ((EFI_D_INFO, "IsMemoryOverwriteRequested: Un-controlled Reset detected and hence requesting the Memory Clear\n"));
    bRetValue = 1;
  }

Exit:
  DEBUG ((EFI_D_INFO, "IsMemoryOverwriteRequested: bRetValue = %d\n", bRetValue));
  return bRetValue;
}

VOID
HandleMorRequest (
  VOID
  )
{
  BOOLEAN bCleanMemory = 0;

  //
  // Check if there is a need for memory overwrite.
  //
  bCleanMemory = IsMemoryOverwriteRequested ();

  //
  // Clear memory if OS requested a memory overwrite operation.
  // This is being done for the memory used by OS => Stolen memory is not cleared as OS cannot store keys in UEFI Stolen memory
  //
  if (bCleanMemory) {
    DEBUG ((EFI_D_INFO, "Clearing the memory per MOR request.\n"));
    ClearAvailableSystemMemory ();
  }

  return;
}


/**
  Entry point of the NULL memory test driver.

  This function is the entry point of the NULL memory test driver.
  It simply installs the Generic Memory Test Protocol.

  @param  ImageHandle    The firmware allocated handle for the EFI image.
  @param  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    Generic Memory Test Protocol is successfully installed.

**/
EFI_STATUS
EFIAPI
GenericMemoryTestEntryPoint (
  IN  EFI_HANDLE           ImageHandle,
  IN  EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = gBS->InstallProtocolInterface (
                  &mGenericMemoryTestHandle,
                  &gEfiGenericMemTestProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mGenericMemoryTest
                  );
  ASSERT_EFI_ERROR (Status);

  HandleMorRequest();

  return EFI_SUCCESS;
}

/**
  Initialize the generic memory test.

  This function implements EFI_GENERIC_MEMORY_TEST_PROTOCOL.MemoryTestInit.
  It simply promotes untested reserved memory to system memory without real test.

  @param  This                Protocol instance pointer.
  @param  Level               The coverage level of the memory test.
  @param  RequireSoftECCInit  Indicate if the memory need software ECC init.

  @retval EFI_SUCCESS         The generic memory test initialized correctly.
  @retval EFI_NO_MEDIA        There is not any non-tested memory found, in this
                              function if not any non-tesed memory found means
                              that the memory test driver have not detect any
                              non-tested extended memory of current system.

**/
EFI_STATUS
EFIAPI
InitializeMemoryTest (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL          *This,
  IN  EXTENDMEM_COVERAGE_LEVEL                 Level,
  OUT BOOLEAN                                  *RequireSoftECCInit
  )
{
  UINTN                           NumberOfDescriptors;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap;
  UINTN                           Index;

  gDS->GetMemorySpaceMap (&NumberOfDescriptors, &MemorySpaceMap);
  for (Index = 0; Index < NumberOfDescriptors; Index++) {
    if (MemorySpaceMap[Index].GcdMemoryType == EfiGcdMemoryTypeReserved &&
        (MemorySpaceMap[Index].Capabilities & (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED)) ==
          (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED)
          ) {
      //
      // For those reserved memory that have not been tested, simply promote to system memory.
      //
      gDS->RemoveMemorySpace (
             MemorySpaceMap[Index].BaseAddress,
             MemorySpaceMap[Index].Length
             );

      gDS->AddMemorySpace (
             EfiGcdMemoryTypeSystemMemory,
             MemorySpaceMap[Index].BaseAddress,
             MemorySpaceMap[Index].Length,
             MemorySpaceMap[Index].Capabilities &~
             (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED | EFI_MEMORY_RUNTIME)
             );

      mTestedSystemMemory += MemorySpaceMap[Index].Length;
      mTotalSystemMemory += MemorySpaceMap[Index].Length;
    } else if (MemorySpaceMap[Index].GcdMemoryType == EfiGcdMemoryTypeSystemMemory) {
      mTotalSystemMemory += MemorySpaceMap[Index].Length;
    }
  }

  FreePool (MemorySpaceMap);

  *RequireSoftECCInit = FALSE;
  return EFI_SUCCESS;
}

/**
  Perform the memory test.

  This function implements EFI_GENERIC_MEMORY_TEST_PROTOCOL.PerformMemoryTest.
  It simply returns EFI_NOT_FOUND.

  @param  This                Protocol instance pointer.
  @param  TestedMemorySize    Return the tested extended memory size.
  @param  TotalMemorySize     Return the whole system physical memory size, this
                              value may be changed if in some case some error
                              DIMMs be disabled.
  @param  ErrorOut            Any time the memory error occurs, this will be
                              TRUE.
  @param  IfTestAbort         Indicate if the user press "ESC" to skip the memory
                              test.

  @retval EFI_SUCCESS         One block of memory test ok, the block size is hide
                              internally.
  @retval EFI_NOT_FOUND       Indicate all the non-tested memory blocks have
                              already go through.
  @retval EFI_DEVICE_ERROR    Mis-compare error, and no agent can handle it

**/
EFI_STATUS
EFIAPI
GenPerformMemoryTest (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL          *This,
  IN OUT UINT64                                *TestedMemorySize,
  OUT UINT64                                   *TotalMemorySize,
  OUT BOOLEAN                                  *ErrorOut,
  IN BOOLEAN                                   TestAbort
  )
{
  *ErrorOut         = FALSE;
  *TestedMemorySize = mTestedSystemMemory;
  *TotalMemorySize  = mTotalSystemMemory;

  return EFI_NOT_FOUND;
}

/**
  The memory test finished.

  This function implements EFI_GENERIC_MEMORY_TEST_PROTOCOL.Finished.
  It simply returns EFI_SUCCESS.

  @param  This                Protocol instance pointer.

  @retval EFI_SUCCESS         Successful free all the generic memory test driver
                              allocated resource and notify to platform memory
                              test driver that memory test finished.

**/
EFI_STATUS
EFIAPI
GenMemoryTestFinished (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL *This
  )
{
  return EFI_SUCCESS;
}

/**
  Provide capability to test compatible range which used by some special
  driver required using memory range before BDS perform memory test.

  This function implements EFI_GENERIC_MEMORY_TEST_PROTOCOL.CompatibleRangeTest.
  It simply sets the memory range to system memory.

  @param  This                Protocol instance pointer.
  @param  StartAddress        The start address of the memory range.
  @param  Length              The memory range's length.

  @retval EFI_SUCCESS           The compatible memory range pass the memory test.
  @retval EFI_INVALID_PARAMETER The compatible memory range must be below 16M.

**/
EFI_STATUS
EFIAPI
GenCompatibleRangeTest (
  IN EFI_GENERIC_MEMORY_TEST_PROTOCOL          *This,
  IN  EFI_PHYSICAL_ADDRESS                     StartAddress,
  IN  UINT64                                   Length
  )
{
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR Descriptor;

  gDS->GetMemorySpaceDescriptor (StartAddress, &Descriptor);

  gDS->RemoveMemorySpace (StartAddress, Length);

  gDS->AddMemorySpace (
         EfiGcdMemoryTypeSystemMemory,
         StartAddress,
         Length,
         Descriptor.Capabilities &~(EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED | EFI_MEMORY_RUNTIME)
         );

  return EFI_SUCCESS;
}

