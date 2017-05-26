/** @file
  Prototypes and defines for the SC SMM Dispatcher.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SMM_H_
#define _SC_SMM_H_

#include <PiSmm.h>
#include <Uefi.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmGpiDispatch2.h>
#include <Protocol/SmmIchnDispatch.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PerformanceLib.h>
#include <Protocol/SmmIchnDispatchEx.h>
#include <ScAccess.h>
#include <Library/GpioLib.h>

#if defined (X64_BUILD_SUPPORT) && (X64_BUILD_SUPPORT == 1)
#define EFI_BAD_POINTER          0xAFAFAFAFAFAFAFAFULL
#else
#define EFI_BAD_POINTER          0xAFAFAFAFUL
#endif

///
/// Define an enumeration for all the supported protocols
///
typedef enum {
  UsbType,
  SxType,
  SwType,
  GpiType,
  IchnType,
  IchnExType,
  PowerButtonType,
  PeriodicTimerType,
  ScSmmProtocolTypeMax
} SC_SMM_PROTOCOL_TYPE;

///
/// SPECIFYING A REGISTER
/// We want a general way of referring to addresses.  For this case, we'll only
/// need addresses in the ACPI table (and the TCO entries within the ACPI table).
/// However, it's interesting to consider what it would take to support other types
/// of addresses.  To address Will's concern, I think it prudent to accommodate it
/// early on in the design.
///
/// Addresses we need to consider:
///
///  Type:                           Required:
///  I/O                             Yes
///  ACPI   (special case of I/O)    Only if we want to
///  TCO    (special case of ACPI)   Only if we want to
///  GPIO   (special case of MMIO)   Only if we want to
///  Memory (or Memory Mapped I/O)   Only if we want to
///
#ifdef PCIESC_SUPPORT
typedef enum {
  ACPI_ADDR_TYPE,
  GPIO_ADDR_TYPE,
  MEMORY_MAPPED_IO_ADDRESS_TYPE,
  PCIE_ADDR_TYPE,
  PCR_ADDR_TYPE,
  NUM_ADDR_TYPES,                     ///< count of items in this enum
  SC_SMM_ADDR_TYPE_NULL        = -1   ///< sentinel to indicate NULL or to signal end of arrays
} ADDR_TYPE;
#else
typedef enum {
  ACPI_ADDR_TYPE,
  GPIO_ADDR_TYPE,
  MEMORY_MAPPED_IO_ADDRESS_TYPE,
  PCR_ADDR_TYPE,
  NUM_ADDR_TYPES,                     ///< count of items in this enum
  SC_SMM_ADDR_TYPE_NULL        = -1   ///< sentinel to indicate NULL or to signal end of arrays
} ADDR_TYPE;
#endif

///
/// Assumption: 32-bits -- enum's evaluate to integer
/// Assumption: This code will only run on IA-32.  Justification: IA-64 doesn't have SMIs.
/// We don't have to worry about 64-bit addresses.
/// Typedef the size of addresses in case the numbers I'm using are wrong or in case
/// this changes.  This is a good idea because PCI_ADDR will change, for example, when
/// we add support for PciExpress.
///
typedef UINT16 IO_ADDR;
typedef IO_ADDR ACPI_ADDR;  // can omit
typedef IO_ADDR TCO_ADDR;   // can omit
typedef UINTN MEM_ADDR;
typedef MEM_ADDR *MEMORY_MAPPED_IO_ADDRESS;
typedef MEM_ADDR *GPIO_ADDR;

#ifdef PCIESC_SUPPORT
typedef union {
  UINT32  Raw;
  struct {
    UINT8 Reg;
    UINT8 Fnc;
    UINT8 Dev;
    UINT8 Bus;
  } Fields;
} PCIE_ADDR;
#endif

typedef union {
  UINT32  Raw;
  struct {
    UINT16 Offset;
    UINT8  Pid;
    UINT8  Base;
  } Fields;
} PCR_ADDR;

typedef struct {
  ADDR_TYPE Type;
  union {
    ///
    /// used to initialize during declaration/definition
    ///
    UINT32                    raw;

    ///
    /// used to access useful data
    ///
    IO_ADDR                   io;
    ACPI_ADDR                 acpi;
    TCO_ADDR                  tco;
    MEM_ADDR                  mem;
    MEMORY_MAPPED_IO_ADDRESS  Mmio;
#ifdef PCIESC_SUPPORT
    PCIE_ADDR                 pcie;
#endif
    PCR_ADDR                  Pcr;
  } Data;

} SC_SMM_ADDRESS;

///
/// SPECIFYING BITS WITHIN A REGISTER
/// Here's a struct that helps us specify a source or enable bit.
///
typedef struct {
  SC_SMM_ADDRESS Reg;
  UINT8          SizeInBytes;
  UINT8          Bit;
} SC_SMM_BIT_DESC;

///
/// Sometimes, we'll have bit descriptions that are unused.  It'd be great to have a
/// way to easily identify them:
///
#define IS_BIT_DESC_NULL(BitDesc)   ((BitDesc).Reg.Type == SC_SMM_ADDR_TYPE_NULL)  // "returns" true when BitDesc is NULL
#define NULL_THIS_BIT_DESC(BitDesc) ((BitDesc).Reg.Type = SC_SMM_ADDR_TYPE_NULL)   // will "return" an integer w/ value of 0
#define NULL_BIT_DESC_INITIALIZER \
  { \
    { \
      SC_SMM_ADDR_TYPE_NULL, \
      { \
        0 \
      } \
    }, \
    0, 0 \
  }
//
// The specify the callback's Sts & En bits because they'll
// be commonly used together:
//
#define NUM_EN_BITS   2
#define NUM_STS_BITS  1

//
// Flags
//
typedef UINT8 SC_SMM_SOURCE_FLAGS;

//
// Flags required today
//
#define SC_SMM_NO_FLAGS          0
#define SC_SMM_SCI_EN_DEPENDENT  1

//
// Flags that might be required tomorrow
//
typedef struct {
  SC_SMM_SOURCE_FLAGS  Flags;
  SC_SMM_BIT_DESC      En[NUM_EN_BITS];    ///< Describes the enable bit(s) for the SMI event
  SC_SMM_BIT_DESC      Sts[NUM_STS_BITS];  ///< Describes the secondary status bit for the SMI event. Might be the same as TopLevelSmi
} SC_SMM_SOURCE_DESC;

///
/// Used to initialize null source descriptor
///
#define NULL_SOURCE_DESC_INITIALIZER \
  { \
    SC_SMM_NO_FLAGS, \
    { \
      NULL_BIT_DESC_INITIALIZER, NULL_BIT_DESC_INITIALIZER \
    }, \
    { \
      NULL_BIT_DESC_INITIALIZER \
    } \
  }

///
/// CHILD CONTEXTS
/// To keep consistent w/ the architecture, we'll need to provide the context
/// to the child when we call its callback function.  After talking with Will,
/// we agreed that we'll need functions to "dig" the context out of the hardware
/// in many cases (Sx, Trap, Gpi, etc), and we'll need a function to compare those
/// contexts to prevent unnecessary dispatches.  The general type for these
/// "GetContext" functions, so a union of all the protocol contexts for
/// our internal use:
///
typedef union {
  //
  // (in no particular order)
  //
  EFI_SMM_ICHN_DISPATCH_CONTEXT           Ichn;
  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT        IchnEx;
  EFI_SMM_SX_REGISTER_CONTEXT             Sx;
  EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT PeriodicTimer;
  EFI_SMM_SW_REGISTER_CONTEXT             Sw;
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT   PowerButton;
  EFI_SMM_USB_REGISTER_CONTEXT            Usb;
  EFI_SMM_GPI_REGISTER_CONTEXT            Gpi;
} SC_SMM_CONTEXT;

typedef union {
  UINTN ElapsedTime;
} SC_SMM_MISC_DATA;

typedef struct _DATABASE_RECORD DATABASE_RECORD;

/**
  Get SMM context address

  @param[in]  Record         The Database address
  @param[out] Context        The Smm Context address.

  @retval     None

**/
typedef
VOID
(EFIAPI *GET_CONTEXT) (
  IN  DATABASE_RECORD    *Record,
  OUT SC_SMM_CONTEXT     *Context
  );

