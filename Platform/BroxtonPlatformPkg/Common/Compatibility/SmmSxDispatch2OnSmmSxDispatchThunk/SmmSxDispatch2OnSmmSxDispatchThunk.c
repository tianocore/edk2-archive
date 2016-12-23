/** @file
  SMM SxDispatch2 Protocol on SMM SxDispatch Protocol Thunk driver.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmSxDispatch.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

typedef struct {
  LIST_ENTRY                     Link;
  EFI_HANDLE                     DispatchHandle;
  UINTN                          DispatchFunction;
} EFI_SMM_SX_DISPATCH2_THUNK_CONTEXT;


/**
  Provides the parent dispatch service for a given Sx source generator.

  This service registers a function (DispatchFunction) which will be called when the sleep state
  event specified by RegisterContext is detected. On return, DispatchHandle contains a
  unique handle which may be used later to unregister the function using UnRegister().
  The DispatchFunction will be called with Context set to the same value as was passed into
  this function in RegisterContext and with CommBuffer and CommBufferSize set to
  NULL and 0 respectively.

  @param[in]  This                   Pointer to the EFI_SMM_SX_DISPATCH2_PROTOCOL instance.
  @param[in]  DispatchFunction       Function to register for handler when the specified sleep state event occurs.
  @param[in]  RegisterContext        Pointer to the dispatch function's context.
                                     The caller fills this context in before calling
                                     the register function to indicate to the register
                                     function which Sx state type and phase the caller
                                     wishes to be called back on. For this intertace,
                                     the Sx driver will call the registered handlers for
                                     all Sx type and phases, so the Sx state handler(s)
                                     must check the Type and Phase field of the Dispatch
                                     context and act accordingly.
  @param[out] DispatchHandle         Handle of dispatch function, for when interfacing
                                     with the parent Sx state SMM driver.

  @retval     EFI_SUCCESS            The dispatch function has been successfully
                                     registered and the SMI source has been enabled.
  @retval     EFI_UNSUPPORTED        The Sx driver or hardware does not support that
                                     Sx Type/Phase.
  @retval     EFI_DEVICE_ERROR       The Sx driver was unable to enable the SMI source.
  @retval     EFI_INVALID_PARAMETER  RegisterContext is invalid. Type & Phase are not
                                     within valid range.
  @retval     EFI_OUT_OF_RESOURCES   There is not enough memory (system or SMM) to manage this
                                     child.

**/
EFI_STATUS
EFIAPI
SmmSxDispatch2Register (
  IN  CONST EFI_SMM_SX_DISPATCH2_PROTOCOL  *This,
  IN        EFI_SMM_HANDLER_ENTRY_POINT2   DispatchFunction,
  IN  CONST EFI_SMM_SX_REGISTER_CONTEXT    *RegisterContext,
  OUT       EFI_HANDLE                     *DispatchHandle
  );


/**
  Unregisters an Sx-state service.

  This service removes the handler associated with DispatchHandle so that it will no longer be
  called in response to sleep event.

  @param[in] This                   Pointer to the EFI_SMM_SX_DISPATCH2_PROTOCOL instance.
  @param[in] DispatchHandle         Handle of the service to remove.

  @retval    EFI_SUCCESS            The service has been successfully removed.
  @retval    EFI_INVALID_PARAMETER  The DispatchHandle was not valid.

**/
EFI_STATUS
EFIAPI
SmmSxDispatch2UnRegister (
  IN CONST EFI_SMM_SX_DISPATCH2_PROTOCOL  *This,
  IN       EFI_HANDLE                     DispatchHandle
  );

EFI_SMM_SX_DISPATCH2_PROTOCOL gSmmSxDispatch2 = {
  SmmSxDispatch2Register,
  SmmSxDispatch2UnRegister
};

EFI_SMM_SX_DISPATCH_PROTOCOL  *mSmmSxDispatch;

LIST_ENTRY                    mSmmSxDispatch2ThunkQueue = INITIALIZE_LIST_HEAD_VARIABLE (mSmmSxDispatch2ThunkQueue);


