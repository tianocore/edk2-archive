/** @file
  HECI driver.

  Copyright (c) 2007 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Guid/EventGroup.h>
#include "Hecidrv.h"
#include <Private/Library/HeciInitLib.h>
#include <ScRegs/RegsPcu.h>
#include <Library/S3BootScriptLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/SideBandLib.h>

extern DXE_SEC_POLICY_PROTOCOL *mDxePlatformSeCPolicy;

EFI_HECI_PROTOCOL *mHeci2Protocol = NULL;

HECI_INSTANCE     *mHeciContext;
EFI_HANDLE        mHeciDrv;

#define S_SEC_DevID_RANGE_LO_Test 0xA9A
#define S_SEC_DevID_RANGE_HI_Test 0xA9E

VOID
EFIAPI
FlashDeviceEndOfServicesEvent (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS     HobPtr;
  MBP_CURRENT_BOOT_MEDIA   *BootMediaData = NULL;

  DEBUG ((EFI_D_INFO, "Begin - End of Services HECI Event\n"));

  ASSERT_EFI_ERROR (mHeci2Protocol != NULL);

  HobPtr.Guid = GetFirstGuidHob (&gEfiBootMediaHobGuid);
  ASSERT (HobPtr.Guid != NULL);

  BootMediaData = (MBP_CURRENT_BOOT_MEDIA *) GET_GUID_HOB_DATA (HobPtr.Guid);

  //
  // The Send Proxy State Message should not be sent on SPI boot
  //
  if (BootMediaData->PhysicalData != BOOT_FROM_SPI) {
    Heci2GetProxyStateNoResp (mHeci2Protocol);
  }

  DEBUG ((EFI_D_INFO, "End - End of Services HECI Event\n"));
}


VOID
EFIAPI
FlashDeviceEndOfPostEvent (
  VOID
  )
{
  EFI_STATUS   Status;

  DEBUG((EFI_D_INFO, "Begin - End of Post HECI Event\n"));

  if (mHeci2Protocol == NULL) {
    DEBUG ((EFI_D_INFO, "HECI Protocol %x\n", mHeci2Protocol));
    Status = gBS->LocateProtocol (
                    &gEfiHeciSmmRuntimeProtocolGuid,
                    NULL,
                    &mHeci2Protocol
                    );
    if (GetFirstGuidHob (&gFdoModeEnabledHobGuid) == NULL) {
      ASSERT_EFI_ERROR (Status);
    }
  }

  DEBUG ((EFI_D_INFO, "End - End of Post HECI Event\n"));
}


/**
  Notify the system of the End of Post message.

**/
VOID NotifyEOP (
  VOID
  )
{
  EFI_HANDLE    Handle;
  EFI_STATUS    Status;

  Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiCseEndofPostGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  FlashDeviceEndOfPostEvent ();
}


/**
  Notify the system send End of Services message.

**/
VOID NotifyEOS (
  VOID
  )
{
  EFI_HANDLE    Handle;
  EFI_STATUS    Status;

  Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiCseEndofServicesGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  FlashDeviceEndOfServicesEvent ();
}


/**
  CF9GR Lock Config.

**/
VOID
LockConfig (
  VOID
  )
{
  UINT32                          PmcBase;
  UINT32                          SecMode;
  HECI_FWS_REGISTER               SecFirmwareStatus;
  UINT32                          Data;

  DEBUG ((DEBUG_INFO, "LockConfig () - Start\n"));

  PmcBase        = PMC_BASE_ADDRESS;

  HeciGetSeCStatus (&SecMode);
  SecFirmwareStatus.ul = HeciPciRead32 (R_SEC_FW_STS0);
  DEBUG ((DEBUG_INFO, "SecFirmwareStatus.ul: 0x%X\n", SecFirmwareStatus.ul));

  //
  // APL/BXT IAFW Spec: Additional Power Management Programming
  // Step 2
  // Lock the "CF9h Global reset" field by setting CF9LOCK bit (PBASE + 0x48[31]) for production build,
  // but this field should not be locked for manufacturing mode. When the Manufacturing Mode is closed,
  // CF9h Global Reset should be cleared (step#1) and CF9LOCK bit should be set (step#2).
  //
  Data = 0;
  if (((SecMode == SEC_MODE_NORMAL) || (SecMode == SEC_MODE_TEMP_DISABLED)) && !(SecFirmwareStatus.r.ManufacturingMode)) {
    Data |= B_PMC_PMIR_CF9LOCK;
  }

  MmioAndThenOr32 (
    (UINTN) (PmcBase + R_PMC_PMIR),
    (UINT32) (~(B_PMC_PMIR_CF9LOCK | B_PMC_PMIR_CF9GR)),
    (UINT32) Data
    );

  S3BootScriptSaveMemWrite (
    EfiBootScriptWidthUint32,
    (UINTN) (PmcBase + R_PMC_PMIR),
    1,
    (VOID *) (UINTN) (PmcBase + R_PMC_PMIR)
    );
}