/**
  Compare two buffer Context

  @param[IN] Context1        Context1 buffer address.
  @param[IN] Context2        Context2 buffer address.

  @retval    TRUE            Context1 and Context2 is same.
  @retval    FALSE           Context1 and Context2 is diffrent.

**/
typedef
BOOLEAN
(EFIAPI *CMP_CONTEXT) (
  IN SC_SMM_CONTEXT     *Context1,
  IN SC_SMM_CONTEXT     *Context2
  );

typedef
VOID
(EFIAPI *GET_COMMBUFFER) (
  IN  DATABASE_RECORD    *Record,
  OUT VOID               **CommBuffer,
  OUT UINTN              *CommBufferSize
 );

///
/// Finally, every protocol will require a "Get Context" and "Compare Context" call, so
/// we may as well wrap that up in a table, too.
///
typedef struct {
  GET_CONTEXT GetContext;
  CMP_CONTEXT CmpContext;
  GET_COMMBUFFER  GetCommBuffer;
} CONTEXT_FUNCTIONS;

extern CONTEXT_FUNCTIONS          ContextFunctions[ScSmmProtocolTypeMax];

/**
  Maps a USB context to a source description.

  @param[in]  Context             The context we need to map.  Type must be USB.
  @param[out] SrcDesc             The source description that corresponds to the given context.

**/
VOID
MapUsbToSrcDesc (
  IN  SC_SMM_CONTEXT              *Context,
  OUT SC_SMM_SOURCE_DESC          *SrcDesc
  );

