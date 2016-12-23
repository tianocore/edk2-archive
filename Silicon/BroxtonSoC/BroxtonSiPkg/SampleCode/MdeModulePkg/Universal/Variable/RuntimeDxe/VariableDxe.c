/** @file
  Implement all four UEFI Runtime Variable services for the nonvolatile
  and volatile storage space and install variable architecture protocol.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Variable.h"

extern VARIABLE_STORE_HEADER   *mNvVariableCache;
extern VARIABLE_INFO_ENTRY     *gVariableInfo;
EFI_HANDLE                     mHandle                    = NULL;
EFI_EVENT                      mVirtualAddressChangeEvent = NULL;
extern BOOLEAN                 mEndOfDxe;
VOID                           ***mVarCheckAddressPointer = NULL;
UINTN                          mVarCheckAddressPointerCount = 0;
EDKII_VARIABLE_LOCK_PROTOCOL   mVariableLock              = { VariableLockRequestToLock };
EDKII_VAR_CHECK_PROTOCOL       mVarCheck                  = { VarCheckRegisterSetVariableCheckHandler,
                                                              VarCheckVariablePropertySet,
                                                              VarCheckVariablePropertyGet };

/**
  Return TRUE if ExitBootServices () has been called.

**/
BOOLEAN
AtRuntime (
  VOID
  )
{
  return EfiAtRuntime ();
}


/**
  Initializes a basic mutual exclusion lock.

  This function initializes a basic mutual exclusion lock to the released state
  and returns the lock.  Each lock provides mutual exclusion access at its task
  priority level.  Since there is no preemption or multiprocessor support in EFI,
  acquiring the lock only consists of raising to the locks TPL.
  If Lock is NULL, then ASSERT().
  If Priority is not a valid TPL value, then ASSERT().

  @param[in]  Lock       A pointer to the lock data structure to initialize.
  @param[in]  Priority   EFI TPL is associated with the lock.

  @return     The lock.

**/
EFI_LOCK *
InitializeLock (
  IN OUT EFI_LOCK                         *Lock,
  IN     EFI_TPL                          Priority
  )
{
  return EfiInitializeLock (Lock, Priority);
}


/**
  Acquires lock only at boot time. Simply returns at runtime.

  This is a temperary function that will be removed when
  EfiAcquireLock() in UefiLib can handle the call in UEFI
  Runtimer driver in RT phase.
  It calls EfiAcquireLock() at boot time, and simply returns
  at runtime.

  @param[in]  Lock         A pointer to the lock to acquire.

**/
VOID
AcquireLockOnlyAtBootTime (
  IN EFI_LOCK                             *Lock
  )
{
  if (!AtRuntime ()) {
    EfiAcquireLock (Lock);
  }
}


/**
  Releases lock only at boot time. Simply returns at runtime.

  This is a temperary function which will be removed when
  EfiReleaseLock() in UefiLib can handle the call in UEFI
  Runtimer driver in RT phase.
  It calls EfiReleaseLock() at boot time and simply returns
  at runtime.

  @param[in]  Lock         A pointer to the lock to release.

**/
VOID
ReleaseLockOnlyAtBootTime (
  IN EFI_LOCK                             *Lock
  )
{
  if (!AtRuntime ()) {
    EfiReleaseLock (Lock);
  }
}


/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  This is a notification function registered on EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  It convers pointer to new virtual address.

  @param[in]  Event        Event whose notification function is being invoked.
  @param[in]  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
