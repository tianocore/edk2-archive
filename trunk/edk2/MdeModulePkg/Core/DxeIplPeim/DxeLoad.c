/*++

Copyright (c) 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  DxeLoad.c

Abstract:

  Last PEIM.
  Responsibility of this module is to load the DXE Core from a Firmware Volume.

--*/

#include "DxeIpl.h"
#include <Ppi/GuidedSectionExtraction.h>

// porting note remove later
#include "FrameworkPei.h"
// end of remove later

EFI_STATUS
CustomDecompressExtractSection (
  IN CONST  EFI_PEI_GUIDED_SECTION_EXTRACTION_PPI *This,
  IN CONST  VOID                                  *InputSection,
  OUT       VOID                                  **OutputBuffer,
  OUT       UINTN                                 *OutputSize,
  OUT       UINT32                                *AuthenticationStatus
);

STATIC
EFI_STATUS
EFIAPI 
Decompress (
  IN CONST  EFI_PEI_DECOMPRESS_PPI  *This,
  IN CONST  EFI_COMPRESSION_SECTION *InputSection,
  OUT       VOID                    **OutputBuffer,
  OUT       UINTN                   *OutputSize
);


BOOLEAN gInMemory = FALSE;

//
// Module Globals used in the DXE to PEI handoff
// These must be module globals, so the stack can be switched
//
static EFI_DXE_IPL_PPI mDxeIplPpi = {
  DxeLoadCore
};

//static EFI_PEI_FV_FILE_LOADER_PPI mLoadFilePpi = {
//  DxeIplLoadFile
//};

STATIC EFI_PEI_GUIDED_SECTION_EXTRACTION_PPI mCustomDecompressExtractiongPpi = {
  CustomDecompressExtractSection
};

STATIC EFI_PEI_DECOMPRESS_PPI mDecompressPpi = {
  Decompress
};

static EFI_PEI_PPI_DESCRIPTOR     mPpiList[] = {
/*  {
  EFI_PEI_PPI_DESCRIPTOR_PPI,
  &gEfiPeiFvFileLoaderPpiGuid,
  &mLoadFilePpi
  },*/
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gEfiDxeIplPpiGuid,
    &mDxeIplPpi
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiDecompressPpiGuid,
    &mDecompressPpi
  }
};

static EFI_PEI_PPI_DESCRIPTOR     mPpiSignal = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  NULL
};

EFI_STATUS
EFIAPI
PeimInitializeDxeIpl (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  Initializes the Dxe Ipl PPI

Arguments:

  FfsHeader   - Pointer to FFS file header
  PeiServices - General purpose services available to every PEIM.

Returns:

  EFI_SUCCESS

--*/
{
  EFI_STATUS                                Status;
  EFI_BOOT_MODE                             BootMode;
  EFI_GUID                                  **DecompressGuidList;
  UINT32                                    DecompressMethodNumber;
  EFI_PEI_PPI_DESCRIPTOR                    *GuidPpi;
  
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = PeiServicesRegisterForShadow (FfsHeader);
    if (Status == EFI_SUCCESS) {

      gInMemory = TRUE;
      //
      // EFI_SUCESS means the first time call register for shadow 
      // 
      return Status;
    } else if (Status == EFI_ALREADY_STARTED) {


      //
      // Get custom decompress method guid list 
      //
      DecompressGuidList     = NULL;
      DecompressMethodNumber = 0;
      Status = CustomDecompressGetAlgorithms (DecompressGuidList, &DecompressMethodNumber);
      if (Status == EFI_OUT_OF_RESOURCES) {
      DecompressGuidList = (EFI_GUID **) AllocatePages (EFI_SIZE_TO_PAGES (DecompressMethodNumber * sizeof (EFI_GUID *)));
      ASSERT (DecompressGuidList != NULL);
      Status = CustomDecompressGetAlgorithms (DecompressGuidList, &DecompressMethodNumber);
      }
      ASSERT_EFI_ERROR(Status);
      
      //
      // Install custom decompress extraction guid ppi
      //
      if (DecompressMethodNumber > 0) {
      	GuidPpi = NULL;
      	GuidPpi = (EFI_PEI_PPI_DESCRIPTOR *) AllocatePages (EFI_SIZE_TO_PAGES (DecompressMethodNumber * sizeof (EFI_PEI_PPI_DESCRIPTOR)));
      	ASSERT (GuidPpi != NULL);
      	while (DecompressMethodNumber-- > 0) {
      	  GuidPpi->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
      	  GuidPpi->Ppi   = &mCustomDecompressExtractiongPpi;
      	  GuidPpi->Guid  = DecompressGuidList [DecompressMethodNumber];
      	  Status = PeiServicesInstallPpi (GuidPpi++);
      	  ASSERT_EFI_ERROR(Status);
      	}
      }
    } else {
      ASSERT_EFI_ERROR (FALSE);
    }
  }
  
  //
  // Install FvFileLoader and DxeIpl PPIs.
  //
  Status = PeiServicesInstallPpi (mPpiList);
  ASSERT_EFI_ERROR(Status);  
	
  return Status;
}

