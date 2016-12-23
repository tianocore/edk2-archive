/** @file
  Memory Operation Functions for IA32 Architecture.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CpuInitDxe.h"
#include "MpCommon.h"
#include "MpService.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>

#if defined (__GNUC__)
#define ALIGN_16BYTE_BOUNDRY  __attribute__ ((aligned (16)))
#else
#define ALIGN_16BYTE_BOUNDRY  __declspec (align (16))
#endif

typedef struct {
  VOID  *Start;
  UINTN Size;
  UINTN FixOffset;
} INTERRUPT_HANDLER_TEMPLATE_MAP;

/**
  Get the address map of interrupt handler template

  @param[out] AddressMap        The address map of interrupt handle.

**/
VOID
GetTemplateAddressMap (
  OUT INTERRUPT_HANDLER_TEMPLATE_MAP *AddressMap
  );


/**
  Get the code segment

  @retval UINT16    The value of code segment

**/
UINT16
GetCodeSegment (
  VOID
  );


/**
  Get GDTR and IDTR

  @param[out] Gdt        Pointer to  GDT table.
  @param[out] Idt        Pointer to IDT table.

**/
VOID
AsmGetGdtrIdtr (
  OUT IA32_DESCRIPTOR         **Gdt,
  OUT IA32_DESCRIPTOR         **Idt
  );


/**
  Initialize Interrupt descriptor Tables

  @param[in] TableStart        Pointer to interrupt callback table.
  @param[in] IdtTablePtr       Pointer to IDT table.
  @param[in] IdtTableLimit     IDT Table limit.

**/
VOID
InitializeIdt (
  IN EFI_CPU_INTERRUPT_HANDLER      *TableStart,
  IN UINTN                          *IdtTablePtr,
  IN UINT16                         IdtTableLimit
  );

INTERRUPT_GATE_DESCRIPTOR gIdtTable[INTERRUPT_VECTOR_NUMBER] = { 0 };

INTERRUPT_GATE_DESCRIPTOR gOrigIdtTable[INTERRUPT_VECTOR_NUMBER] = { { 0, 0, 0, 0 } };
UINTN mOrigIdtEntryCount = 0;

ALIGN_16BYTE_BOUNDRY IA32_DESCRIPTOR gLidtPseudoDescriptor = {
  sizeof (gIdtTable) - 1,
  (UINTN) gIdtTable
};

extern EFI_CPU_INTERRUPT_HANDLER   mExternalVectorTable[];
extern EFI_PHYSICAL_ADDRESS        mBackupBuffer;

UINT64   mValidMtrrAddressMask = 0xFFFFFF000;
UINT64   mValidMtrrBitsMask    = 0xFFFFFFFFF;

/**
  Initialize Interrupt descriptor Tables

**/
VOID
InitializeInterruptTables (
  VOID
  )
{
  INTERRUPT_HANDLER_TEMPLATE_MAP  TemplateMap;
  UINT16                          CodeSegment;
  INTERRUPT_GATE_DESCRIPTOR       *IdtEntry;
  UINT8                           *InterruptHandler;
  UINT8                           *CurrentHandler;
  UINTN                           Index;
  IA32_DESCRIPTOR                 IdtrForBsp;

  IdtEntry  = gIdtTable;

  GetTemplateAddressMap (&TemplateMap);
  InterruptHandler  = AllocatePool (TemplateMap.Size * INTERRUPT_VECTOR_NUMBER);
  CurrentHandler    = InterruptHandler;

  CodeSegment       = GetCodeSegment ();

  for (Index = 0; Index < INTERRUPT_VECTOR_NUMBER; Index++) {
    CopyMem (CurrentHandler, TemplateMap.Start, TemplateMap.Size);
    *(UINT32 *) (CurrentHandler + TemplateMap.FixOffset)  = Index;

    IdtEntry[Index].OffsetLow = (UINT16) (UINTN) CurrentHandler;
    IdtEntry[Index].SegmentSelector = CodeSegment;
    IdtEntry[Index].Attributes = INTERRUPT_GATE_ATTRIBUTE;
    //
    // 8e00;
    //
    IdtEntry[Index].OffsetHigh = (UINT16) ((UINTN) CurrentHandler >> 16);

    CurrentHandler += TemplateMap.Size;
  }

  AsmReadIdtr (&IdtrForBsp);

  //
  // Size of the IDT currently programmed (number of entries)
  //
  mOrigIdtEntryCount = (IdtrForBsp.Limit + 1) / sizeof (INTERRUPT_GATE_DESCRIPTOR);

  //
  // Update debugger CS with DxeCpu CS.
  //
  IdtEntry = (INTERRUPT_GATE_DESCRIPTOR *) IdtrForBsp.Base;
  for (Index = 0; Index < mOrigIdtEntryCount; Index++) {
    IdtEntry[Index].SegmentSelector   = CodeSegment;
  }
  //
  // Save a copy of the original IDT
  //
  CopyMem (&gOrigIdtTable, (VOID *) IdtrForBsp.Base, IdtrForBsp.Limit + 1);

  InitializeIdt (
    &(mExternalVectorTable[0]),
    (UINTN *) gIdtTable,
    sizeof (INTERRUPT_GATE_DESCRIPTOR) * INTERRUPT_VECTOR_NUMBER
    );

  return;
}