/**
  Figure out which timer the child is requesting and
  send back the source description

  @param[in]  DispatchContext     The pointer to the Dispatch Context instances
  @param[out] SrcDesc             The pointer to the source description

**/
VOID
MapPeriodicTimerToSrcDesc (
  IN  SC_SMM_CONTEXT              *DispatchContext,
  OUT SC_SMM_SOURCE_DESC          *SrcDesc
  );

///
/// Mapping simple contexts can be done by assignment or lookup table
///
extern CONST SC_SMM_SOURCE_DESC  SW_SOURCE_DESC;
extern CONST SC_SMM_SOURCE_DESC  SX_SOURCE_DESC;
extern CONST SC_SMM_SOURCE_DESC  POWER_BUTTON_SOURCE_DESC;

///
/// With the changes we've made to the protocols, we can now use table
/// lookups for the following protocols:
///
extern CONST SC_SMM_SOURCE_DESC  SC_GPI_SOURCE_DESC_TEMPLATE;
extern SC_SMM_SOURCE_DESC        ICHN_SOURCE_DESCS[NUM_ICHN_TYPES];
extern SC_SMM_SOURCE_DESC        ICHN_EX_SOURCE_DESCS[IchnExTypeMAX - IchnExPciExpress];

///
/// For PCHx, APMC is UINT8 port, so the MAX SWI Value is 0xFF.
///
#define MAXIMUM_SWI_VALUE 0xFF

///
/// GENERALIZING THE CALLBACK
/// All SmmXxxDispatch callbacks have the same form:
///
/// VOID Callback( EFI_HANDLE, EFI_SMM_Xxx_DISPATCH_CONTEXT )
/// We need a typedef that'll allow us to call any callback
///
/**
  Clear the SMI status bit by set the source bit of SMI status register

  @param[in] SrcDesc         Pointer to the SC SMI source description table

  @return    None

**/
typedef
VOID
(EFIAPI *SC_SMM_CLEAR_SOURCE) (
  IN SC_SMM_SOURCE_DESC *SrcDesc
  );

///
/// "DATABASE" RECORD
/// Linked list data structures
///
#define DATABASE_RECORD_SIGNATURE SIGNATURE_32 ('D', 'B', 'R', 'C')