/**
  This function find SmmSxDispatch2Context by DispatchHandle.

  @param[in]  DispatchHandle      The DispatchHandle to indentify the SmmSxDispatch2Thunk context

  @return     SmmSxDispatch2Thunk context

**/
EFI_SMM_SX_DISPATCH2_THUNK_CONTEXT *
FindSmmSxDispatch2ContextByDispatchHandle (
  IN EFI_HANDLE   DispatchHandle
  )
{
  LIST_ENTRY                            *Link;
  EFI_SMM_SX_DISPATCH2_THUNK_CONTEXT    *ThunkContext;

  for (Link = mSmmSxDispatch2ThunkQueue.ForwardLink;
       Link != &mSmmSxDispatch2ThunkQueue;
       Link = Link->ForwardLink) {
    ThunkContext = BASE_CR (
                     Link,
                     EFI_SMM_SX_DISPATCH2_THUNK_CONTEXT,
                     Link
                     );
    if (ThunkContext->DispatchHandle == DispatchHandle) {
      return ThunkContext;
    }
  }

  return NULL;
}


/**
  Framework dispatch function for a Sx SMI handler.

  @param[in]  DispatchHandle        The handle of this dispatch function.
  @param[in]  DispatchContext       The pointer to the dispatch function's context.

  @retval     None

**/
VOID
EFIAPI
FrameworkDispatchFunction (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
  EFI_SMM_SX_DISPATCH2_THUNK_CONTEXT    *ThunkContext;
  EFI_SMM_HANDLER_ENTRY_POINT2          DispatchFunction;
  EFI_SMM_SX_REGISTER_CONTEXT           RegisterContext;
  UINTN                                 Size;

  //
  // Construct new context
  //
  RegisterContext.Type  = DispatchContext->Type;
  RegisterContext.Phase = DispatchContext->Phase;

  //
  // Dispatch
  //
  Size = 0;
  //
  // Search context
  //
  ThunkContext = FindSmmSxDispatch2ContextByDispatchHandle (DispatchHandle);
  ASSERT (ThunkContext != NULL);
  if (ThunkContext == NULL) {
    return ;
  }

  DispatchFunction = (EFI_SMM_HANDLER_ENTRY_POINT2) ThunkContext->DispatchFunction;
  DispatchFunction (
    DispatchHandle,
    &RegisterContext,
    NULL,
    &Size
    );

  return ;
}