VariableClassAddressChangeEvent (
  IN EFI_EVENT                            Event,
  IN VOID                                 *Context
  )
{
  UINTN                       Index;
  VARIABLE_STORAGE_PROTOCOL   *VariableStorageProtocol;

  //
  // Setup the callback to determine when to enable variable writes
  //
  for ( Index = 0;
        Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
        Index++) {
    VariableStorageProtocol = mVariableModuleGlobal->VariableGlobal.VariableStores[Index];
    EfiConvertPointer (0x0, (VOID **) &VariableStorageProtocol->GetId);
    EfiConvertPointer (0x0, (VOID **) &VariableStorageProtocol->GetVariable);
    EfiConvertPointer (0x0, (VOID **) &VariableStorageProtocol->GetAuthenticatedVariable);
    EfiConvertPointer (0x0, (VOID **) &VariableStorageProtocol->GetNextVariableName);
    EfiConvertPointer (0x0, (VOID **) &VariableStorageProtocol->GetStorageUsage);
    EfiConvertPointer (0x0, (VOID **) &VariableStorageProtocol->GetAuthenticatedSupport);
    EfiConvertPointer (0x0, (VOID **) &VariableStorageProtocol->SetVariable);
    EfiConvertPointer (0x0, (VOID **) &VariableStorageProtocol->WriteServiceIsReady);
    EfiConvertPointer (0x0, (VOID **) &VariableStorageProtocol->RegisterWriteServiceReadyCallback);
    EfiConvertPointer (0x0, (VOID **) &mVariableModuleGlobal->VariableGlobal.VariableStores[Index]);
  }
  EfiConvertPointer (0x0, (VOID **) &mVariableModuleGlobal->VariableGlobal.VariableStores);
  EfiConvertPointer (0x0, (VOID **) &mVariableModuleGlobal->PlatformLangCodes);
  EfiConvertPointer (0x0, (VOID **) &mVariableModuleGlobal->LangCodes);
  EfiConvertPointer (0x0, (VOID **) &mVariableModuleGlobal->PlatformLang);
  EfiConvertPointer (0x0, (VOID **) &mVariableModuleGlobal->VariableGlobal.VolatileVariableBase);
  EfiConvertPointer (0x0, (VOID **) &mVariableModuleGlobal->VariableGlobal.HobVariableBase);
  EfiConvertPointer (0x0, (VOID **) &mVariableModuleGlobal);
  EfiConvertPointer (0x0, (VOID **) &mNvVariableCache);

  if (mAuthContextOut.AddressPointer != NULL) {
    for (Index = 0; Index < mAuthContextOut.AddressPointerCount; Index++) {
      EfiConvertPointer (0x0, (VOID **) mAuthContextOut.AddressPointer[Index]);
    }
  }

  if (mVarCheckAddressPointer != NULL) {
    for (Index = 0; Index < mVarCheckAddressPointerCount; Index++) {
      EfiConvertPointer (0x0, (VOID **) mVarCheckAddressPointer[Index]);
    }
  }
}