typedef struct _DATABASE_RECORD {
  UINT32                Signature;
  LIST_ENTRY            Link;
  BOOLEAN               Processed;

  ///
  /// Status and Enable bit description
  ///
  SC_SMM_SOURCE_DESC    SrcDesc;

  ///
  /// Callback function
  ///
  EFI_SMM_HANDLER_ENTRY_POINT2  Callback;
  SC_SMM_CONTEXT        ChildContext;

  ///
  /// Special handling hooks -- init them to NULL if unused/unneeded
  ///
  SC_SMM_CLEAR_SOURCE   ClearSource;

  ///
  /// Functions required to make callback code general
  ///
  CONTEXT_FUNCTIONS     ContextFunctions;

  ///
  /// The protocol that this record dispatches
  ///
  SC_SMM_PROTOCOL_TYPE  ProtocolType;

  ///
  /// Misc data for private usage
  ///
  SC_SMM_MISC_DATA     MiscData;

} DATABASE_RECORD;

#define DATABASE_RECORD_FROM_LINK(_record)  CR (_record, DATABASE_RECORD, Link, DATABASE_RECORD_SIGNATURE)
#define DATABASE_RECORD_FROM_CHILDCONTEXT(_record)  CR (_record, DATABASE_RECORD, ChildContext, DATABASE_RECORD_SIGNATURE)

/**
  Register a South Cluster child SMI dispatch function with a parent SMM driver.

  @param[in]  This                    Pointer to the SC_SMM_GENERIC_PROTOCOL instance.
  @param[in]  DispatchFunction        Pointer to dispatch function to be invoked for this SMI source.
  @param[in]  DispatchContext         Pointer to the dispatch function's context.
  @param[out] DispatchHandle          Handle of dispatch function, for when interfacing
                                      with the parent SMM driver, will be the address of linked
                                      list link in the call back record.

  @retval     EFI_OUT_OF_RESOURCES    Insufficient resources to create database record
  @retval     EFI_INVALID_PARAMETER   The input parameter is invalid
  @retval     EFI_SUCCESS             The dispatch function has been successfully
                                      registered and the SMI source has been enabled.

**/
typedef
EFI_STATUS
(EFIAPI *SC_SMM_GENERIC_REGISTER) (
  IN  VOID                             **This,
  IN  VOID                             *DispatchFunction,
  IN  VOID                             *DispatchContext,
  OUT EFI_HANDLE                       *DispatchHandle
  );

/**
  Unregister a South Cluster child SMI source dispatch function with a parent SMM driver.

  @param[in] This                    Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchHandle          Handle of dispatch function to deregister.

  @retval    EFI_SUCCESS             The dispatch function has been successfully
                                     unregistered and the SMI source has been disabled
                                     if there are no other registered child dispatch
                                     functions for this SMI source.
  @retval    EFI_INVALID_PARAMETER   Handle is invalid.
  @retval    EFI_SUCCESS             The function has been successfully unregistered child SMI source.

**/
typedef
EFI_STATUS
(EFIAPI *SC_SMM_GENERIC_UNREGISTER) (
  IN  VOID                            **This,
  IN  EFI_HANDLE                      DispatchHandle
  );

///
/// Define a memory "stamp" equivalent in size and function to most of the protocols
///
typedef struct {
  SC_SMM_GENERIC_REGISTER     Register;
  SC_SMM_GENERIC_UNREGISTER   Unregister;
  UINTN                       Extra1;
  UINTN                       Extra2; // may not need this one
} SC_SMM_GENERIC_PROTOCOL;

/**
  Register a child SMI dispatch function with a parent SMM driver.

  @param[in]  This                    Pointer to the SC_SMM_GENERIC_PROTOCOL instance.
  @param[in]  DispatchFunction        Pointer to dispatch function to be invoked for this SMI source.
  @param[in]  DispatchContext         Pointer to the dispatch function's context.
  @param[out] DispatchHandle          Handle of dispatch function, for when interfacing
                                      with the parent SMM driver, will be the address of linked
                                      list link in the call back record.

  @retval     EFI_OUT_OF_RESOURCES    Insufficient resources to create database record
  @retval     EFI_INVALID_PARAMETER   The input parameter is invalid
  @retval     EFI_SUCCESS             The dispatch function has been successfully
                                      registered and the SMI source has been enabled.

**/
EFI_STATUS
EFIAPI
ScSmmCoreRegister (
  IN  SC_SMM_GENERIC_PROTOCOL          *This,
  IN  EFI_SMM_HANDLER_ENTRY_POINT2     DispatchFunction,
  IN  SC_SMM_CONTEXT                   *DispatchContext,
  OUT EFI_HANDLE                       *DispatchHandle
  );

