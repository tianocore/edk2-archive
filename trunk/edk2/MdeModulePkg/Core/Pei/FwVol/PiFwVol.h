#ifndef __PIFWVOL_H_
#define __PIFWVOL_H_

EFI_STATUS
EFIAPI 
PiFvProcessFv (
  IN CONST  EFI_PEI_FIRMWARE_VOLUME_PPI *This,
  IN CONST  VOID                        *Buffer,
  IN CONST  UINTN                       BufferSize,
  OUT       EFI_PEI_FV_HANDLE           *FvHandle
);


EFI_STATUS
EFIAPI
PiFvFindFileByType ( 
  IN CONST  EFI_PEI_FIRMWARE_VOLUME_PPI   *This, 
  IN CONST  EFI_FV_FILETYPE               SearchType, 
  IN CONST  EFI_PEI_FV_HANDLE             FvHandle,
  IN OUT EFI_PEI_FILE_HANDLE              *FileHandle 
);


EFI_STATUS
EFIAPI 
PiFvFindFileByName (
  IN CONST  EFI_PEI_FIRMWARE_VOLUME_PPI *This,
  IN CONST  EFI_GUID                    *FileName,
  IN CONST  EFI_PEI_FV_HANDLE           FvHandle,
  OUT       EFI_PEI_FILE_HANDLE         *FileHandle
);


EFI_STATUS
EFIAPI 
PiFvGetFileInfo (
  IN  CONST EFI_PEI_FIRMWARE_VOLUME_PPI   *This, 
  IN  CONST EFI_PEI_FILE_HANDLE           FileHandle, 
  OUT       EFI_FV_FILE_INFO              *FileInfo
);

EFI_STATUS
EFIAPI 
PiFvGetVolumeInfo (
  IN CONST  EFI_PEI_FIRMWARE_VOLUME_PPI   *This, 
  IN CONST  EFI_PEI_FV_HANDLE             FvHandle, 
  OUT       EFI_FV_INFO                   *VolumeInfo
);

EFI_STATUS
EFIAPI
PiFvFindSectionByType (
  IN CONST  EFI_PEI_FIRMWARE_VOLUME_PPI   *This,
  IN CONST  EFI_SECTION_TYPE              SearchType,
  IN CONST  EFI_PEI_FILE_HANDLE           FileHandle,
  OUT       VOID                          **SectionData
);


#endif