/**
  Event handle for Exit boot services.

  @param[in]  Event
  @param[in]  ParentImageHandle

**/
VOID
EFIAPI
SeCExitBootServicesEvent (
  IN EFI_EVENT           Event,
  IN VOID                *ParentImageHandle
  )
{
  DEBUG ((EFI_D_INFO, "SeCExitBootServicesEvent ++\n"));
  DEBUG ((EFI_D_INFO, "Start Send EOS Heci Message\n"));

  HeciEndOfServices ();

  DEBUG ((EFI_D_INFO, "End Of Send EOS Heci Message\n"));

  NotifyEOS ();
  gBS->CloseEvent (Event);

  DEBUG ((EFI_D_INFO, "SeCExitBootServicesEvent --\n"));
}


/**
  Event handle for ready to boot.

  @param[in]  Event
  @param[in]  ParentImageHandle

**/
VOID
EFIAPI
SeCReadyToBootEvent (
  IN EFI_EVENT           Event,
  IN VOID                *ParentImageHandle
  )
{
  EFI_STATUS    Status;
  BOOLEAN       SendEOP;

  DEBUG ((EFI_D_INFO, "SeCReadyToBootEvent ++\n"));
  SendEOP = TRUE;
  Status = gBS->LocateProtocol (&gDxePlatformSeCPolicyGuid, NULL, (VOID **) &mDxePlatformSeCPolicy);
  if (!EFI_ERROR (Status)) {
    SendEOP = (mDxePlatformSeCPolicy->SeCConfig.EndOfPostEnabled == 1);
  }
  if (SendEOP) {
    HeciEndOfPost ();
  }

  NotifyEOP ();
  LockConfig ();

  gBS->CloseEvent (Event);

  DEBUG ((EFI_D_INFO, "SeCReadyToBootEvent --\n"));
}


/**
  Entry point for HECI Driver.

  @param[in]    ImageHandle    Standard entry point parameter.
  @param[in]    SystemTable    Standard entry point parameter.

  @retval       EFI_SUCCESS    Init success.
  @retval       Others         Error occurs during HECI driver init.

**/
EFI_STATUS
EFIAPI
InitializeHECI (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   ReadyToBootEvent = NULL;
  EFI_EVENT   ExitBootServicesEvent = NULL;
  EFI_EVENT   EndOfDxeEvent = NULL;

  DEBUG ((EFI_D_INFO, "InitializeHECI Driver Entry Point\n"));

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             SeCReadyToBootEvent,
             (VOID *) &ImageHandle,
             &ReadyToBootEvent
             );
  ASSERT_EFI_ERROR (Status);
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  SeCExitBootServicesEvent,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &ExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "Registering the End of DXE done Call back in HECI  ++\n"));
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  HeciDXECallback,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "InitializeHECI ++\n"));

  mHeciDrv = ImageHandle;
  mHeciContext = AllocateZeroPool (sizeof (HECI_INSTANCE));

  //
  // Initialize HECI protocol pointers
  //
  if (mHeciContext != NULL) {
    mHeciContext->HeciCtlr.ResetHeci      = ResetHeciInterface;
    mHeciContext->HeciCtlr.SendwACK       = HeciSendwACK;
    mHeciContext->HeciCtlr.ReadMsg        = HeciReceive;
    mHeciContext->HeciCtlr.SendMsg        = HeciSend;
    mHeciContext->HeciCtlr.InitHeci       = HeciInitialize;
    mHeciContext->HeciCtlr.ReInitHeci     = HeciReInitialize;
    mHeciContext->HeciCtlr.SeCResetWait   = SeCResetWait;
    mHeciContext->HeciCtlr.GetSeCStatus   = HeciGetSeCStatus;
    mHeciContext->HeciCtlr.GetSeCMode     = HeciGetSeCMode;
  }

  //
  // Install the HECI interface
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHeciContext->Handle,
                  &gEfiHeciProtocolGuid,
                  &mHeciContext->HeciCtlr,
                  NULL
                  );
  DEBUG ((EFI_D_INFO, "InitializeHECI --\n"));

  //
  // Disable the HECI3
  //
  SeCDeviceControl (HECI3_DEVICE, Disabled);

  return EFI_SUCCESS;
}