/**

  Unregister a child SMI source dispatch function with a parent SMM driver.

  @param[in] This                    Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchHandle          Handle of dispatch function to deregister.

  @retval    EFI_SUCCESS             The dispatch function has been successfully
                                     unregistered and the SMI source has been disabled
                                     if there are no other registered child dispatch
                                     functions for this SMI source.
  @retval    EFI_INVALID_PARAMETER   Handle is invalid.
  @retval    EFI_SUCCESS             The function has been successfully unregistered child SMI source.

**/
EFI_STATUS
EFIAPI
ScSmmCoreUnRegister (
  IN  SC_SMM_GENERIC_PROTOCOL        *This,
  IN  EFI_HANDLE                     *DispatchHandle
  );

///
/// South Cluster SMM Protocol
///
typedef union {
  SC_SMM_GENERIC_PROTOCOL                     Generic;

  EFI_SMM_USB_DISPATCH2_PROTOCOL              Usb;
  EFI_SMM_SX_DISPATCH2_PROTOCOL               Sx;
  EFI_SMM_SW_DISPATCH2_PROTOCOL               Sw;
  EFI_SMM_GPI_DISPATCH2_PROTOCOL              Gpi;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL     PowerButton;
  EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL   PeriodicTimer;
  EFI_SMM_ICHN_DISPATCH_PROTOCOL              Ichn;
  EFI_SMM_ICHN_DISPATCH_EX_PROTOCOL           IchnEx;
} SC_SMM_PROTOCOL;

///
/// Define a structure to help us identify the generic protocol
///
#define PROTOCOL_SIGNATURE  SIGNATURE_32 ('P', 'R', 'O', 'T')

typedef struct {
  UINTN                 Signature;

  SC_SMM_PROTOCOL_TYPE  Type;
  EFI_GUID              *Guid;
  SC_SMM_PROTOCOL       Protocols;
} SC_SMM_QUALIFIED_PROTOCOL;

#define QUALIFIED_PROTOCOL_FROM_GENERIC(_generic) \
  CR ( \
  _generic, \
  SC_SMM_QUALIFIED_PROTOCOL, \
  Protocols, \
  PROTOCOL_SIGNATURE \
  )

///
/// Create private data for the protocols that we'll publish
///
typedef struct {
  LIST_ENTRY                  CallbackDataBase;
  EFI_HANDLE                  SmiHandle;
  EFI_HANDLE                  InstallMultProtHandle;
  SC_SMM_QUALIFIED_PROTOCOL   Protocols[ScSmmProtocolTypeMax];
} PRIVATE_DATA;

extern PRIVATE_DATA           mPrivateData;
extern UINT16                 AcpiBaseAddr;

/**
  Get the Software Smi value

  @param[in]  Record              No use
  @param[out] Context             The context that includes Software Smi value to be filled

**/
VOID
EFIAPI
SwGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT SC_SMM_CONTEXT     *Context
  );

/**
  Check whether software SMI value of two contexts match

  @param[in] Context1             Context 1 that includes software SMI value 1
  @param[in] Context2             Context 2 that includes software SMI value 2

  @retval    FALSE                Software SMI value match
  @retval    TRUE                 Software SMI value don't match

**/
BOOLEAN
EFIAPI
SwCmpContext (
  IN SC_SMM_CONTEXT               *Context1,
  IN SC_SMM_CONTEXT               *Context2
  );

/**
  Gather the CommBuffer information of SmmSwDispatch2.

  @param[in]  Record              No use
  @param[out] CommBuffer          Point to the CommBuffer structure
  @param[out] CommBufferSize      Point to the Size of CommBuffer structure

**/
VOID
EFIAPI
SwGetCommBuffer (
  IN  DATABASE_RECORD             *Record,
  OUT VOID                        **CommBuffer,
  OUT UINTN                       *CommBufferSize
  );

/**
  Get the Sleep type

  @param[in]  Record              No use
  @param[out] Context             The context that includes SLP_TYP bits to be filled

**/
VOID
EFIAPI
SxGetContext (
  IN  DATABASE_RECORD            *Record,
  OUT SC_SMM_CONTEXT             *Context
  );

