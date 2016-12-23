/** @file
  IA-32 Exception Handler.

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseMemoryLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include "CpuInitDxe.h"
#include "MpCommon.h"
#include "Exception.h"
#include <Library/BaseMemoryLib.h>

typedef
VOID
(*EFI_INSTALL_EXCEPTION) (
  IN UINT32  InterruptType,
  IN VOID    *SystemContext
  );

typedef struct {
  UINT32  ErrorMessage;
  UINT8   Interrupt;
} EFI_EXCEPTION_HANDLER;

//
// Error code flag indicating whether or not an error code will be
// pushed on the stack if an exception occurs.
//
// 1 means an error code will be pushed, otherwise 0
//
// bit 0 - exception 0
// bit 1 - exception 1
// etc.
//
UINT32 mErrorCodeFlag = 0x00027d00;

//
// Local Table
//
EFI_EXCEPTION_HANDLER mExceptionTable[] = {
  {
    EFI_SW_EC_IA32_DIVIDE_ERROR,
    INTERRUPT_HANDLER_DIVIDE_ZERO
  },
  {
    EFI_SW_EC_IA32_DEBUG,
    INTERRUPT_HANDLER_DEBUG
  },
  {
    EFI_SW_EC_IA32_NMI,
    INTERRUPT_HANDLER_NMI
  },
  {
    EFI_SW_EC_IA32_BREAKPOINT,
    INTERRUPT_HANDLER_BREAKPOINT
  },
  {
    EFI_SW_EC_IA32_OVERFLOW,
    INTERRUPT_HANDLER_OVERFLOW
  },
  {
    EFI_SW_EC_IA32_BOUND,
    INTERRUPT_HANDLER_BOUND
  },
  {
    EFI_SW_EC_IA32_INVALID_OPCODE,
    INTERRUPT_HANDLER_INVALID_OPCODE
  },
  //
  // Interrupt 7, 9, 15 not defined in the debug support protocol. Hence no status codes for them!
  //
  {
    EFI_SW_EC_IA32_DOUBLE_FAULT,
    INTERRUPT_HANDLER_DOUBLE_FAULT
  },
  {
    EFI_SW_EC_IA32_INVALID_TSS,
    INTERRUPT_HANDLER_INVALID_TSS
  },
  {
    EFI_SW_EC_IA32_SEG_NOT_PRESENT,
    INTERRUPT_HANDLER_SEGMENT_NOT_PRESENT
  },
  {
    EFI_SW_EC_IA32_STACK_FAULT,
    INTERRUPT_HANDLER_STACK_SEGMENT_FAULT
  },
  {
    EFI_SW_EC_IA32_GP_FAULT,
    INTERRUPT_HANDLER_GP_FAULT
  },
  {
    EFI_SW_EC_IA32_PAGE_FAULT,
    INTERRUPT_HANDLER_PAGE_FAULT
  },
  {
    EFI_SW_EC_IA32_FP_ERROR,
    INTERRUPT_HANDLER_MATH_FAULT
  },
  {
    EFI_SW_EC_IA32_ALIGNMENT_CHECK,
    INTERRUPT_HANDLER_ALIGNMENT_FAULT
  },
  {
    EFI_SW_EC_IA32_MACHINE_CHECK,
    INTERRUPT_HANDLER_MACHINE_CHECK
  },
  {
    EFI_SW_EC_IA32_SIMD,
    INTERRUPT_HANDLER_STREAMING_SIMD
  }
};

UINTN mExceptionNumber = sizeof (mExceptionTable) / sizeof (EFI_EXCEPTION_HANDLER);

CPU_STATUS_CODE_TEMPLATE  mStatusCodeData  =  {
  {
    sizeof (EFI_STATUS_CODE_DATA),
    sizeof (EFI_SYSTEM_CONTEXT_IA32),
    { 0 }
  },
  {
    0
  }
};

/**
  Reports StatusCode for Exception

  This function reports status code for exception.

  @param[in]  InterruptType   Interrupt type
  @param[in]  SystemContext   EFI_SYSTEM_CONTEXT

**/
EFI_STATUS
ReportData (
  IN EFI_EXCEPTION_TYPE   InterruptType,
  IN EFI_SYSTEM_CONTEXT   SystemContext
  )
{
  UINT32                ErrorMessage;
  UINT32                Index;

  CopyMem (
    &mStatusCodeData.SystemContext.SystemContextIa32,
    SystemContext.SystemContextIa32,
    sizeof (EFI_SYSTEM_CONTEXT_IA32)
    );

  ErrorMessage = EFI_SOFTWARE_DXE_BS_DRIVER;
  for (Index = 0; Index < mExceptionNumber; Index++) {
    if (mExceptionTable[Index].Interrupt == InterruptType) {
      ErrorMessage |= mExceptionTable[Index].ErrorMessage;
      break;
    }
  }

  ReportStatusCode (
    (EFI_ERROR_CODE | EFI_ERROR_UNRECOVERED),
    EFI_SOFTWARE_UNSPECIFIED | ErrorMessage
    );

  return EFI_SUCCESS;
}