/**
  Set Interrupt Descriptor Table Handler Address.

  @param[in] Index        The Index of the interrupt descriptor table handle.

**/
VOID
SetInterruptDescriptorTableHandlerAddress (
  IN UINTN Index
  )
{
  IA32_DESCRIPTOR            IdtrForBsp;
  INTERRUPT_GATE_DESCRIPTOR  *IdtEntry;

  AsmReadIdtr (&IdtrForBsp);
  IdtEntry = (INTERRUPT_GATE_DESCRIPTOR *) IdtrForBsp.Base;

  //
  // Plug in CPU Driver version
  //
  CopyMem (&IdtEntry[Index], &gIdtTable[Index], sizeof (INTERRUPT_GATE_DESCRIPTOR));
}


/**
  Restore original Interrupt Descriptor Table Handler Address.

  @param[in] Index        The Index of the interrupt descriptor table handle.

**/
VOID
RestoreInterruptDescriptorTableHandlerAddress (
  IN UINTN       Index
  )
{
  IA32_DESCRIPTOR            IdtrForBsp;
  INTERRUPT_GATE_DESCRIPTOR  *IdtEntry;

  if (Index >= mOrigIdtEntryCount) {
    return;
  }

  AsmReadIdtr (&IdtrForBsp);
  IdtEntry = (INTERRUPT_GATE_DESCRIPTOR *) IdtrForBsp.Base;
  CopyMem (&IdtEntry[Index], &gOrigIdtTable[Index], sizeof (INTERRUPT_GATE_DESCRIPTOR));
}


/**
  Prepare memory for essential system tables.

  @retval   EFI_SUCCESS           Memory successfully prepared.

**/
EFI_STATUS
PrepareMemory (
  VOID
  )
{
  ZeroMem (mExternalVectorTable, 0x100 * 4);

  //
  // Initialize the Interrupt Descriptor Table
  //
  InitializeInterruptTables ();

  return EFI_SUCCESS;
}


