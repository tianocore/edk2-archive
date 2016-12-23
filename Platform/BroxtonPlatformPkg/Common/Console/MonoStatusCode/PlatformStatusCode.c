/** @file
  Contains Platform specific implementations required to use status codes.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformStatusCode.h"
#include <Library/PeiServicesLib.h>
#include <Library/TimerLib.h>
#include <Library/TraceHubDebugLib.h>

typedef struct {
  EFI_STATUS_CODE_DATA  DataHeader;
  EFI_HANDLE            Handle;
} PEIM_FILE_HANDLE_EXTENDED_DATA;

extern BOOLEAN ImageInMemory;


#define MmPciAddress( Segment, Bus, Device, Function, Register ) \
  ( (UINTN)PCIEX_BASE_ADDRESS + \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )

/**
  Get PostCode from status code type and value.

  @param[in]  CodeType         Indicates the type of status code being reported.
  @param[in]  Value            Describes the current status of a hardware or
                               software entity. This includes information about the class and
                               subclass that is used to classify the entity as well as an operation.
                               For progress codes, the operation is the current activity.
                               For error codes, it is the exception.For debug codes,it is not defined at this time.

  @return     PostCode

**/
UINT32
EFIAPI
GetPostCodeFromStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value
  );

EFI_STATUS
EFIAPI
Port80ReportStatusCode (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN EFI_STATUS_CODE_TYPE        CodeType,
  IN EFI_STATUS_CODE_VALUE       Value,
  IN UINT32                      Instance,
  IN CONST EFI_GUID              *CallerId,
  IN CONST EFI_STATUS_CODE_DATA  *Data OPTIONAL
  )
{
  EFI_STATUS               Status;
  EFI_FV_FILE_INFO         FvFileInfo;
  UINT16                   Port80Code = 0;

  //
  // Progress or error code, Output Port 80h card
  //
  Port80Code = (UINT16) GetPostCodeFromStatusCode (CodeType, Value);
  if (Port80Code == 0) {
    if ((Data != NULL) && (Value ==(EFI_SOFTWARE_PEI_CORE | EFI_SW_PC_INIT_BEGIN))) {
      Status = PeiServicesFfsGetFileInfo (
                 ((PEIM_FILE_HANDLE_EXTENDED_DATA *) (Data + 1))->Handle,
                 &FvFileInfo
                 );
      if (!EFI_ERROR (Status)) {
        Port80Code = (FvFileInfo.FileName.Data4[6]<<8) + (FvFileInfo.FileName.Data4[7]);
      }
    }
  }
  if (Port80Code != 0){
    IoWrite16 (0x80, (UINT16) Port80Code);
    DEBUG ((EFI_D_INFO, "POSTCODE=<%04x>\n", Port80Code));
  }

  return  EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
SerialReportStatusCode (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN EFI_STATUS_CODE_TYPE        CodeType,
  IN EFI_STATUS_CODE_VALUE       Value,
  IN UINT32                      Instance,
  IN CONST EFI_GUID              *CallerId,
  IN CONST EFI_STATUS_CODE_DATA  *Data OPTIONAL
  )
{
  CHAR8           *Filename;
  CHAR8           *Description;
  CHAR8           *Format;
  CHAR8           Buffer[EFI_STATUS_CODE_DATA_MAX_SIZE];
  UINT32          ErrorLevel;
  UINT32          LineNumber;
  UINTN           CharCount;
  BASE_LIST       Marker;

  Buffer[0] = '\0';

  if (Data != NULL &&
      ReportStatusCodeExtractAssertInfo (CodeType, Value, Data, &Filename, &Description, &LineNumber)) {
    //
    // Print ASSERT() information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "\n\rPEI_ASSERT!: %a (%d): %a\n\r",
                  Filename,
                  LineNumber,
                  Description
                  );
  } else if (Data != NULL &&
             ReportStatusCodeExtractDebugInfo (Data, &ErrorLevel, &Marker, &Format)) {
    //
    // Print DEBUG() information into output buffer.
    //
    CharCount = AsciiBSPrint (
                  Buffer,
                  sizeof (Buffer),
                  Format,
                  Marker
                  );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    //
    // Print ERROR information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "ERROR: C%x:V%x I%x",
                  CodeType,
                  Value,
                  Instance
                  );

    ASSERT(CharCount > 0);

    if (CallerId != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %g",
                     CallerId
                     );
    }

    if (Data != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %x",
                     Data
                     );
    }

    CharCount += AsciiSPrint (
                   &Buffer[CharCount],
                   (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                   "\n\r"
                   );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {

    //
    // remove "PROGRESS CODE" outputs and always returns EFI_SUCCESS.
    // This was done as hundreds of outputs were occuring but
    // there was confusion over the meaning/value of them.
    //
    return EFI_SUCCESS;

  } else if (Data != NULL &&
             CompareGuid (&Data->Type, &gEfiStatusCodeDataTypeStringGuid) &&
             ((EFI_STATUS_CODE_STRING_DATA *) Data)->StringType == EfiStringAscii) {
    //
    // EFI_STATUS_CODE_STRING_DATA
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "%a\n\r",
                  ((EFI_STATUS_CODE_STRING_DATA *) Data)->String.Ascii
                  );
  } else {
    //
    // Code type is not defined.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "Undefined: C%x:V%x I%x\n\r",
                  CodeType,
                  Value,
                  Instance
                  );
  }

  //
  // Call SerialPort Lib function to do print.
  //
  SerialPortWrite ((UINT8 *) Buffer, CharCount);

  return EFI_SUCCESS;
}