/**
  Common exception handler.

  @param[in] InterruptType  Exception type.
  @param[in] SystemContext  Pointer to EFI_SYSTEM_CONTEXT.

**/
VOID
EFIAPI
CommonExceptionHandler (
  IN EFI_EXCEPTION_TYPE   InterruptType,
  IN EFI_SYSTEM_CONTEXT   SystemContext
  )
{
  DEBUG ((
    DEBUG_ERROR,
    "!!!! IA32 Exception Type - %08x !!!!\n",
    InterruptType
    ));
  DEBUG ((
    DEBUG_ERROR,
    "EIP - %08x, CS - %08x, EFLAGS - %08x\n",
    SystemContext.SystemContextIa32->Eip,
    SystemContext.SystemContextIa32->Cs,
    SystemContext.SystemContextIa32->Eflags
    ));
  if (mErrorCodeFlag & (1 << InterruptType)) {
    DEBUG ((
      DEBUG_ERROR,
      "ExceptionData - %08x\n",
      SystemContext.SystemContextIa32->ExceptionData
      ));
  }
  DEBUG ((
    DEBUG_ERROR,
    "EAX - %08x, ECX - %08x, EDX - %08x, EBX - %08x\n",
    SystemContext.SystemContextIa32->Eax,
    SystemContext.SystemContextIa32->Ecx,
    SystemContext.SystemContextIa32->Edx,
    SystemContext.SystemContextIa32->Ebx
    ));
  DEBUG ((
    DEBUG_ERROR,
    "ESP - %08x, EBP - %08x, ESI - %08x, EDI - %08x\n",
    SystemContext.SystemContextIa32->Esp,
    SystemContext.SystemContextIa32->Ebp,
    SystemContext.SystemContextIa32->Esi,
    SystemContext.SystemContextIa32->Edi
    ));
  DEBUG ((
    DEBUG_ERROR,
    "DS - %08x, ES - %08x, FS - %08x, GS - %08x, SS - %08x\n",
    SystemContext.SystemContextIa32->Ds,
    SystemContext.SystemContextIa32->Es,
    SystemContext.SystemContextIa32->Fs,
    SystemContext.SystemContextIa32->Gs,
    SystemContext.SystemContextIa32->Ss
    ));
  DEBUG ((
    DEBUG_ERROR,
    "GDTR - %08x %08x, IDTR - %08x %08x\n",
    SystemContext.SystemContextIa32->Gdtr[0],
    SystemContext.SystemContextIa32->Gdtr[1],
    SystemContext.SystemContextIa32->Idtr[0],
    SystemContext.SystemContextIa32->Idtr[1]
    ));
  DEBUG ((
    DEBUG_ERROR,
    "LDTR - %08x, TR - %08x\n",
    SystemContext.SystemContextIa32->Ldtr,
    SystemContext.SystemContextIa32->Tr
    ));
  DEBUG ((
    DEBUG_ERROR,
    "CR0 - %08x, CR2 - %08x, CR3 - %08x, CR4 - %08x\n",
    SystemContext.SystemContextIa32->Cr0,
    SystemContext.SystemContextIa32->Cr2,
    SystemContext.SystemContextIa32->Cr3,
    SystemContext.SystemContextIa32->Cr4
    ));
  DEBUG ((
    DEBUG_ERROR,
    "DR0 - %08x, DR1 - %08x, DR2 - %08x, DR3 - %08x\n",
    SystemContext.SystemContextIa32->Dr0,
    SystemContext.SystemContextIa32->Dr1,
    SystemContext.SystemContextIa32->Dr2,
    SystemContext.SystemContextIa32->Dr3
    ));
  DEBUG ((
    DEBUG_ERROR,
    "DR6 - %08x, DR7 - %08x\n",
    SystemContext.SystemContextIa32->Dr6,
    SystemContext.SystemContextIa32->Dr7
    ));

  //
  // Report Status Code
  //
  ReportData (InterruptType, SystemContext);

  //
  // Use this macro to hang so that the compiler does not optimize out
  // the following RET instructions. This allows us to return if we
  // have a debugger attached.
  //
  CpuDeadLoop ();

  return ;
}

/**
  Install the IA-32 EM64T Exception Handler.
  The current operation (which likely will change) will uninstall all the
  pertinent exception handlers (0-7, 10-14, 16-19) except for Int8 which the timer
  is currently sitting on (or soon will be).

  It then installs all the appropriate handlers for each exception.

  The handler then calls gRT->ReportStatusCode with a specific progress code.  The
  progress codes for now start at 0x200 for IA-32 processors. See Status Code
  Specification for details. The Status code Specification uses the enumeration from
  the EFI 1.1 Debug Support Protocol.

  @param[in]   CpuProtocol    Instance of CPU Arch Protocol

  @retval      EFI_SUCCESS    This function always return success after registering handlers.

**/
EFI_STATUS
InitializeException (
  IN  EFI_CPU_ARCH_PROTOCOL *CpuProtocol
  )
{
  EFI_STATUS  Status;
  UINTN       Index;

  mStatusCodeData.Header.Type = gCpuStatusCodeDataTypeExceptionHandlerGuid;
  Status = CpuProtocol->DisableInterrupt (CpuProtocol);

  for (Index = 0; Index < mExceptionNumber; Index ++) {
    Status = CpuProtocol->RegisterInterruptHandler (
                            CpuProtocol,
                            mExceptionTable[Index].Interrupt,
                            NULL
                            );

    Status = CpuProtocol->RegisterInterruptHandler (
                            CpuProtocol,
                            mExceptionTable[Index].Interrupt,
                            CommonExceptionHandler
                            );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