/**
  Init required protocol for Pch Sw Dispatch protocol.

**/
VOID
ScSwDispatchInit (
  VOID
  );

/**
  Check whether sleep type of two contexts match

  @param[in] Context1             Context 1 that includes sleep type 1
  @param[in] Context2             Context 2 that includes sleep type 2

  @retval    FALSE                Sleep types match
  @retval    TRUE                 Sleep types don't match

**/
BOOLEAN
EFIAPI
SxCmpContext (
  IN SC_SMM_CONTEXT               *Context1,
  IN SC_SMM_CONTEXT               *Context2
  );

/**
  Update the elapsed time from the Interval data of DATABASE_RECORD

  @param[in]  Record              The pointer to the DATABASE_RECORD.
  @param[out] HwContext           The Context to be updated.

**/
VOID
EFIAPI
PeriodicTimerGetContext (
  IN  DATABASE_RECORD             *Record,
  OUT SC_SMM_CONTEXT              *Context
  );

/**
  Check whether Periodic Timer of two contexts match

  @param[in] Context1             Context 1 that includes Periodic Timer  1
  @param[in] Context2             Context 2 that includes Periodic Timer  2

  @retval    FALSE                Periodic Timer match
  @retval    TRUE                 Periodic Timer don't match

**/
BOOLEAN
EFIAPI
PeriodicTimerCmpContext (
  IN SC_SMM_CONTEXT               *Context1,
  IN SC_SMM_CONTEXT               *Context2
  );

/**
  Gather the CommBuffer information of SmmPeriodicTimerDispatch2.

  @param[in]  Record              No use
  @param[out] CommBuffer          Point to the CommBuffer structure
  @param[out] CommBufferSize      Point to the Size of CommBuffer structure

**/
VOID
EFIAPI
PeriodicTimerGetCommBuffer (
  IN  DATABASE_RECORD             *Record,
  OUT VOID                        **CommBuffer,
  OUT UINTN                       *CommBufferSize
  );

/**
  Get the power button status.

  @param[in]  Record              The pointer to the DATABASE_RECORD.
  @param[out] Context             Calling context from the hardware, will be updated with the current power button status.

**/
VOID
EFIAPI
PowerButtonGetContext (
  IN  DATABASE_RECORD             *Record,
  OUT SC_SMM_CONTEXT              *Context
  );

/**
  Check whether Power Button status of two contexts match

  @param[in] Context1             Context 1 that includes Power Button status 1
  @param[in] Context2             Context 2 that includes Power Button status 2

  @retval    FALSE                Power Button status match
  @retval    TRUE                 Power Button status don't match

**/
BOOLEAN
EFIAPI
PowerButtonCmpContext (
  IN SC_SMM_CONTEXT               *Context1,
  IN SC_SMM_CONTEXT               *Context2
  );

/**
  This function is responsible for calculating and enabling any timers that are required
  to dispatch messages to children. The SrcDesc argument isn't acutally used.

  @param[in] SrcDesc              Pointer to the SC_SMM_SOURCE_DESC instance.

**/
VOID
EFIAPI
ScSmmPeriodicTimerClearSource (
  IN SC_SMM_SOURCE_DESC *SrcDesc
  );

/**
  This services returns the next SMI tick period that is supported by the chipset.
  The order returned is from longest to shortest interval period.

  @param[in]      This                    Pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL instance.
  @param[in, out] SmiTickInterval         Pointer to pointer of the next shorter SMI interval period that is supported by the child.

  @retval         EFI_SUCCESS             The service returned successfully.
  @retval         EFI_INVALID_PARAMETER   The parameter SmiTickInterval is invalid.

**/
EFI_STATUS
EFIAPI
ScSmmPeriodicTimerDispatchGetNextShorterInterval (
  IN CONST EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL    *This,
  IN OUT UINT64                                         **SmiTickInterval
  );

VOID
ScSmmSxGoToSleep (
  VOID
  );

/**
  Clear the SMI status bit after the SMI handling is done

  @param[in] SrcDesc              Pointer to the SC SMI source description table

**/
VOID
EFIAPI
ScSmmIchnClearSource (
  IN SC_SMM_SOURCE_DESC *SrcDesc
  );

#endif