/**
  Call all status code listeners in the MonoStatusCode.

  @param[in]  PeiServices              The PEI core services table.
  @param[in]  CodeType                 Type of Status Code.
  @param[in]  Value                    Value to output for Status Code.
  @param[in]  Instance                 Instance Number of this status code.
  @param[in]  CallerId                 ID of the caller of this status code.
  @param[in]  Data                     Optional data associated with this status code.

  @retval     EFI_SUCCESS              If status code is successfully reported.
  @retval     EFI_NOT_AVAILABLE_YET    If StatusCodePpi has not been installed.

**/
EFI_STATUS
EFIAPI
PlatformReportStatusCode (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_STATUS_CODE_TYPE        CodeType,
  IN EFI_STATUS_CODE_VALUE       Value,
  IN UINT32                      Instance,
  IN CONST EFI_GUID              *CallerId,
  IN CONST EFI_STATUS_CODE_DATA  *Data OPTIONAL
  )
{
  //
  // If we are in debug mode, we will allow serial status codes
  //
  SerialReportStatusCode (PeiServices, CodeType, Value, Instance, CallerId, Data);

  Port80ReportStatusCode (PeiServices, CodeType, Value, Instance, CallerId, Data);

  return EFI_SUCCESS;
}


/**
  Install the PEIM.  Initialize listeners, publish the PPI and HOB for PEI and
  DXE use respectively.

  @param[in]  FfsHeader       FV this PEIM was loaded from.
  @param[in]  PeiServices     General purpose services available to every PEIM.

  @retval     EFI_SUCCESS     The function always returns success.

**/
EFI_STATUS
EFIAPI
InstallMonoStatusCode (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_PEI_PPI_DESCRIPTOR    *PeiPpiDescriptor;
  EFI_STATUS                Status = EFI_SUCCESS;

  if (!ImageInMemory) {

  //
  // Initialize all listeners
  //
  InitializeMonoStatusCode (FfsHeader, PeiServices);

 } else {

    //
    // locate the SEC platform information PPI
    //
    Status = (*PeiServices)->LocatePpi (
                               PeiServices,
                               &gEfiPeiStatusCodePpiGuid, // GUID
                               0,                         // INSTANCE
                               &PeiPpiDescriptor,         // EFI_PEI_PPI_DESCRIPTOR
                               NULL                       // PPI
                               );
    if (Status == EFI_SUCCESS) {
      //
      // Reinstall the StatusCode PPI
      //
      Status = (**PeiServices).ReInstallPpi (
                                 PeiServices,
                                 PeiPpiDescriptor,
                                 &mPpiListStatusCode
                                 );
    }
    //
    // Publish the listener in a HOB for DXE use.
    //
    InitializeDxeReportStatusCode (PeiServices);
  }

  return EFI_SUCCESS;
}


/**
  Initialize the Serial Port.

  @param[in]  FfsHeader      FV this PEIM was loaded from.
  @param[in]  PeiServices    General purpose services available to every PEIM.

  @retval     None

**/
VOID
EFIAPI
PlatformInitializeStatusCode (
  IN EFI_FFS_FILE_HEADER     *FfsHeader,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  //
  // Initialize additional debug status code listeners.
  //
  SerialPortInitialize ();
}