EFI_STATUS
EFIAPI
DxeLoadCore (
  IN EFI_DXE_IPL_PPI       *This,
  IN EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PEI_HOB_POINTERS  HobList
  )
/*++

Routine Description:

  Main entry point to last PEIM

Arguments:
  This         - Entry point for DXE IPL PPI
  PeiServices  - General purpose services available to every PEIM.
  HobList      - Address to the Pei HOB list

Returns:

  EFI_SUCCESS          - DEX core was successfully loaded.
  EFI_OUT_OF_RESOURCES - There are not enough resources to load DXE core.

--*/
{
  EFI_STATUS                                Status;
  EFI_GUID                                  DxeCoreFileName;
//  EFI_GUID                                  FirmwareFileName;
  //VOID                                      *Pe32Data;
//  VOID                                      *FvImageData;     
  EFI_PHYSICAL_ADDRESS                      DxeCoreAddress;
  UINT64                                    DxeCoreSize;
  EFI_PHYSICAL_ADDRESS                      DxeCoreEntryPoint;
  EFI_PEI_PE_COFF_LOADER_PROTOCOL           *PeiEfiPeiPeCoffLoader;
  EFI_BOOT_MODE                             BootMode;
  EFI_PEI_RECOVERY_MODULE_PPI               *PeiRecovery;
  EFI_PEI_S3_RESUME_PPI                     *S3Resume;
  EFI_PEI_FV_HANDLE                         VolumeHandle;
  EFI_PEI_FILE_HANDLE                       FileHandle;
  UINTN                                     Instance;


//  PERF_START (PeiServices, L"DxeIpl", NULL, 0);

  //
  // if in S3 Resume, restore configure
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR(Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    Status = PeiServicesLocatePpi (
               &gEfiPeiS3ResumePpiGuid,
               0,
               NULL,
               (VOID **)&S3Resume
               );
    ASSERT_EFI_ERROR (Status);

    Status = S3Resume->S3RestoreConfig (PeiServices);
    ASSERT_EFI_ERROR (Status);
  } else if (BootMode == BOOT_IN_RECOVERY_MODE) {

    Status = PeiServicesLocatePpi (
               &gEfiPeiRecoveryModulePpiGuid,
               0,
               NULL,
               (VOID **)&PeiRecovery
               );
    ASSERT_EFI_ERROR (Status);

    Status = PeiRecovery->LoadRecoveryCapsule (PeiServices, PeiRecovery);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Load Recovery Capsule Failed.(Status = %r)\n", Status));
      CpuDeadLoop ();
    }

    //
    // Now should have a HOB with the DXE core w/ the old HOB destroyed
    //
  }

  //
  // Install the PEI Protocols that are shared between PEI and DXE
  //
  PeiEfiPeiPeCoffLoader = (EFI_PEI_PE_COFF_LOADER_PROTOCOL *)GetPeCoffLoaderProtocol ();
  ASSERT (PeiEfiPeiPeCoffLoader != NULL);

  //
  // Look in all the FVs present in PEI and find the DXE Core
  //
  Instance = 0;
  Status = DxeIplFindFirmwareVolumeInstance (&Instance, EFI_FV_FILETYPE_DXE_CORE, &VolumeHandle, &FileHandle);
  ASSERT_EFI_ERROR (Status);

  CopyMem(&DxeCoreFileName, &(((EFI_FFS_FILE_HEADER*)FileHandle)->Name), sizeof (EFI_GUID));

  //
  // Load the DXE Core from a Firmware Volume
  //
  Status = PeiLoadFile (
            FileHandle,
            &DxeCoreAddress,
            &DxeCoreSize,
            &DxeCoreEntryPoint
            );

  ASSERT_EFI_ERROR (Status);

  //
  // Add HOB for the DXE Core
  //
  BuildModuleHob (
    &DxeCoreFileName,
    DxeCoreAddress,
    DxeCoreSize,
    DxeCoreEntryPoint
    );

  //
  // Add HOB for the PE/COFF Loader Protocol
  //
  BuildGuidDataHob (
    &gEfiPeiPeCoffLoaderGuid,
    (VOID *)&PeiEfiPeiPeCoffLoader,
    sizeof (VOID *)
    );
  //
  // Report Status Code EFI_SW_PEI_PC_HANDOFF_TO_NEXT
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_CORE_PC_HANDOFF_TO_NEXT
    );

  //
  // Transfer control to the DXE Core
  // The handoff state is simply a pointer to the HOB list
  //

  DEBUG ((EFI_D_INFO, "DXE Core Entry Point 0x%08x\n", (UINTN) DxeCoreEntryPoint));
  HandOffToDxeCore (DxeCoreEntryPoint, HobList, &mPpiSignal);
  //
  // If we get here, then the DXE Core returned.  This is an error
  // Dxe Core should not return.
  //
  ASSERT (FALSE);
  CpuDeadLoop ();

  return EFI_OUT_OF_RESOURCES;
}


