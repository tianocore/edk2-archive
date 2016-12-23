/** @file
  This file include all platform action which can be customized by IBV/OEM.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EfiBootStub.h"
#include "OsipPrivate.h"
#include <Library/DxeServicesLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>

//
// Global descriptor table (GDT) Template
//
STATIC GDT_ENTRIES GdtTemplate = {
  //
  // NULL_SEL
  //
  {
    0x0,            // limit 15:0
    0x0,            // base 15:0
    0x0,            // base 23:16
    0x0,            // type
    0x0,            // limit 19:16, flags
    0x0,            // base 31:24
  },

  //
  // NULL_SEL2
  //
  {
    0x0,            // limit 15:0
    0x0,            // base 15:0
    0x0,            // base 23:16
    0x0,            // type
    0x0,            // limit 19:16, flags
    0x0,            // base 31:24
  },

  //
  // SYS_CODE_SEL
  //
  {
    0x0FFFF,        // limit 0xFFFFF
    0x0,            // base 0
    0x0,
    0x09B,          // present, ring 0, data, expand-up, writable
    0x0CF,          // page-granular, 32-bit
    0x0,
  },

  //
  // SYS_DATA_SEL
  //
  {
    0x0FFFF,        // limit 0xFFFFF
    0x0,            // base 0
    0x0,
    0x093,          // present, ring 0, data, expand-up, writable
    0x0CF,          // page-granular, 32-bit
    0x0,
  }
};

VOID
AsmStartLinuxKernel (
  UINT32 KernelEntry
  );

EFI_STATUS
IsFileExistent (
  IN  EFI_HANDLE     Device,
  IN CONST CHAR16    *FileName
  )
{
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root;
  EFI_FILE_HANDLE                  ThisFile;

  Root     = NULL;
  ThisFile = NULL;
  //
  // Handle the file system interface to the device
  //
  Status = gBS->HandleProtocol (
                  Device,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID **) &Volume
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = Volume->OpenVolume (
                     Volume,
                     &Root
                     );
  if (EFI_ERROR (Status)) {
    Root = NULL;
    goto Done;
  }
  ASSERT (Root != NULL);
  Status = Root->Open (Root, &ThisFile, (CHAR16 *) FileName, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  ASSERT (ThisFile != NULL);

Done:
  if (ThisFile != NULL) {
    ThisFile->Close (ThisFile);
  }
  if (Root != NULL) {
    Root->Close (Root);
  }
  return Status;
}


EFI_STATUS
LoadFileFromFileSystem (
  IN CONST CHAR16        *FileName,
  OUT VOID               **FileBuffer,
  OUT UINTN              *FileSize
  )
{
  EFI_HANDLE                 *FileSystemHandles;
  UINTN                      NumberFileSystemHandles;
  UINTN                      Index;
  EFI_STATUS                 Status;
  EFI_DEVICE_PATH_PROTOCOL   *FilePath;
  UINT32                     AuthenticationStatus;

  *FileBuffer = NULL;
  *FileSize = 0;
  //
  // If there is simple file protocol which does not consume block Io protocol,
  // create a boot option for it here.
  //
  FileSystemHandles = NULL;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberFileSystemHandles,
         &FileSystemHandles
         );

  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    Status  =  IsFileExistent (FileSystemHandles[Index], FileName);
    if (!EFI_ERROR (Status)) {
      FilePath = FileDevicePath (FileSystemHandles[Index], FileName);
      *FileBuffer = GetFileBufferByFilePath (TRUE, FilePath, FileSize,&AuthenticationStatus);
      FreePool (FilePath);

      if (*FileBuffer != NULL)
        break;
    }
  }

  if (FileSystemHandles) {
    FreePool (FileSystemHandles);
  }

  if (*FileBuffer == NULL)
    return EFI_NOT_FOUND;

  return EFI_SUCCESS;
}


VOID
setup_boot_params (
  IN struct boot_params  *bp,
  struct setup_header    *sh,
  IN VOID                *CmdLine,
  IN VOID                *InitrdPtr,
  IN UINTN               InitrdSize
  )
{
  UINT8                     Index;

  ZeroMem (bp, sizeof (struct boot_params));
  bp->screen_info.orig_video_mode = 0;
  bp->screen_info.orig_video_lines = 0;
  bp->screen_info.orig_video_cols = 0;
  bp->alt_mem_k = 128 * 1024; // hard coded 128M mem here, since SFI will override it
  CopyMem (&bp->hdr, sh,  sizeof (struct setup_header));
  bp->hdr.cmd_line_ptr = (UINT32) (UINTN) CmdLine;
  bp->hdr.cmdline_size = (UINT32) (AsciiStrLen ((CONST CHAR8 *) (UINTN) CmdLine));
  bp->hdr.type_of_loader = 0xff; //bootstub is unknown bootloader for kernel :)
  bp->hdr.ramdisk_size = (UINT32) InitrdSize;
  bp->hdr.ramdisk_image = (bp->alt_mem_k * 1024 - bp->hdr.ramdisk_size) & 0xFFFFF000;
  bp->hdr.hardware_subarch = X86_SUBARCH_MRST;

  DEBUG ((EFI_D_INFO, "Relocating initramfs to high memory ...\n"));
  CopyMem ((UINT8 *) (UINTN) bp->hdr.ramdisk_image, (UINT8 *) InitrdPtr, (UINTN) bp->hdr.ramdisk_size);

  //
  // Prepare the e820 , hard code now
  //
  Index = 0;
  bp->e820_map[Index].addr = 0x00000;
  bp->e820_map[Index].size = 0x8f000;
  bp->e820_map[Index++].type = 1;

  bp->e820_map[Index].addr = 0x8f000;
  bp->e820_map[Index].size = 0x90000 - 0x8f000;
  bp->e820_map[Index++].type = 2;

  bp->e820_map[Index].addr = 0x90000;
  bp->e820_map[Index].size = 0xa0000 - 0x90000;
  bp->e820_map[Index++].type = 1;

  bp->e820_map[Index].addr = 0x100000;
  bp->e820_map[Index].size = 0x3afcf000 - 0x100000;
  bp->e820_map[Index++].type = 1;

  bp->e820_map[Index].addr = 0x3afcf000;
  bp->e820_map[Index].size = 0x3b7ff000 - 0x3afcf000;
  bp->e820_map[Index++].type = 2;

  bp->e820_map[Index].addr = 0x3b7ff000;
  bp->e820_map[Index].size = 0x3b800000 - 0x3b7ff000;
  bp->e820_map[Index++].type = 2;

  bp->e820_entries = Index;

}


UINT32
get_32bit_entry (
  unsigned char  *ptr
  )
{
  while (1){
    if (*(UINT32 *) ptr == SETUP_SIGNATURE && *(UINT32 *) (ptr+4) == 0)
      break;
    ptr++;
  }
  ptr+=4;
  return (UINT32) (((UINTN) ptr + 511) / 512) * 512;
}


EFI_STATUS
EfiBootStub (
  IN VOID        *CmdLine,
  IN VOID        *KernelPtr,
  IN UINTN       KernelSize,
  IN VOID        *InitrdPtr,
  IN UINTN       InitrdSize
  )
{
  GDT_ENTRIES       *gdt;
  IA32_DESCRIPTOR   gdtPtr;
  IA32_DESCRIPTOR   idtPtr = {0, 0};
  UINT32            KernelEntry;

  setup_boot_params (
    (struct boot_params *) (UINTN) BOOT_PARAMS_OFFSET,
    (struct setup_header *) (UINTN) SETUP_HEADER_OFFSET,
    CmdLine,
    InitrdPtr,
    InitrdSize
    );

  SetInterruptState (FALSE);

  //
  // Load idt and gdt
  //
  AsmWriteIdtr (&idtPtr);

  gdt = (GDT_ENTRIES *) (UINTN) ((UINTN) CmdLine - 0x1000);
  CopyMem (gdt, &GdtTemplate, sizeof (GdtTemplate));

  //
  // Write GDT register
  //
  gdtPtr.Base = (UINT32) (UINTN) (VOID *) gdt;
  gdtPtr.Limit = (UINT16) (sizeof (GdtTemplate) - 1);
  AsmWriteGdtr (&gdtPtr);

  //
  // Jmp to the kernel entry
  //
#if X64_BUILD_ENABLE
   //
   //put X64 jump here...
   //
   KernelEntry = 0;
#else
  KernelEntry = get_32bit_entry ((unsigned char *) (UINTN) BZIMAGE_OFFSET);
  AsmStartLinuxKernel (KernelEntry);
#endif

  return EFI_DEVICE_ERROR;
}


EFI_STATUS
BdsBootAndroidFromUsb (
  VOID
  )
{
  EFI_STATUS                       Status;
  VOID                             *KernelFileBuffer;
  UINTN                            KernelFileSize;
  VOID                             *InitrdFileBuffer;
  UINTN                            InitrdFileSize;
  UINT8                            *CmdPtr;
  CHAR8                            DefaultCmdLine [] = "console=ttyS0,115200 console=logk0 earlyprintk=nologger loglevel=8 kmemleak=off androidboot.bootmedia=sdcard androidboot.hardware=baylake emmc_ipanic.ipanic_part_number=1 vga=current i915.modeset=1 drm.vblankoffdelay=1 acpi_backlight=vendor";
  CHAR8                            MainBootMode [] = "androidboot.wakesrc=00 androidboot.mode=main";

  //
  // Load the kernel image
  //
  KernelFileBuffer = NULL;
  KernelFileSize = 0;
  Status = LoadFileFromFileSystem (L"\\kernel.img", &KernelFileBuffer, &KernelFileSize);
  if (EFI_ERROR (Status))
    return  EFI_NOT_FOUND;

  //
  // Load the initrd image
  //
  InitrdFileBuffer = NULL;
  InitrdFileSize = 0;
  Status = LoadFileFromFileSystem (L"\\initrd.img", &InitrdFileBuffer, &InitrdFileSize);
  if (EFI_ERROR (Status))
    return  EFI_NOT_FOUND;

  //
  // Prepare the cmd line at 0x1100000
  //
  CmdPtr = (UINT8 *) (UINTN) CMDLINE_OFFSET;
  ZeroMem (CmdPtr, 0x200);
  CopyMem (CmdPtr, DefaultCmdLine, sizeof (DefaultCmdLine));
  CmdPtr += (sizeof (DefaultCmdLine) - 1);
  *CmdPtr = 0x20;
  CmdPtr += 1;
  CopyMem (CmdPtr, MainBootMode, sizeof (MainBootMode));
  CmdPtr += (sizeof (MainBootMode) - 1);
  *CmdPtr = 0x0A;

  //
  // Copy the kernel image to 0x1102000
  //
  CmdPtr = (UINT8 *) (UINTN) BZIMAGE_OFFSET;
  CopyMem (CmdPtr, KernelFileBuffer, KernelFileSize);

  //
  // Start Efi Boot Stub
  //
  EfiBootStub ((UINT8 *) (UINTN) CMDLINE_OFFSET, (UINT8 *) (UINTN) BZIMAGE_OFFSET, KernelFileSize, InitrdFileBuffer, InitrdFileSize);

  FreePool (KernelFileBuffer);
  FreePool (InitrdFileBuffer);

  return EFI_DEVICE_ERROR;
}


EFI_STATUS
LoadOsipImageFromBlockDevices (
  IN    UINTN                           ImageIndex,
  OUT   EFI_PHYSICAL_ADDRESS            *ImageBase,
  OUT   UINTN                           *ImageSize,
  OUT   EFI_PHYSICAL_ADDRESS            *EntryPoint
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      *BlockIoHandles;
  UINTN                           NumberBlockIoHandles;
  UINTN                           Index;
  EFI_BLOCK_IO_PROTOCOL           *TestBlockIo;
  EFI_BLOCK_IO_PROTOCOL           *EmmcBlockIo;
  VOID                            *OsipBuffer;
  OSIP_HEADER                     *pOSIP;
  UINTN                           FirstBlock;
  UINTN                           NumberBlocks;
  VOID                            *pLoadAddress;
  UINTN                           OSSize;
  UINTN                           VrlSize;
  BOOLEAN                         SignedOs;

  EmmcBlockIo = NULL;
  pOSIP = NULL;

  OsipBuffer = NULL;
  OsipBuffer = AllocatePages (OSP_BLOCKS_TO_PAGES (OSIP_SIZE_IN_BLOCKS));
  if (!OsipBuffer)
    return EFI_OUT_OF_RESOURCES;

  BlockIoHandles = NULL;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiBlockIoProtocolGuid,
         NULL,
         &NumberBlockIoHandles,
         &BlockIoHandles
         );

  for (Index = 0; Index < NumberBlockIoHandles; Index++) {
    TestBlockIo = NULL;
    Status = gBS->HandleProtocol(
                    BlockIoHandles[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &TestBlockIo
                    );
    if ((EFI_ERROR (Status)) || (TestBlockIo == NULL)) {
      continue;
    }

    //
    // Read OSIP block from the block devices
    //
    ZeroMem (OsipBuffer, OSIP_SIZE_IN_BLOCKS * OSP_BLOCK_SIZE);
    Status = TestBlockIo->ReadBlocks(
                            TestBlockIo,
                            TestBlockIo->Media->MediaId,
                            (EFI_LBA) 0,  // OSIP hard coded to start at LBA 0
                            OSIP_SIZE_IN_BLOCKS * OSP_BLOCK_SIZE,
                            (VOID *) (UINTN) OsipBuffer);
    if (EFI_ERROR (Status))
      continue;

    pOSIP = (OSIP_HEADER *) (UINTN) OsipBuffer;
    if (pOSIP->Signature == OSIP_HEADER_SIGNATURE) {
      EmmcBlockIo = TestBlockIo;
      ValidateOsip(pOSIP, 1);
      break;
    }

    pOSIP = NULL;
  }

  if (BlockIoHandles) {
    FreePool (BlockIoHandles);
  }

  if ((!EmmcBlockIo) || (!pOSIP)) {
    if (OsipBuffer) {
      FreePages (OsipBuffer, OSP_BLOCKS_TO_PAGES(OSIP_SIZE_IN_BLOCKS));
    }

    return EFI_NOT_FOUND;
  }

  //
  // Parse OSII entry and find OS Image size in bytes
  //
  FirstBlock   = pOSIP->Osii[ImageIndex].FirstBlock;
  NumberBlocks = pOSIP->Osii[ImageIndex].BlockCount;
  pLoadAddress = (VOID *) (UINTN)(pOSIP->Osii[ImageIndex].LoadAddress);
  OSSize = NumberBlocks * OSP_BLOCK_SIZE;

  //
  // Load OS image to the address required
  //
  Status = EmmcBlockIo->ReadBlocks(
                          EmmcBlockIo,
                          EmmcBlockIo->Media->MediaId,
                          FirstBlock,
                          OSSize,
                          (VOID *) (UINTN) pLoadAddress);
  if (EFI_ERROR(Status)) {
    if (OsipBuffer) {
      FreePages (OsipBuffer, OSP_BLOCKS_TO_PAGES (OSIP_SIZE_IN_BLOCKS));
    }

    return EFI_DEVICE_ERROR;
  }

  SignedOs = ((pOSIP->Osii[ImageIndex].Attributes & 1) == 0) ? TRUE : FALSE;
  if (SignedOs) {
    VrlSize = 0x1e0;
  } else {
    VrlSize = 0;
  }

  OSSize -= VrlSize;

  if (ImageBase)
    *ImageBase = (EFI_PHYSICAL_ADDRESS) (UINTN)pLoadAddress;

  if (ImageSize)
    *ImageSize = OSSize;

  if (EntryPoint)
    *EntryPoint = (EFI_PHYSICAL_ADDRESS) (UINTN) pOSIP->Osii[ImageIndex].EntryPoint;

  if (OsipBuffer) {
    FreePages (OsipBuffer, OSP_BLOCKS_TO_PAGES (OSIP_SIZE_IN_BLOCKS));
  }

  return EFI_SUCCESS;
}


EFI_STATUS
BdsBootAndroidFromEmmc (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINTN                            KernelSize;
  VOID                             *InitrdBuffer;
  UINTN                            InitrdSize;
  UINT8                            *CmdPtr;
  CHAR8                            DefaultCmdLine [] = "console=ttyS0,115200 console=logk0 earlyprintk=nologger loglevel=8 kmemleak=off androidboot.bootmedia=sdcard androidboot.hardware=baylake emmc_ipanic.ipanic_part_number=1 vga=current i915.modeset=1 drm.vblankoffdelay=1 acpi_backlight=vendor";
  CHAR8                            MainBootMode [] = "androidboot.wakesrc=00 androidboot.mode=main";

  Status = LoadOsipImageFromBlockDevices (0, NULL, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to find OSIP Structure in any block devices \n"));
    return  EFI_NOT_FOUND;
  }

  //
  // Prepare the cmd line at 0x1100000
  //
  CmdPtr = (UINT8 *) (UINTN) CMDLINE_OFFSET;
  ZeroMem (CmdPtr, 0x200);
  CopyMem (CmdPtr, DefaultCmdLine, sizeof (DefaultCmdLine));
  CmdPtr += (sizeof (DefaultCmdLine) - 1);
  *CmdPtr = 0x20;
  CmdPtr += 1;
  CopyMem (CmdPtr, MainBootMode, sizeof (MainBootMode));
  CmdPtr += (sizeof (MainBootMode) - 1);
  *CmdPtr = 0x0A;

  //
  // Get the kernel and initrd information
  //
  KernelSize = *((UINT32 *) (UINTN) BZIMAGE_SIZE_OFFSET);
  InitrdSize = *((UINT32 *) (UINTN) INITRD_SIZE_OFFSET);
  InitrdBuffer = (VOID *) (UINTN) (BZIMAGE_OFFSET + KernelSize);

  //
  // Start Efi Boot Stub
  //
  EfiBootStub ((UINT8 *) (UINTN) CMDLINE_OFFSET, (UINT8 *) (UINTN) BZIMAGE_OFFSET, KernelSize, InitrdBuffer, InitrdSize);

  return EFI_DEVICE_ERROR;
}


EFI_STATUS
BdsBootAndroid (
  VOID
  )
{
  UINTN                            KernelSize;
  VOID                             *InitrdBuffer;
  UINTN                            InitrdSize;

  //
  // Get the kernel and initrd information
  //
  KernelSize = *((UINT32 *) (UINTN) BZIMAGE_SIZE_OFFSET);
  InitrdSize = *((UINT32 *) (UINTN) INITRD_SIZE_OFFSET);
  InitrdBuffer = (VOID *) (UINTN) (BZIMAGE_OFFSET + KernelSize);

  //
  // Start Efi Boot Stub
  //
  EfiBootStub ((UINT8 *) (UINTN) CMDLINE_OFFSET, (UINT8 *) (UINTN) BZIMAGE_OFFSET, KernelSize, InitrdBuffer, InitrdSize);

  return EFI_DEVICE_ERROR;
}