/**
  Notification function of EVT_GROUP_READY_TO_BOOT event group.

  This is a notification function registered on EVT_GROUP_READY_TO_BOOT event group.
  When the Boot Manager is about to load and execute a boot option, it reclaims variable
  storage if free size is below the threshold.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
OnReadyToBoot (
  EFI_EVENT                               Event,
  VOID                                    *Context
  )
{
  if (!mEndOfDxe) {
    //
    // Set the End Of DXE bit in case the EFI_END_OF_DXE_EVENT_GROUP_GUID event is not signaled.
    //
    mEndOfDxe = TRUE;
    mVarCheckAddressPointer = VarCheckLibInitializeAtEndOfDxe (&mVarCheckAddressPointerCount);
    //
    // The initialization for variable quota.
    //
    InitializeVariableQuota ();
  }
  ReclaimForOS ();
  if (FeaturePcdGet (PcdVariableCollectStatistics)) {
    if (mVariableModuleGlobal->VariableGlobal.AuthFormat) {
      gBS->InstallConfigurationTable (&gEfiAuthenticatedVariableGuid, gVariableInfo);
    } else {
      gBS->InstallConfigurationTable (&gEfiVariableGuid, gVariableInfo);
    }
  }

  gBS->CloseEvent (Event);
}


/**
  Notification function of EFI_END_OF_DXE_EVENT_GROUP_GUID event group.

  This is a notification function registered on EFI_END_OF_DXE_EVENT_GROUP_GUID event group.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
OnEndOfDxe (
  EFI_EVENT                               Event,
  VOID                                    *Context
  )
{
  DEBUG ((EFI_D_INFO, "[Variable]END_OF_DXE is signaled\n"));
  mEndOfDxe = TRUE;
  mVarCheckAddressPointer = VarCheckLibInitializeAtEndOfDxe (&mVarCheckAddressPointerCount);
  //
  // The initialization for variable quota.
  //
  InitializeVariableQuota ();
  if (PcdGetBool (PcdReclaimVariableSpaceAtEndOfDxe)) {
    ReclaimForOS ();
  }

  gBS->CloseEvent (Event);
}


/**
  Non-Volatile variable write service is ready event handler.

**/
VOID
EFIAPI
InstallVariableWriteReady (
  VOID
  )
{
  EFI_STATUS    Status;

  DEBUG ((DEBUG_INFO, "Installing EfiVariableWriteArchProtocolGuid\n"));
  //
  // Install the Variable Write Architectural protocol.
  //
  Status = gBS->InstallProtocolInterface (
                  &mHandle,
                  &gEfiVariableWriteArchProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
}


/**
  Variable Driver main entry point. The Variable driver places the 4 EFI
  runtime services in the EFI System Table and installs arch protocols
  for variable read and write services being available. It also registers
  a notification function for an EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval    EFI_SUCCESS    Variable service successfully initialized.

**/
EFI_STATUS
EFIAPI
VariableServiceInitialize (
  IN EFI_HANDLE                ImageHandle,
  IN EFI_SYSTEM_TABLE          *SystemTable
  )
{
  EFI_STATUS                   Status;
  EFI_EVENT                    ReadyToBootEvent;
  EFI_EVENT                    EndOfDxeEvent;
  EFI_HANDLE                   *Handles;
  UINTN                        Index;

  //
  // Allocate runtime memory for variable driver global structure.
  //
  mVariableModuleGlobal = AllocateRuntimeZeroPool (sizeof (VARIABLE_MODULE_GLOBAL));
  if (mVariableModuleGlobal == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get the VARIABLE_STORAGE_PROTOCOL instances
  //
  mVariableModuleGlobal->VariableGlobal.VariableStoresCount = 0;
  Handles                                                   = NULL;

  //
  // Determine if Force Volatile Mode is enabled
  //
  mForceVolatileVariable = PcdGetBool (PcdForceVolatileVariable);

  if (!mForceVolatileVariable) {
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gVariableStorageProtocolGuid,
                    NULL,
                    &mVariableModuleGlobal->VariableGlobal.VariableStoresCount,
                    &Handles
                    );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    mVariableModuleGlobal->VariableGlobal.VariableStores =
      AllocateRuntimeZeroPool (
        sizeof (VARIABLE_STORAGE_PROTOCOL *) *
        mVariableModuleGlobal->VariableGlobal.VariableStoresCount
        );
    ASSERT (mVariableModuleGlobal->VariableGlobal.VariableStores != NULL);
    if (mVariableModuleGlobal->VariableGlobal.VariableStores == NULL) {
      FreePool (Handles);
      return EFI_OUT_OF_RESOURCES;
    }
    for ( Index = 0;
          Index < mVariableModuleGlobal->VariableGlobal.VariableStoresCount;
          Index++) {
      Status = gBS->OpenProtocol (
                      Handles[Index],
                      &gVariableStorageProtocolGuid,
                      (VOID **) &mVariableModuleGlobal->VariableGlobal.VariableStores[Index],
                      gImageHandle,
                      NULL,
                      EFI_OPEN_PROTOCOL_GET_PROTOCOL
                      );
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
        FreePool (Handles);
        return Status;
      }
    }
    FreePool (Handles);
  }

  //
  // Initialize variable services
  //
  Status = VariableCommonInitialize ();
  ASSERT_EFI_ERROR (Status);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gEdkiiVariableLockProtocolGuid,
                  &mVariableLock,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gEdkiiVarCheckProtocolGuid,
                  &mVarCheck,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  SystemTable->RuntimeServices->GetVariable         = VariableServiceGetVariable;
  SystemTable->RuntimeServices->GetNextVariableName = VariableServiceGetNextVariableName;
  SystemTable->RuntimeServices->SetVariable         = VariableServiceSetVariable;
  SystemTable->RuntimeServices->QueryVariableInfo   = VariableServiceQueryVariableInfo;

  //
  // Now install the Variable Runtime Architectural protocol on a new handle.
  //
  Status = gBS->InstallProtocolInterface (
                  &mHandle,
                  &gEfiVariableArchProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  VariableClassAddressChangeEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mVirtualAddressChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register the event handling function to reclaim variable for OS usage.
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             OnReadyToBoot,
             NULL,
             &ReadyToBootEvent
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Register the event handling function to set the End Of DXE flag.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  OnEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

