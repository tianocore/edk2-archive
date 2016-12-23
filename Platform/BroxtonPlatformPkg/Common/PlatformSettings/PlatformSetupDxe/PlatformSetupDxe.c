/** @file
  This file also installs UEFI PLATFORM_DRIVER_OVERRIDE_PROTOCOL.

  The main code offers a UI interface in device manager to let user configure
  platform override protocol to override the default algorithm for matching
  drivers to controllers.

  The main flow:
  1. It dynamically locates all controller device path.
  2. It dynamically locates all drivers which support binding protocol.
  3. It exports and dynamically updates two menu to let user select the
     mapping between drivers to controllers.
  4. It save all the mapping info in NV variables which will be consumed
     by platform override protocol driver to publish the platform override protocol.

  Copyright (c) 2007 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformSetupDxe.h"
#include "Guid/SetupVariable.h"
#include <Protocol/SeCOperation.h>
#include <Library/PlatformSecureDefaultsLib.h>
#include <SystemConfigVariable.h>

#define EFI_CALLBACK_INFO_SIGNATURE SIGNATURE_32 ('C', 'l', 'b', 'k')
#define EFI_CALLBACK_INFO_FROM_THIS(a)  CR (a, EFI_CALLBACK_INFO, ConfigAccess, EFI_CALLBACK_INFO_SIGNATURE)

#define RESET_GENERATOR_PORT 0xCF9

typedef struct {
  UINTN                           Signature;
  EFI_HANDLE                      DriverHandle;
  EFI_HII_HANDLE                  RegisteredHandle;
  SYSTEM_CONFIGURATION            FakeNvData;
  EFI_HII_CONFIG_ROUTING_PROTOCOL *HiiConfigRouting;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  ConfigAccess;
} EFI_CALLBACK_INFO;

#pragma pack(1)

//
// HII specific Vendor Device Path definition.
//
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;

#pragma pack()

//
// uni string and Vfr Binary data.
//
extern UINT8  VfrBin[];
extern UINT8  PlatformSetupDxeStrings[];

EFI_HANDLE            mImageHandle;

//
// module global data
//
#define EFI_NORMAL_SETUP_GUID \
  { 0xec87d643, 0xeba4, 0x4bb5, 0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0xd, 0xa9 }

EFI_GUID                     mNormalSetupGuid = EFI_NORMAL_SETUP_GUID;

EFI_GUID                     mSystemConfigGuid = SYSTEM_CONFIGURATION_GUID;
CHAR16                       mVariableName[] = L"Setup";
CHAR16                       mSetupName[] = L"Setup";
EFI_CALLBACK_INFO           *mCallbackInfo;

HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    EFI_CALLER_ID_GUID
  },

  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

VOID
LoadOsDefaultValues (
  IN EFI_CALLBACK_INFO                          *Private
  )
{
  Private->FakeNvData.GOPEnable                  = TRUE;
  Private->FakeNvData.SecureBoot                 = FALSE;
  Private->FakeNvData.Cg8254                     = FALSE;
  Private->FakeNvData.LpssI2C7Enabled            = 1;
  Private->FakeNvData.LpssSpi1Enabled            = 1;
  Private->FakeNvData.I2cNfc                     = TRUE;
  Private->FakeNvData.I2cPss                     = TRUE;
  Private->FakeNvData.SelectBtDevice             = TRUE;
  Private->FakeNvData.I2s343A                    = TRUE;
  Private->FakeNvData.I2s34C1                    = TRUE;
  Private->FakeNvData.UserCameraSel              = TRUE;
  Private->FakeNvData.WorldCameraSel             = 2;
#if X64_BUILD_ENABLE
  Private->FakeNvData.TPM                        = 1;
#else
  Private->FakeNvData.TPM                        = 0;
#endif
  Private->FakeNvData.MonitorMwaitEnable         = 2;
  Private->FakeNvData.I2C0Speed                  = 1;
  Private->FakeNvData.I2C1Speed                  = 1;
  Private->FakeNvData.I2C2Speed                  = 1;
  Private->FakeNvData.I2C3Speed                  = 1;
  Private->FakeNvData.I2C4Speed                  = 1;
  Private->FakeNvData.I2C5Speed                  = 1;
  Private->FakeNvData.I2C6Speed                  = 1;
  Private->FakeNvData.I2C7Speed                  = 1;
  Private->FakeNvData.TcoLock                    = 0;
}


VOID
LoadPlatformDefaultValues (
  IN EFI_CALLBACK_INFO                       *Private
  )
{
  switch (Private->FakeNvData.BoardId) {
    case BOARD_ID_LFH_CRB:
    case BOARD_ID_MINNOW:
      if (Private->FakeNvData.PmicSetupDefault == 1) {
        Private->FakeNvData.EnableRenderStandby = FALSE;
      }
    default:
      break;
  }

  Private->FakeNvData.PlatformSettingEn = 1;
}

VOID
CheckSystemConfigLoad(SYSTEM_CONFIGURATION *SystemConfigPtr);

VOID
CheckSystemConfigSave(SYSTEM_CONFIGURATION *SystemConfigPtr);


/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.

  @param[in]   This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]   Request                A null-terminated Unicode string in <ConfigRequest> format.
  @param[out]  Progress               On return, points to a character in the Request string.
                                      Points to the string's null terminator if request was successful.
                                      Points to the most recent '&' before the first failing name/value
                                      pair (or the beginning of the string if the failure is in the
                                      first name/value pair) if the request was not successful.
  @param[out]  Results                A null-terminated Unicode string in <ConfigAltResp> format which
                                      has all values filled in for the names in the Request string.
                                      String to be allocated by the called function.

  @retval      EFI_SUCCESS            The Results is filled with the requested values.
  @retval      EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval      EFI_INVALID_PARAMETER  Request is NULL, illegal syntax, or unknown name.
  @retval      EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
SystemConfigExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                       Status;
  EFI_CALLBACK_INFO                *Private;
  EFI_HII_CONFIG_ROUTING_PROTOCOL  *HiiConfigRouting;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;
  UINTN                            BufferSize;
  VOID                             *SystemConfigPtr;
  UINT32                           VariableAttributes;
  UINTN                            VariableSize = 0;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &mSystemConfigGuid, mVariableName)) {
    return EFI_NOT_FOUND;
  }

  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  Size             = 0;
  AllocatedRequest = FALSE;
  Private          = EFI_CALLBACK_INFO_FROM_THIS (This);

  SetupInfo ();

  HiiConfigRouting = Private->HiiConfigRouting;
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&mSystemConfigGuid, mVariableName, Private->DriverHandle);
    if (ConfigRequestHdr != NULL) {
      Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
      ConfigRequest = AllocateZeroPool (Size);
      ASSERT (ConfigRequest != NULL);
      if (ConfigRequest == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      AllocatedRequest = TRUE;
      BufferSize = sizeof (SYSTEM_CONFIGURATION);
      UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64) BufferSize);
      FreePool (ConfigRequestHdr);
    }
  }

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  SystemConfigPtr = AllocateZeroPool (VariableSize);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  SystemConfigPtr
                  );

  if (SystemConfigPtr == NULL) {
    ZeroMem (&Private->FakeNvData, sizeof (SYSTEM_CONFIGURATION));
  } else {
    CheckSystemConfigLoad (SystemConfigPtr);
    CopyMem (&Private->FakeNvData, SystemConfigPtr, sizeof (SYSTEM_CONFIGURATION));
    FreePool (SystemConfigPtr);
  }

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  Status = HiiConfigRouting->BlockToConfig (
                               HiiConfigRouting,
                               ConfigRequest,
                               (UINT8 *) &Private->FakeNvData,
                               sizeof (SYSTEM_CONFIGURATION),
                               Results,
                               Progress
                               );

  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}


/**
  This function processes the results of changes in configuration.

  @param[in]   This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]   Configuration          A null-terminated Unicode string in <ConfigRequest> format.
  @param[out]  Progress               A pointer to a string filled in with the offset of the most
                                      recent '&' before the first failing name/value pair (or the
                                      beginning of the string if the failure is in the first
                                      name/value pair) or the terminating NULL if all was successful.

  @retval      EFI_SUCCESS            The Results is processed successfully.
  @retval      EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval      EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
SystemConfigRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_CALLBACK_INFO            *Private;
  EFI_STATUS                   Status;
  UINTN                        VariableSize;
  UINT32                       VariableAttributes;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Configuration;

  if (!HiiIsConfigHdrMatch (Configuration, &mSystemConfigGuid, mVariableName)) {
    return EFI_NOT_FOUND;
  }

  Private = EFI_CALLBACK_INFO_FROM_THIS (This);
  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  &VariableAttributes,
                  &VariableSize,
                  &Private->FakeNvData
                  );

  Status  = mCallbackInfo->HiiConfigRouting->ConfigToBlock (mCallbackInfo->HiiConfigRouting, Configuration, (UINT8 *)&Private->FakeNvData, &VariableSize, Progress);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Error @ Function:%a Line:%d",__FUNCTION__, __LINE__));
    return Status;
  }
  VariableSize = sizeof (SYSTEM_CONFIGURATION);

  Status = gRT->SetVariable (
                  L"Setup",
                  &gEfiSetupVariableGuid,
                  VariableAttributes,
                  VariableSize,
                  &Private->FakeNvData
                  );
  CheckSystemConfigSave (&Private->FakeNvData);

  return EFI_SUCCESS;
}


/**
  This is the function that is called to provide results data to the driver.  This data
  consists of a unique key which is used to identify what data is either being passed back
  or being asked for.

  @param[in]   This                  Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]   Action                A null-terminated Unicode string in <ConfigRequest> format.
  @param[in]   KeyValue              A unique Goto OpCode callback value which record user's selection.
                                     0x100 <= KeyValue <0x500 : user select a controller item in the first page;
                                     KeyValue == 0x1234       : user select 'Refresh' in first page, or user select 'Go to Previous Menu' in second page
                                     KeyValue == 0x1235       : user select 'Pci device filter' in first page
                                     KeyValue == 0x1500       : user select 'order ... priority' item in second page
                                     KeyValue == 0x1800       : user select 'commit changes' in third page
                                     KeyValue == 0x2000       : user select 'Go to Previous Menu' in third page
  @param[in]   Type                  The type of value for the question.
  @param[in]   Value                 A pointer to the data being sent to the original exporting driver.
  @param[out]  ActionRequest         On return, points to the action requested by the callback function.

  @retval      EFI_SUCCESS           The Results is processed successfully.
               EFI_NOT_FOUND         The Data can't be found.
               EFI_OUT_OF_RESOURCES  The memory resource not enough.

**/
EFI_STATUS
EFIAPI
SystemConfigCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        KeyValue,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_CALLBACK_INFO                          *Private;
  SYSTEM_CONFIGURATION                       *FakeNvData;
  EFI_INPUT_KEY                              Key;
  CHAR16                                     *StringBuffer1;
  CHAR16                                     *StringBuffer2;
  EFI_STATUS                                 Status;
  SEC_OPERATION_PROTOCOL                     *SeCOp;

  StringBuffer1 = AllocateZeroPool (200 * sizeof (CHAR16));
  ASSERT (StringBuffer1 != NULL);
  StringBuffer2 = AllocateZeroPool (200 * sizeof (CHAR16));
  ASSERT (StringBuffer2 != NULL);

  if ((StringBuffer1 == NULL) || (StringBuffer2 == NULL)) {
    return EFI_OUT_OF_RESOURCES;
  }

  switch (Action) {
    case EFI_BROWSER_ACTION_CHANGING:
    case EFI_BROWSER_ACTION_CHANGED:
    {
      if (KeyValue == 0x1001) {
        Private = EFI_CALLBACK_INFO_FROM_THIS (This);
        FakeNvData = &Private->FakeNvData;
        if (!HiiGetBrowserData (&mSystemConfigGuid, mVariableName, sizeof (SYSTEM_CONFIGURATION), (UINT8 *) FakeNvData)) {
           return EFI_NOT_FOUND;
        }

        LoadOsDefaultValues (Private);

        //
        // Pass changed uncommitted data back to Form Browser
        //
        HiiSetBrowserData (&mSystemConfigGuid, mVariableName, sizeof (SYSTEM_CONFIGURATION), (UINT8 *) FakeNvData, NULL);
      } else if (KeyValue == 0x1002  /*IpuEn Callback*/) {
        DEBUG ((DEBUG_INFO,"IpuEn Callback.\n"));
        Private = EFI_CALLBACK_INFO_FROM_THIS (This);
        FakeNvData = &Private->FakeNvData;
        if (!HiiGetBrowserData (&mSystemConfigGuid, mVariableName, sizeof (SYSTEM_CONFIGURATION), (UINT8 *) FakeNvData)) {
          return EFI_NOT_FOUND;
        }

        if (Private->FakeNvData.IpuEn == 1) {
          Private->FakeNvData.VTdEnable = 0;
        }
        //
        // Pass changed uncommitted data back to Form Browser
        //
        HiiSetBrowserData (&mSystemConfigGuid, mVariableName, sizeof (SYSTEM_CONFIGURATION), (UINT8 *) FakeNvData, NULL);
      } else if (KeyValue == 0x1234) {
        Status = gBS->LocateProtocol (
                        &gEfiSeCOperationProtocolGuid,
                        NULL,
                        (VOID **) &SeCOp
                        );

        if (EFI_ERROR (Status)) {
          break;
        }
        StrCpyS (StringBuffer1, 200, L" Perform SeC UnConfiguration? ");
        StrCpyS (StringBuffer2, 200, L"Enter (YES)  /   Esc (NO)");

        //
        // Popup a menu to notice user
        //
        do {
          CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

        //
        // If the user hits the YES Response key,
        //
        if (Key.ScanCode == SCAN_ESC) {
          break;
        }
        StrCpyS (StringBuffer1, 200, L" Performing SeC UnConfiguration... ");
        StrCpyS (StringBuffer2, 200, L"  ");
        CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, NULL, StringBuffer1, StringBuffer2, NULL);
        Status = SeCOp->PerformSeCOperation(SEC_OP_UNCONFIGURATION);

      } else if (KeyValue == 0x1235) {
        StrCpyS (StringBuffer1, 200, L"Will you disable PTT ? ");
        StrCpyS (StringBuffer2, 200, L"Enter (YES)  /   Esc (NO)");

        //
        // Popup a menu to notice user
        //
        do {
          CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

        //
        // If the user hits the YES Response key,
        //
        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {

        }
      } else if (KeyValue == 0x1236) {
        StrCpyS (StringBuffer1, 200, L"Will you revoke trust ? ");
        StrCpyS (StringBuffer2, 200, L"Enter (YES)  /   Esc (NO)");

        //
        // Popup a menu to notice user
        //
        do {
          CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

        //
        // If the user hits the YES Response key,
        //
        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {

        }
      } else if (KeyValue == 0x1237 /*KEY_CLEAR_KEK_AND_PK*/ ) {
        //
        //Delete PK, KEK, DB, DBx
        //
        EnableCustomMode ();
        DeleteKeys ();
        StrCpyS (StringBuffer1, 200, L"Clear Keys Completed");
        StrCpyS (StringBuffer2, 200, L"Please Restart System");

        //
        // Popup a menu to notice user
        //
        do {
          CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

        gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
      } else if (KeyValue == 0x1238 /*KEY_LOAD_DEFAULTS_KEYS*/ ) {
        //
        // Enroll PK, KEK, DB and DBx
        //
        EnrollKeys ();
        StrCpyS (StringBuffer1, 200, L"Restore Keys Completed");
        StrCpyS (StringBuffer2, 200, L"Please Restart System");

        //
        // Popup a notification menu
        //
        do {
          CreatePopUp(EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

        //
        // Reset the system
        //
        gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

      } else if (KeyValue == 0x1239) {
        //
        // Popup a notification menu
        //
        StrCpyS (StringBuffer1, 200, L"Do you want to clear NVM / RPMB data region?");
        StrCpyS (StringBuffer2, 200, L"Enter (YES)  /   Esc (NO)");

        do {
          CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
        } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

        //
        // If the user hits the Enter (YES) Response key
        //
        if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
            //
            // CSE to Clear RPMB (NVM storage), it requires to receive the commands in sequence
            // i.e., from Host side, 'IFWI Prepare For Update' command should be issued first for the
            // purpose of keeping CSE in reset state and then 'Data Clear' Command to clear NVM.
            // If CSE is not in reset state, then Data clear will not happen.
            //
            Status = HeciIfwiPrepareForUpdate ();
            if (Status == EFI_SUCCESS) {
              //
              // CSE Data Clear Command for TXE Compliance Test
              //
              Status = HeciDataClear ();

              //
              // Popup a notification menu
              //
              if (Status == EFI_SUCCESS) {
                StrCpyS (StringBuffer1, 200, L"NVM / RPMB Data region Cleared SUCCESSFULLY");
                StrCpyS (StringBuffer2, 200, L"Press Enter to restart the system");
              } else {
                StrCpyS (StringBuffer1, 200, L"NVM / RPMB Data Clear FAILED");
                StrCpyS (StringBuffer2, 200, L"System still needs to restart, Press Enter");
              }

              do {
                CreatePopUp(EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
              } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

              //
              // Reset the system (EfiResetCold)
              // Note: At this point, 'IFWI Prepare for Update' command is given to CSE and CSE is in reset state.
              // Invoking gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL); Notifies HECI_SYSTEM_RESET by writing
              // into port 0xB2, this causes system hang and should not be issued. Hence directly writing into port 0xCF9
              //
              IoWrite8(RESET_GENERATOR_PORT, 0x2);
              IoWrite8(RESET_GENERATOR_PORT, 0xE);

            } else {
              StrCpyS (StringBuffer1, 200, L"NVM / RPMB Data Clear FAILED, CSE not in Reset State");
              StrCpyS (StringBuffer2, 200, L"Press Enter to Continue");
              do {
                CreatePopUp(EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
              } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
            }
        }

      }
      break;
    }


    case EFI_BROWSER_ACTION_DEFAULT_STANDARD:
    {
      Private = EFI_CALLBACK_INFO_FROM_THIS (This);
      FakeNvData = &Private->FakeNvData;
      if (!HiiGetBrowserData (&mSystemConfigGuid, mVariableName, sizeof (SYSTEM_CONFIGURATION), (UINT8 *) FakeNvData)) {
        return EFI_NOT_FOUND;
      }

      LoadPlatformDefaultValues (Private);
      //
      // Pass changed uncommitted data back to Form Browser
      //
      HiiSetBrowserData (&mSystemConfigGuid, mVariableName, sizeof (SYSTEM_CONFIGURATION), (UINT8 *) FakeNvData, NULL);

      break;
    }

    default:
      break;
  }

  FreePool (StringBuffer1);
  FreePool (StringBuffer2);

  if ((Action == EFI_BROWSER_ACTION_FORM_OPEN) || (Action == EFI_BROWSER_ACTION_FORM_CLOSE)) {
    //
    // Do nothing for UEFI OPEN/CLOSE Action
    //
    return EFI_SUCCESS;
  }

  Private = EFI_CALLBACK_INFO_FROM_THIS (This);
  FakeNvData = &Private->FakeNvData;
  if (!HiiGetBrowserData (&mSystemConfigGuid, mVariableName, sizeof (SYSTEM_CONFIGURATION), (UINT8 *) FakeNvData)) {
    return EFI_NOT_FOUND;
  }

  //
  // Pass changed uncommitted data back to Form Browser
  //
  HiiSetBrowserData (&mSystemConfigGuid, mVariableName, sizeof (SYSTEM_CONFIGURATION), (UINT8 *) FakeNvData, NULL);

  return EFI_SUCCESS;
}


VOID
EFIAPI
SetupStringUpdate (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  DEBUG ((EFI_D_ERROR, "SetupStringUpdate Event is Signalled \n"));

  gBS->CloseEvent (Event);
  SetupInfo ();
}


/**
  The driver Entry Point. The function will export a disk device class formset and
  its callback function to hii database.

  @param[in]  ImageHandle    The firmware allocated handle for the EFI image.
  @param[in]  SystemTable    A pointer to the EFI System Table.

  @retval     EFI_SUCCESS    The entry point is executed successfully.
  @retval     other          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
PlatformSetupDxeInit (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_FORM_BROWSER2_PROTOCOL  *FormBrowser2;
  SYSTEM_CONFIG               SystemConfig;
  UINTN                       DataSize;
  BOOLEAN                     HiiDataExport;
  EFI_EVENT                   Event;
  VOID                        *Registration;
  VOID                        *Interface;

  mImageHandle = ImageHandle;

  //
  // There should only be one Form Configuration protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiFormBrowser2ProtocolGuid,
                  NULL,
                  (VOID **) &FormBrowser2
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  mCallbackInfo = AllocateZeroPool (sizeof (EFI_CALLBACK_INFO));
  if (mCallbackInfo == NULL) {
    return EFI_BAD_BUFFER_SIZE;
  }

  mCallbackInfo->Signature = EFI_CALLBACK_INFO_SIGNATURE;
  mCallbackInfo->ConfigAccess.ExtractConfig = SystemConfigExtractConfig;
  mCallbackInfo->ConfigAccess.RouteConfig   = SystemConfigRouteConfig;
  mCallbackInfo->ConfigAccess.Callback      = SystemConfigCallback;

  //
  // Install Device Path Protocol and Config Access protocol to driver handle
  // Install Platform Driver Override Protocol to driver handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mCallbackInfo->DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mCallbackInfo->ConfigAccess,
                  NULL
                  );

  if (EFI_ERROR (Status)) {
    goto Finish;
  }

  //
  // Publish our HII data
  //
  mCallbackInfo->RegisteredHandle = HiiAddPackages (
                                      &mSystemConfigGuid,
                                      mCallbackInfo->DriverHandle,
                                      VfrBin,
                                      PlatformSetupDxeStrings,
                                      NULL
                                      );

  if (mCallbackInfo->RegisteredHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Finish;
  }

  mHiiHandle = mCallbackInfo->RegisteredHandle;
  //
  // Locate ConfigRouting protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiConfigRoutingProtocolGuid,
                  NULL,
                  (VOID **) &mCallbackInfo->HiiConfigRouting
                  );
  if (EFI_ERROR (Status)) {
    goto Finish;
  }

  //
  // Clear all the global variable
  //
  HiiDataExport = 0;
  Status = gBS->LocateProtocol (&gEfiVariableArchProtocolGuid, NULL, &Interface);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, " PlatformSetupDxeInit(): Get SystemConfig Variable \n"));
    ZeroMem (&SystemConfig, sizeof (SystemConfig));
    DataSize = sizeof (SYSTEM_CONFIG);

    Status = gRT->GetVariable (
                    SYSTEM_CONFIG_NAME,
                    &gSystemConfigGuid,
                    NULL,
                    &DataSize,
                    &SystemConfig
                    );

    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, " PlatformSetupDxeInit(): GetVariable Success assigning hiiexport flag \n"));
      HiiDataExport = (BOOLEAN) SystemConfig.HiiExport;
    }
  }
  DEBUG ((DEBUG_INFO, " PlatformSetupDxeInit(): HiiDataExport = 0x%x \n", HiiDataExport));
  if (HiiDataExport) {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    (EFI_EVENT_NOTIFY)SetupStringUpdate,
                    NULL,
                    &Event
                    );

    if (!EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, " PlatformSetupDxeInit(): Successfully Registered the Callback:SetupStringUpdate\n"));
      Status = gBS->RegisterProtocolNotify (
                      &gEfiDxeSmmReadyToLockProtocolGuid,
                      Event,
                      &Registration
                      );

      if (!EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, " PlatformSetupDxeInit(): DxeSmmReadyToLock Protocol Notify Registration is done:SetupStringUpdate\n"));
      }
    }
  }
  return EFI_SUCCESS;

Finish:
  if (mCallbackInfo->DriverHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           mCallbackInfo->DriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mHiiVendorDevicePath,
           &gEfiHiiConfigAccessProtocolGuid,
           &mCallbackInfo->ConfigAccess,
           NULL
           );
  }

  if (mCallbackInfo->RegisteredHandle != NULL) {
    HiiRemovePackages (mCallbackInfo->RegisteredHandle);
  }

  if (mCallbackInfo != NULL) {
    FreePool (mCallbackInfo);
  }

  return Status;
}


/**
  Unload its installed protocol.

  @param[in]  ImageHandle       Handle that identifies the image to be unloaded.

  @retval     EFI_SUCCESS       The image has been unloaded.

**/
EFI_STATUS
EFIAPI
PlatformSetupDxeUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  if (mCallbackInfo != NULL) {
    if (mCallbackInfo->DriverHandle != NULL) {
      gBS->UninstallMultipleProtocolInterfaces (
             mCallbackInfo->DriverHandle,
             &gEfiDevicePathProtocolGuid,
             &mHiiVendorDevicePath,
             &gEfiHiiConfigAccessProtocolGuid,
             &mCallbackInfo->ConfigAccess,
             NULL
             );
    }

    if (mCallbackInfo->RegisteredHandle != NULL) {
      HiiRemovePackages (mCallbackInfo->RegisteredHandle);
    }

    FreePool (mCallbackInfo);
  }

  return EFI_SUCCESS;
}