EFI_STATUS
DxeIplFindFirmwareVolumeInstance (
  IN OUT UINTN              *Instance,
  IN  EFI_FV_FILETYPE       SeachType,
  OUT EFI_PEI_FV_HANDLE     *VolumeHandle,
  OUT EFI_PEI_FILE_HANDLE   *FileHandle
  )
/*++

Routine Description:

  Find the First Volume that contains the first FileType.

Arguments:

  Instance     - The Fv instance.
  SeachType    - The type of file to search.
  VolumeHandle - Pointer to Fv which contains the file to search. 
  FileHandle   - Pointer to FFS file to search.

Returns:

  EFI_STATUS

--*/  
{
  EFI_STATUS  Status;
  EFI_STATUS  VolumeStatus;

  do {
    VolumeStatus = PeiServicesFfsFindNextVolume (*Instance, VolumeHandle);
    if (!EFI_ERROR (VolumeStatus)) {
      *FileHandle = NULL;
      Status = PeiServicesFfsFindNextFile (SeachType, *VolumeHandle, FileHandle);
      if (!EFI_ERROR (Status)) {
        return Status;
      }
    }
    *Instance += 1;
  } while (!EFI_ERROR (VolumeStatus));

  return VolumeStatus;
}

EFI_STATUS
PeiLoadFile (
  IN  EFI_PEI_FILE_HANDLE                       FileHandle,
  OUT EFI_PHYSICAL_ADDRESS                      *ImageAddress,
  OUT UINT64                                    *ImageSize,
  OUT EFI_PHYSICAL_ADDRESS                      *EntryPoint
  )
/*++

Routine Description:

  Loads and relocates a PE/COFF image into memory.

Arguments:

  PeiEfiPeiPeCoffLoader - Pointer to a PE COFF loader protocol

  Pe32Data         - The base address of the PE/COFF file that is to be loaded and relocated

  ImageAddress     - The base address of the relocated PE/COFF image

  ImageSize        - The size of the relocated PE/COFF image

  EntryPoint       - The entry point of the relocated PE/COFF image

Returns:

  EFI_SUCCESS   - The file was loaded and relocated

  EFI_OUT_OF_RESOURCES - There was not enough memory to load and relocate the PE/COFF file

--*/
{

  EFI_STATUS                        Status;
  PE_COFF_LOADER_IMAGE_CONTEXT      ImageContext;
  VOID                              *Pe32Data;
  EFI_PEI_PE_COFF_LOADER_PROTOCOL   *PeiEfiPeiPeCoffLoader;

  PeiEfiPeiPeCoffLoader = (EFI_PEI_PE_COFF_LOADER_PROTOCOL *)GetPeCoffLoaderProtocol ();
  //
  // First try to find the required section in this ffs file.
  //
  Status = PeiServicesFfsFindSectionData (
             EFI_SECTION_PE32,
             FileHandle,
             &Pe32Data
             );

  if (EFI_ERROR (Status)) {
    Status = PeiServicesFfsFindSectionData (
               EFI_SECTION_TE,
               FileHandle,
               &Pe32Data
               );
  }
  
  if (EFI_ERROR (Status)) {
    //
    // NO image types we support so exit.
    //
    return Status;
  }

  ZeroMem (&ImageContext, sizeof (ImageContext));
  ImageContext.Handle = Pe32Data;
  Status              = GetImageReadFunction (&ImageContext);

  ASSERT_EFI_ERROR (Status);

  Status = PeiEfiPeiPeCoffLoader->GetImageInfo (PeiEfiPeiPeCoffLoader, &ImageContext);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Allocate Memory for the image
  //
  ImageContext.ImageAddress = (EFI_PHYSICAL_ADDRESS)(UINTN) AllocatePages (EFI_SIZE_TO_PAGES ((UINT32) ImageContext.ImageSize));
  ASSERT (ImageContext.ImageAddress != 0);

  //
  // Load the image to our new buffer
  //
  Status = PeiEfiPeiPeCoffLoader->LoadImage (PeiEfiPeiPeCoffLoader, &ImageContext);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Relocate the image in our new buffer
  //
  Status = PeiEfiPeiPeCoffLoader->RelocateImage (PeiEfiPeiPeCoffLoader, &ImageContext);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Flush the instruction cache so the image data is written before we execute it
  //
  InvalidateInstructionCacheRange ((VOID *)(UINTN)ImageContext.ImageAddress, (UINTN)ImageContext.ImageSize);

  *ImageAddress = ImageContext.ImageAddress;
  *ImageSize    = ImageContext.ImageSize;
  *EntryPoint   = ImageContext.EntryPoint;

  return EFI_SUCCESS;
}