/**
  Prepare Wakeup Buffer and stack for APs.

  @param[out] WakeUpBuffer               Pointer to the address of wakeup buffer for output.
  @param[out] StackAddressStart          Pointer to the stack address of APs for output.
  @param[in]  MaximumCPUsForThisSystem   Maximum CPUs in this system.

  @retval     EFI_SUCCESS                Memory successfully prepared for APs.
  @retval     Other                      Error occurred while allocating memory.

**/
EFI_STATUS
PrepareMemoryForAPs (
  OUT EFI_PHYSICAL_ADDRESS       *WakeUpBuffer,
  OUT VOID                       **StackAddressStart,
  IN UINTN                       MaximumCPUsForThisSystem
  )
{
  EFI_STATUS                                  Status;
  MP_ASSEMBLY_ADDRESS_MAP                     AddressMap;

  //
  // Release All APs with a lock and wait for them to retire to rendezvous procedure.
  // We need a 64 aligned 4K aligned area for IA-32 to use broadcast APIs. But we need it only
  // on a temporary basis.
  //
  Status = AllocateWakeUpBuffer (WakeUpBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Claim memory for AP stack.
  //
  Status = gBS->AllocatePool (
                  EfiACPIMemoryNVS,
                  MaximumCPUsForThisSystem * STACK_SIZE_PER_PROC,
                  StackAddressStart
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePages (*WakeUpBuffer, 1);
    return Status;
  }

  AsmGetAddressMap (&AddressMap);
  CopyMem ((VOID *) (UINTN) *WakeUpBuffer, AddressMap.RendezvousFunnelAddress, AddressMap.Size);
  *(UINT32 *) (UINTN) (*WakeUpBuffer + AddressMap.FlatJumpOffset + 3) = (UINT32) (*WakeUpBuffer + AddressMap.PModeEntryOffset);

  return EFI_SUCCESS;
}


/**
  Prepare exchange information for APs.

  @param[out] ExchangeInfo         Pointer to the exchange info buffer for output.
  @param[in]  StackAddressStart    Start address of APs' stacks.
  @param[in]  ApFunction           Address of function assigned to AP.
  @param[in]  WakeUpBuffer         Pointer to the address of wakeup buffer.

  @retval     EFI_SUCCESS          Exchange Info successfully prepared for APs.

**/
EFI_STATUS
PrepareExchangeInfo (
  OUT MP_CPU_EXCHANGE_INFO           *ExchangeInfo,
  IN  VOID                           *StackAddressStart,
  IN  VOID                           *ApFunction,
  IN  EFI_PHYSICAL_ADDRESS           WakeUpBuffer
  )
{
  gBS->SetMem ((VOID *) ExchangeInfo, EFI_PAGE_SIZE - MP_CPU_EXCHANGE_INFO_OFFSET, 0);

  ExchangeInfo->Lock        = VacantFlag;
  ExchangeInfo->StackStart  = StackAddressStart;
  ExchangeInfo->StackSize   = STACK_SIZE_PER_PROC;
  ExchangeInfo->ApFunction  = ApFunction;

  CopyMem (&ExchangeInfo->GdtrProfile, (VOID *) (UINTN) mAcpiCpuData->GdtrProfile, sizeof (IA32_DESCRIPTOR));
  CopyMem (&ExchangeInfo->IdtrProfile, (VOID *) (UINTN) mAcpiCpuData->IdtrProfile, sizeof (IA32_DESCRIPTOR));

  ExchangeInfo->BufferStart = (UINT32) WakeUpBuffer;
  ExchangeInfo->InitFlag    = 1;

  return EFI_SUCCESS;
}


/**
  Prepare Wakeup Buffer and stack for APs during S3.

  @param[out] WakeUpBuffer          Pointer to the address of wakeup buffer for output.
  @param[out] StackAddressStart     Pointer to the stack address of APs for output.

  @retval     EFI_SUCCESS           Memory successfully prepared for APs.

**/
EFI_STATUS
S3PrepareMemoryForAPs (
  OUT EFI_PHYSICAL_ADDRESS       *WakeUpBuffer,
  OUT VOID                       **StackAddressStart
  )
{
  MP_ASSEMBLY_ADDRESS_MAP        AddressMap;

  *WakeUpBuffer      = mAcpiCpuData->WakeUpBuffer;
  *StackAddressStart = (VOID *) (UINTN) mAcpiCpuData->StackAddress;

  AsmGetAddressMap (&AddressMap);
  CopyMem ((VOID *) (UINTN) *WakeUpBuffer, AddressMap.RendezvousFunnelAddress, AddressMap.Size);
  *(UINT32 *) (UINTN) (*WakeUpBuffer + AddressMap.FlatJumpOffset + 3) = (UINT32) (*WakeUpBuffer + AddressMap.PModeEntryOffset);

  return EFI_SUCCESS;
}


/**
  Prepare exchange information for APs during S3.

  @param[out] ExchangeInfo            Pointer to the exchange info for output.
  @param[in]  StackAddressStart       Start address of APs' stacks.
  @param[in]  ApFunction              Address of function assigned to AP.
  @param[in]  WakeUpBuffer            Pointer to the address of wakeup buffer.

  @retval EFI_SUCCESS                 Exchange Info successfully prepared for APs.

**/
EFI_STATUS
S3PrepareExchangeInfo (
  OUT MP_CPU_EXCHANGE_INFO           *ExchangeInfo,
  IN  VOID                           *StackAddressStart,
  IN  VOID                           *ApFunction,
  IN  EFI_PHYSICAL_ADDRESS           WakeUpBuffer
  )
{
  ExchangeInfo->Lock            = VacantFlag;
  ExchangeInfo->StackStart      = (VOID *) (UINTN) StackAddressStart;
  ExchangeInfo->StackSize       = STACK_SIZE_PER_PROC;
  ExchangeInfo->ApFunction      = ApFunction;

  CopyMem (&ExchangeInfo->GdtrProfile, (VOID *) (UINTN) mAcpiCpuData->GdtrProfile, sizeof (IA32_DESCRIPTOR));
  CopyMem (&ExchangeInfo->IdtrProfile, (VOID *) (UINTN) mAcpiCpuData->IdtrProfile, sizeof (IA32_DESCRIPTOR));

  ExchangeInfo->BufferStart     = (UINT32) WakeUpBuffer;
  ExchangeInfo->InitFlag        = 2;

  //
  // There is no need to initialize CpuNumber and BistBuffer fields in ExchangeInfo here.
  //
  return EFI_SUCCESS;
}


/**
  Dynamically write the far jump destination in APs' wakeup buffer,
  in order to refresh APs' CS registers for mode switching.

**/
VOID
RedirectFarJump (
  VOID
  )
{
  MP_ASSEMBLY_ADDRESS_MAP    AddressMap;

  AsmGetAddressMap (&AddressMap);
  *(UINT32 *) (UINTN) (mBackupBuffer + AddressMap.FlatJumpOffset + 3) = (UINT32) (mAcpiCpuData->WakeUpBuffer + AddressMap.PModeEntryOffset);

  return;
}


/**
  Set specified IDT entry with given function pointer.

  @param[in]  FunctionPointer             Function pointer for IDT entry.
  @param[out] IdtEntry                    The IDT entry to update.

  @retval     UINTN                       The original IDT entry value.

**/
UINTN
SetIdtEntry (
  IN  UINTN                       FunctionPointer,
  OUT INTERRUPT_GATE_DESCRIPTOR   *IdtEntry
)
{
  UINTN  OriginalEntry;

  OriginalEntry = ((UINT32) IdtEntry->OffsetHigh << 16) + IdtEntry->OffsetLow;

  IdtEntry->OffsetLow  = (UINT16) FunctionPointer;
  IdtEntry->OffsetHigh = (UINT16) (FunctionPointer >> 16);

  return OriginalEntry;
}


/**
  Prepare GDTR and IDTR for AP

  @param[out]  Gdtr          The GDTR profile
  @param[out]  Idtr          The IDTR profile

  @retval      EFI_STATUS    Status returned by each sub-routine
  @retval      EFI_SUCCESS   GDTR and IDTR has been prepared for AP

**/
EFI_STATUS
PrepareGdtIdtForAP (
  OUT IA32_DESCRIPTOR          *Gdtr,
  OUT IA32_DESCRIPTOR          *Idtr
  )
{
  INTERRUPT_GATE_DESCRIPTOR  *IdtForAP;
  SEGMENT_DESCRIPTOR         *GdtForAP;
  IA32_DESCRIPTOR            *IdtrForBSP;
  IA32_DESCRIPTOR            *GdtrForBSP;
  UINT8                      *MceHandler;
  EFI_STATUS                 Status;

  AsmGetGdtrIdtr (&GdtrForBSP, &IdtrForBSP);

  //
  // Allocate reserved memory for IDT
  //
  Status = AllocateAlignedReservedMemory (
             IdtrForBSP->Limit + 1,
             8,
             (VOID **) &IdtForAP
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Allocate reserved memory for GDT
  //
  Status = AllocateAlignedReservedMemory (
             GdtrForBSP->Limit + 1,
             8,
             (VOID **) &GdtForAP
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->AllocatePool (
                  EfiACPIMemoryNVS,
                  SIZE_OF_MCE_HANDLER,
                  (VOID **) &MceHandler
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // McheHandler content: iret (opcode = 0xcf)
  //
  *MceHandler = 0xCF;

  CopyMem (GdtForAP, (VOID *) GdtrForBSP->Base, GdtrForBSP->Limit + 1);
  CopyMem (IdtForAP, (VOID *) IdtrForBSP->Base, IdtrForBSP->Limit + 1);

  IdtForAP[INTERRUPT_HANDLER_MACHINE_CHECK].OffsetLow   = (UINT16) (UINTN) MceHandler;
  IdtForAP[INTERRUPT_HANDLER_MACHINE_CHECK].OffsetHigh  = (UINT16) ((UINTN) MceHandler >> 16);

  //
  // Create Gdtr, IDTR profile
  //
  Gdtr->Base  = (UINTN) GdtForAP;
  Gdtr->Limit = GdtrForBSP->Limit;

  Idtr->Base  = (UINTN) IdtForAP;
  Idtr->Limit = IdtrForBSP->Limit;

  return EFI_SUCCESS;
}