/**
  Provides the parent dispatch service for a given Sx source generator.

  This service registers a function (DispatchFunction) which will be called when the sleep state
  event specified by RegisterContext is detected. On return, DispatchHandle contains a
  unique handle which may be used later to unregister the function using UnRegister().
  The DispatchFunction will be called with Context set to the same value as was passed into
  this function in RegisterContext and with CommBuffer and CommBufferSize set to
  NULL and 0 respectively.

  @param[in] This                    Pointer to the EFI_SMM_SX_DISPATCH2_PROTOCOL instance.
  @param[in] DispatchFunction        Function to register for handler when the specified sleep state event occurs.
  @param[in] RegisterContext         Pointer to the dispatch function's context.
                                     The caller fills this context in before calling
                                     the register function to indicate to the register
                                     function which Sx state type and phase the caller
                                     wishes to be called back on. For this intertace,
                                     the Sx driver will call the registered handlers for
                                     all Sx type and phases, so the Sx state handler(s)
                                     must check the Type and Phase field of the Dispatch
                                     context and act accordingly.
  @param[out] DispatchHandle         Handle of dispatch function, for when interfacing
                                     with the parent Sx state SMM driver.

  @retval     EFI_SUCCESS            The dispatch function has been successfully
                                     registered and the SMI source has been enabled.
  @retval     EFI_UNSUPPORTED        The Sx driver or hardware does not support that
                                     Sx Type/Phase.
  @retval     EFI_DEVICE_ERROR       The Sx driver was unable to enable the SMI source.
  @retval     EFI_INVALID_PARAMETER  RegisterContext is invalid. Type & Phase are not
                                     within valid range.
  @retval     EFI_OUT_OF_RESOURCES   There is not enough memory (system or SMM) to manage this
                                     child.

**/
EFI_STATUS
EFIAPI
SmmSxDispatch2Register (
  IN  CONST EFI_SMM_SX_DISPATCH2_PROTOCOL  *This,
  IN        EFI_SMM_HANDLER_ENTRY_POINT2   DispatchFunction,
  IN  CONST EFI_SMM_SX_REGISTER_CONTEXT    *RegisterContext,
  OUT       EFI_HANDLE                     *DispatchHandle
  )
{
  EFI_SMM_SX_DISPATCH2_THUNK_CONTEXT    *ThunkContext;
  EFI_SMM_SX_DISPATCH_CONTEXT           DispatchContext;
  EFI_STATUS                            Status;

  DispatchContext.Type  = RegisterContext->Type;
  DispatchContext.Phase = RegisterContext->Phase;

  Status = mSmmSxDispatch->Register (
                             mSmmSxDispatch,
                             FrameworkDispatchFunction,
                             &DispatchContext,
                             DispatchHandle
                             );

  if (!EFI_ERROR (Status)) {
    //
    // Register
    //
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (EFI_SMM_SX_DISPATCH2_THUNK_CONTEXT),
                      (VOID **) &ThunkContext
                      );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      mSmmSxDispatch->UnRegister (mSmmSxDispatch, *DispatchHandle);
      return EFI_OUT_OF_RESOURCES;
    }

    ThunkContext->DispatchFunction = (UINTN) DispatchFunction;
    ThunkContext->DispatchHandle   = *DispatchHandle;
    InsertTailList (&mSmmSxDispatch2ThunkQueue, &ThunkContext->Link);
  }

  return Status;
}


/**
  Unregisters an Sx-state service.

  This service removes the handler associated with DispatchHandle so that it will no longer be
  called in response to sleep event.

  @param[in] This                   Pointer to the EFI_SMM_SX_DISPATCH2_PROTOCOL instance.
  @param[in] DispatchHandle         Handle of the service to remove.

  @retval    EFI_SUCCESS            The service has been successfully removed.
  @retval    EFI_INVALID_PARAMETER  The DispatchHandle was not valid.

**/
EFI_STATUS
EFIAPI
SmmSxDispatch2UnRegister (
  IN CONST EFI_SMM_SX_DISPATCH2_PROTOCOL  *This,
  IN       EFI_HANDLE                     DispatchHandle
  )
{
  EFI_SMM_SX_DISPATCH2_THUNK_CONTEXT    *ThunkContext;
  EFI_STATUS                            Status;

  Status = mSmmSxDispatch->UnRegister (mSmmSxDispatch, DispatchHandle);
  if (!EFI_ERROR (Status)) {
    //
    // Unregister
    //
    ThunkContext = FindSmmSxDispatch2ContextByDispatchHandle (DispatchHandle);
    ASSERT (ThunkContext != NULL);
    if (ThunkContext != NULL) {
      RemoveEntryList (&ThunkContext->Link);
      gSmst->SmmFreePool (ThunkContext);
    }
  }

  return Status;
}


/**
  Entry Point for this thunk driver.

  @param[in] ImageHandle  Image handle of this driver.
  @param[in] SystemTable  A Pointer to the EFI System Table.

  @retval    EFI_SUCCESS  The entry point is executed successfully.
  @retval    other        Some error occurred when executing this entry point.

**/
EFI_STATUS
EFIAPI
SmmSxDispatch2ThunkMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS               Status;

  //
  // Locate Framework SMM SxDispatch Protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmmSxDispatchProtocolGuid, NULL, (VOID **) &mSmmSxDispatch);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Publish PI SMM SxDispatch2 Protocol
  //
  ImageHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &ImageHandle,
                    &gEfiSmmSxDispatch2ProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gSmmSxDispatch2
                    );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