/**
  The ExtractSection() function processes the input section and
  returns a pointer to the section contents. If the section being
  extracted does not require processing (if the section
  GuidedSectionHeader.Attributes has the
  EFI_GUIDED_SECTION_PROCESSING_REQUIRED field cleared), then
  OutputBuffer is just updated to point to the start of the
  section's contents. Otherwise, *Buffer must be allocated
  from PEI permanent memory.

  @param This                   Indicates the
                                EFI_PEI_GUIDED_SECTION_EXTRACTION_PPI instance.
                                Buffer containing the input GUIDed section to be
                                processed. OutputBuffer OutputBuffer is
                                allocated from PEI permanent memory and contains
                                the new section stream.
  
  @param OutputSize             A pointer to a caller-allocated
                                UINTN in which the size of *OutputBuffer
                                allocation is stored. If the function
                                returns anything other than EFI_SUCCESS,
                                the value of OutputSize is undefined.
  
  @param AuthenticationStatus   A pointer to a caller-allocated
                                UINT32 that indicates the
                                authentication status of the
                                output buffer. If the input
                                section's GuidedSectionHeader.
                                Attributes field has the
                                EFI_GUIDED_SECTION_AUTH_STATUS_VALID 
                                bit as clear,
                                AuthenticationStatus must return
                                zero. These bits reflect the
                                status of the extraction
                                operation. If the function
                                returns anything other than
                                EFI_SUCCESS, the value of
                                AuthenticationStatus is
                                undefined.
  
  @retval EFI_SUCCESS           The InputSection was
                                successfully processed and the
                                section contents were returned.
  
  @retval EFI_OUT_OF_RESOURCES  The system has insufficient
                                resources to process the request.
  
  @reteval EFI_INVALID_PARAMETER The GUID in InputSection does
                                not match this instance of the
                                GUIDed Section Extraction PPI.
**/
EFI_STATUS
CustomDecompressExtractSection (
  IN CONST  EFI_PEI_GUIDED_SECTION_EXTRACTION_PPI *This,
  IN CONST  VOID                                  *InputSection,
  OUT       VOID                                  **OutputBuffer,
  OUT       UINTN                                 *OutputSize,
  OUT       UINT32                                *AuthenticationStatus
)
{
  EFI_STATUS      Status;
  UINT8           *ScratchBuffer;
  UINT32          ScratchSize;
  UINT32          SectionLength;
  UINT32          DestinationSize;  
  
  //
  // Set authentic value to zero.
  //
  *AuthenticationStatus = 0;
  //
  // Calculate Section data Size
  //
  SectionLength   = *(UINT32 *) (((EFI_COMMON_SECTION_HEADER *) InputSection)->Size) & 0x00ffffff;
  //
  // Get compressed data information
  //
  Status = CustomDecompressGetInfo (
             (GUID *) ((UINT8 *) InputSection + sizeof (EFI_COMMON_SECTION_HEADER)),
             (UINT8 *) InputSection + sizeof (EFI_GUID_DEFINED_SECTION),
             SectionLength - sizeof (EFI_GUID_DEFINED_SECTION),
             &DestinationSize,
             &ScratchSize
             );
  if (EFI_ERROR (Status)) {
    //
    // GetInfo failed
    //
    DEBUG ((EFI_D_ERROR, "Extract guided section Failed - %r\n", Status));
    return Status;
  }

  //
  // Allocate scratch buffer
  //
  ScratchBuffer = AllocatePages (EFI_SIZE_TO_PAGES (ScratchSize));
  if (ScratchBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Allocate destination buffer
  //
  *OutputSize   = (UINTN) DestinationSize;
  *OutputBuffer = AllocatePages (EFI_SIZE_TO_PAGES (*OutputSize));
  if (*OutputBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Call decompress function
  //
  Status = CustomDecompress (
             (GUID *) ((UINT8 *) InputSection + sizeof (EFI_COMMON_SECTION_HEADER)),
             (UINT8 *) InputSection + sizeof (EFI_GUID_DEFINED_SECTION),
             *OutputBuffer,
             ScratchBuffer
             );

  if (EFI_ERROR (Status)) {
    //
    // Decompress failed
    //
    DEBUG ((EFI_D_ERROR, "Extract guided section Failed - %r\n", Status));
    return Status;
  }
  
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI 
Decompress (
  IN CONST  EFI_PEI_DECOMPRESS_PPI  *This,
  IN CONST  EFI_COMPRESSION_SECTION *CompressionSection,
  OUT       VOID                    **OutputBuffer,
  OUT       UINTN                   *OutputSize
 )
{
  EFI_STATUS                      Status;
  UINT8                           *DstBuffer;
  UINT8                           *ScratchBuffer;
  UINTN                           DstBufferSize;
  UINT32                          ScratchBufferSize;
  EFI_COMMON_SECTION_HEADER       *Section;
  UINTN                           SectionLength;

  if (CompressionSection->CommonHeader.Type != EFI_SECTION_COMPRESSION) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  Section = (EFI_COMMON_SECTION_HEADER *) CompressionSection;
  SectionLength         = *(UINT32 *) (Section->Size) & 0x00ffffff;
  
  //
  // This is a compression set, expand it
  //
  switch (CompressionSection->CompressionType) {
  case EFI_STANDARD_COMPRESSION:
    //
    // Load EFI standard compression.
    // For compressed data, decompress them to dstbuffer.
    //
    Status = UefiDecompressGetInfo (
               (UINT8 *) ((EFI_COMPRESSION_SECTION *) Section + 1),
               (UINT32) SectionLength - sizeof (EFI_COMPRESSION_SECTION),
               (UINT32 *) &DstBufferSize,
               &ScratchBufferSize
               );
    if (EFI_ERROR (Status)) {
      //
      // GetInfo failed
      //
      DEBUG ((EFI_D_ERROR, "Decompress GetInfo Failed - %r\n", Status));
      return EFI_NOT_FOUND;
    }
    //
    // Allocate scratch buffer
    //
    ScratchBuffer = AllocatePages (EFI_SIZE_TO_PAGES (ScratchBufferSize));
    if (ScratchBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Allocate destination buffer
    //
    DstBuffer = AllocatePages (EFI_SIZE_TO_PAGES (DstBufferSize));
    if (DstBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Call decompress function
    //
    Status = UefiDecompress (
                (CHAR8 *) ((EFI_COMPRESSION_SECTION *) Section + 1),
                DstBuffer,
                ScratchBuffer
                );
    if (EFI_ERROR (Status)) {
      //
      // Decompress failed
      //
      DEBUG ((EFI_D_ERROR, "Decompress Failed - %r\n", Status));
      return EFI_NOT_FOUND;
    }
    break;

  // porting note the original branch for customized compress is removed, it should be change to use GUID compress

  case EFI_NOT_COMPRESSED:
    //
    // Allocate destination buffer
    //
    DstBufferSize = CompressionSection->UncompressedLength;
    DstBuffer     = AllocatePages (EFI_SIZE_TO_PAGES (DstBufferSize));
    if (DstBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // stream is not actually compressed, just encapsulated.  So just copy it.
    //
    CopyMem (DstBuffer, CompressionSection + 1, DstBufferSize);
    break;

  default:
    //
    // Don't support other unknown compression type.
    //
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  *OutputSize = DstBufferSize;
  *OutputBuffer = DstBuffer;

  return EFI_SUCCESS;
}

