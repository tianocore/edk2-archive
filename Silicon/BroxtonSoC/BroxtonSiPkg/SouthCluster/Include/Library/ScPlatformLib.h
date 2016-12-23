/** @file
  Header file for ScPlatform Lib.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_PLATFORM_LIB_H_
#define _SC_PLATFORM_LIB_H_

#include <ScAccess.h>
#include <Uefi/UefiBaseType.h>

typedef struct {
  UINT8 DevNum;
  UINT8 Pid;
  UINT8 RpNumBase;
} PCIE_CONTROLLER_INFO;

/**
  Get Sc Maximum Pcie Root Port Number.

  @retval UINT8                         Sc Maximum Pcie Root Port Number

**/
UINT8
EFIAPI
GetScMaxPciePortNum (
  VOID
  );

/**
  Delay for at least the request number of microseconds.
  This function would be called by runtime driver, please do not use any MMIO marco here.

  @param[in]  Microseconds               Number of microseconds to delay.

**/
VOID
EFIAPI
ScPmTimerStall (
  IN  UINTN   Microseconds
  );

/**
  Check whether SPI is in descriptor mode.

  @param[in] SpiBase                       The SC SPI Base Address

  @retval    TRUE                          SPI is in descriptor mode
  @retval    FALSE                         SPI is not in descriptor mode

**/
BOOLEAN
EFIAPI
ScIsSpiDescriptorMode (
  IN  UINTN   SpiBase
  );

/**
  Determine if SC is supported.

  @retval TRUE                          SC is supported
  @retval FALSE                         SC is not supported

**/
BOOLEAN
IsScSupported (
  VOID
  );

/**
  This function can be called to poll for certain value within a time given.

  @param[in] MmioAddress                   The Mmio Address.
  @param[in] BitMask                       Bits to be masked.
  @param[in] BitValue                      Value to be polled.
  @param[in] DelayTime                     Delay time in terms of 100 micro seconds.

  @retval    EFI_SUCCESS                   Successfully polled the value.
  @retval    EFI_TIMEOUT                   Timeout while polling the value.

**/
EFI_STATUS
EFIAPI
ScMmioPoll32 (
  IN  UINTN         MmioAddress,
  IN  UINT32        BitMask,
  IN  UINT32        BitValue,
  IN  UINT16        DelayTime
  );

/**
  Get SC Pcie Root Port Device and Function Number by Root Port physical Number.

  @param[in]  RpNumber                  Root port physical number. (0-based)
  @param[out] RpDev                     Return corresponding root port device number.
  @param[out] RpFun                     Return corresponding root port function number.

  @retval     EFI_SUCCESS               Root port device and function is retrieved
  @retval     EFI_INVALID_PARAMETER     RpNumber is invalid

**/
EFI_STATUS
EFIAPI
GetScPcieRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFun
  );

/**
  Get Root Port physical Number by SC Pcie Root Port Device and Function Number.

  @param[in]  RpDev                     Root port device number.
  @param[in]  RpFun                     Root port function number.
  @param[out] RpNumber                  Return corresponding Root port physical number.

  @retval     EFI_SUCCESS               Physical root port is retrieved
  @retval     EFI_INVALID_PARAMETER     RpDev and/or RpFun are invalid
  @retval     EFI_UNSUPPORTED           Root port device and function is not assigned to any physical root port

**/
EFI_STATUS
EFIAPI
GetScPcieRpNumber (
  IN  UINTN   RpDev,
  IN  UINTN   RpFun,
  OUT UINTN   *RpNumber
  );

/**
  Read PCR register.
  It returns PCR register and size in 4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrRead32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  OUT UINT32                            *OutData
  );

/**
  Read PCR register.
  It returns PCR register and size in 2bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrRead16 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  OUT UINT16                            *OutData
  );

/**
  Read PCR register.
  It returns PCR register and size in 1bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrRead8 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  OUT UINT8                             *OutData
  );

/**
  Write PCR register.
  It programs PCR register and size in 4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrWrite32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            InData
  );

/**
  Write PCR register.
  It programs PCR register and size in 2bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrWrite16 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT16                            InData
  );

/**
  Write PCR register.
  It programs PCR register and size in 1bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrWrite8 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT8                             InData
  );

/**
  Reads an 4-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 4-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            AndData,
  IN  UINT32                            OrData
  );

/**
  Reads an 2-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 2-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr16 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT16                            AndData,
  IN  UINT16                            OrData
  );

/**
  Reads an 1-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 1-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr8 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT8                             AndData,
  IN  UINT8                             OrData
  );

/**
  Hide P2SB device.

  @param[in]  P2sbBase                  Pci base address of P2SB controller.

  @retval     EFI_SUCCESS               Always return success.

**/
EFI_STATUS
PchHideP2sb (
  IN UINTN                                 P2sbBase
  );

/**
  Reveal P2SB device.
  Also return the original P2SB status which is for Hidding P2SB or not after.
  If OrgStatus is not NULL, then TRUE means P2SB is unhidden,
  and FALSE means P2SB is hidden originally.

  @param[in]  P2sbBase                  Pci base address of P2SB controller.
  @param[out] OrgStatus                 Original P2SB hidding/unhidden status

  @retval     EFI_SUCCESS               Always return success.

**/
EFI_STATUS
PchRevealP2sb (
  IN UINTN                                 P2sbBase,
  OUT BOOLEAN                              *OrgStatus
  );

//
// implemented in PchSbiAccess.c
//

/**
  PCH SBI Register structure

**/
typedef struct {
  UINT32            SbiAddr;
  UINT32            SbiExtAddr;
  UINT32            SbiData;
  UINT16            SbiStat;
  UINT16            SbiRid;
} PCH_SBI_REGISTER_STRUCT;

/**
  PCH SBI opcode definitions

**/
typedef enum {
  MemoryRead             = 0x0,
  MemoryWrite            = 0x1,
  PciConfigRead          = 0x4,
  PciConfigWrite         = 0x5,
  PrivateControlRead     = 0x6,
  PrivateControlWrite    = 0x7,
  GpioLockUnlock         = 0x13
} PCH_SBI_OPCODE;

/**
  PCH SBI response status definitions

**/
typedef enum {
  SBI_SUCCESSFUL          = 0,
  SBI_UNSUCCESSFUL        = 1,
  SBI_POWERDOWN           = 2,
  SBI_MIXED               = 3,
  SBI_INVALID_RESPONSE
} PCH_SBI_RESPONSE;

/**
  Execute PCH SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in]      Pid                        Port ID of the SBI message
  @param[in]      Offset                     Offset of the SBI message
  @param[in]      Opcode                     Opcode
  @param[in]      Posted                     Posted message
  @param[in, out] Data32                     Read/Write data
  @param[out]     Response                   Response

  @retval         EFI_SUCCESS                Successfully completed.
  @retval         EFI_DEVICE_ERROR           Transaction fail
  @retval         EFI_INVALID_PARAMETER      Invalid parameter

**/
EFI_STATUS
EFIAPI
PchSbiExecution (
  IN     SC_SBI_PID                     Pid,
  IN     UINT64                         Offset,
  IN     PCH_SBI_OPCODE                 Opcode,
  IN     BOOLEAN                        Posted,
  IN OUT UINT32                         *Data32,
  OUT    UINT8                          *Response
  );

/**
  Full function for executing PCH SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in]      Pid                        Port ID of the SBI message
  @param[in]      Offset                     Offset of the SBI message
  @param[in]      Opcode                     Opcode
  @param[in]      Posted                     Posted message
  @param[in]      Fbe                        First byte enable
  @param[in]      Bar                        Bar
  @param[in]      Fid                        Function ID
  @param[in, out] Data32                     Read/Write data
  @param[out]     Response                   Response

  @retval         EFI_SUCCESS                Successfully completed.
  @retval         EFI_DEVICE_ERROR           Transaction fail
  @retval         EFI_INVALID_PARAMETER      Invalid parameter

**/
EFI_STATUS
EFIAPI
PchSbiExecutionEx (
  IN     SC_SBI_PID                     Pid,
  IN     UINT64                         Offset,
  IN     PCH_SBI_OPCODE                 Opcode,
  IN     BOOLEAN                        Posted,
  IN     UINT16                         Fbe,
  IN     UINT16                         Bar,
  IN     UINT16                         Fid,
  IN OUT UINT32                         *Data32,
  OUT    UINT8                          *Response
  );

/**
  This function saves all PCH SBI registers.
  The save and restore operations must be done while using the PchSbiExecution inside SMM.
  It prevents the racing condition of PchSbiExecution re-entry between POST and SMI.
  Before using this function, make sure the P2SB is not hidden.

  @param[in, out] PchSbiRegister        Structure for saving the registers

  @retval         EFI_SUCCESS           Successfully completed.
  @retval         EFI_DEVICE_ERROR      Device is hidden.

**/
EFI_STATUS
EFIAPI
PchSbiRegisterSave (
  IN OUT PCH_SBI_REGISTER_STRUCT        *PchSbiRegister
  );

/**
  This function restores all PCH SBI registers
  The save and restore operations must be done while using the PchSbiExecution inside SMM.
  It prevents the racing condition of PchSbiExecution re-entry between POST and SMI.
  Before using this function, make sure the P2SB is not hidden.

  @param[in] PchSbiRegister               Structure for restoring the registers

  @retval    EFI_SUCCESS                  Successfully completed.
  @retval    EFI_DEVICE_ERROR             Device is hidden.

**/
EFI_STATUS
EFIAPI
PchSbiRegisterRestore (
  IN PCH_SBI_REGISTER_STRUCT            *PchSbiRegister
  );

//
// implemented in PchCycleDecoding.c
//
//
// structure of LPC general IO range register
// It contains base address, address mask, and enable status.
//
typedef struct {
  UINT32                                BaseAddr :16;
  UINT32                                Length   :15;
  UINT32                                Enable   : 1;
} PCH_LPC_GEN_IO_RANGE;

#define PCH_LPC_GEN_IO_RANGE_MAX        4

//
// structure of LPC general IO range register list
// It lists all LPC general IO ran registers supported by PCH.
//
typedef struct {
  PCH_LPC_GEN_IO_RANGE                  Range[PCH_LPC_GEN_IO_RANGE_MAX];
} PCH_LPC_GEN_IO_RANGE_LIST;

/**
  Set PCH LPC generic IO range.
  For generic IO range, the base address must align to 4 and less than 0xFFFF, and the length must be power of 2
  and less than or equal to 256. Moreover, the address must be length aligned.
  This function basically checks the address and length, which should not overlap with all other generic ranges.
  If no more generic range register available, it returns out of resource error.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Steps of programming generic IO range:
  1. Program LPC/eSPI PCI Offset 84h ~ 93h of Mask, Address, and Enable.
  2. Program LPC/eSPI Generic IO Range #, PCR[DMI] + 2730h ~ 273Fh to the same value programmed in LPC/eSPI PCI Offset 84h~93h.

  @param[in] Address                       Address for generic IO range base address.
  @param[in] Length                        Length of generic IO range.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid base address or length passed.
  @retval    EFI_OUT_OF_RESOURCES          No more generic range available.

**/
EFI_STATUS
EFIAPI
PchLpcGenIoRangeSet (
  IN UINT16                                Address,
  IN UINTN                                 Length
  );

/**
  Get PCH LPC generic IO range list.
  This function returns a list of base address, length, and enable for all LPC generic IO range regsiters.

  @param[in] LpcGenIoRangeList             Return all LPC generic IO range register status.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid base address passed.

**/
EFI_STATUS
EFIAPI
PchLpcGenIoRangeGet (
  IN PCH_LPC_GEN_IO_RANGE_LIST             *LpcGenIoRangeList
  );

/**
  Set PCH LPC memory range decoding.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. Program LPC/eSPI PCI 98h [0] to [0] to disable memory decoding first before changing base address.
  2. Program LPC/eSPI PCI 98h [31:16, 0] to [Address, 1].
  3. Program LPC/eSPI Memory Range, PCR[DMI] + 2740h to the same value programmed in LPC/eSPI PCI Offset 98h.

  @param[in] Address                       Address for memory base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid base address or length passed.
  @retval    EFI_OUT_OF_RESOURCES          No more generic range available.

**/
EFI_STATUS
EFIAPI
PchLpcMemRangeSet (
  IN UINT32                                Address
  );

/**
  Get PCH LPC memory range decoding address.

  @param[in] Address                       Address of LPC memory decoding base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid base address passed.

**/
EFI_STATUS
EFIAPI
PchLpcMemRangeGet (
  IN UINT32                                *Address
  );

/**
  Set PCH BIOS range deocding.
  This will check General Control and Status bit 10 (GCS.BBS) to identify SPI or LPC/eSPI and program BDE register accordingly.
  Please check EDS for detail of BiosDecodeEnable bit definition.
    bit 15: F8-FF Enable
    bit 14: F0-F8 Enable
    bit 13: E8-EF Enable
    bit 12: E0-E8 Enable
    bit 11: D8-DF Enable
    bit 10: D0-D7 Enable
    bit  9: C8-CF Enable
    bit  8: C0-C7 Enable
    bit  7: Legacy F Segment Enable
    bit  6: Legacy E Segment Enable
    bit  5: Reserved
    bit  4: Reserved
    bit  3: 70-7F Enable
    bit  2: 60-6F Enable
    bit  1: 50-5F Enable
    bit  0: 40-4F Enable
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. if GCS.BBS is 0 (SPI), program SPI PCI offset D8h to BiosDecodeEnable.
     if GCS.BBS is 1 (LPC/eSPi), program LPC/eSPI PCI offset D8h to BiosDecodeEnable.
  2. program LPC/eSPI/SPI BIOS Decode Enable, PCR[DMI] + 2744h to the same value programmed in LPC/eSPI or SPI PCI Offset D8h.

  @param[in] BiosDecodeEnable              Bios decode enable setting.

  @retval    EFI_SUCCESS                   Successfully completed.

**/
EFI_STATUS
EFIAPI
PchBiosDecodeEnableSet (
  IN UINT16                                BiosDecodeEnable
  );

/**
  Set PCH LPC IO decode ranges.
  Program LPC I/O Decode Ranges, PCR[DMI] + 2770h[15:0] to the same value programmed in LPC offset 80h.
  Please check EDS for detail of Lpc IO decode ranges bit definition.
  Bit  12: FDD range
  Bit 9:8: LPT range
  Bit 6:4: ComB range
  Bit 2:0: ComA range

  @param[in] LpcIoDecodeRanges             Lpc IO decode ranges bit settings.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_UNSUPPORTED               DMIC.SRL is set.

**/
EFI_STATUS
EFIAPI
PchLpcIoDecodeRangesSet (
  IN UINT16                                LpcIoDecodeRanges
  );

/**
  Set PCH LPC IO enable decoding.
  Setup LPC I/O Enables, PCR[DMI] + 2774h[15:0] to the same value program in LPC offset 82h.
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in LPC 82h[13:10] is always forwarded by DMI to subtractive agent for handling.
  Please check EDS for detail of Lpc IO decode ranges bit definition.

  @param[in] LpcIoEnableDecoding           Lpc IO enable decoding bit settings.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_UNSUPPORTED               DMIC.SRL is set.

**/
EFI_STATUS
EFIAPI
PchLpcIoEnableDecodingSet (
  IN UINT16                                LpcIoEnableDecoding
  );

/**
  Enable VTd support in PSF.

  @retval    EFI_SUCCESS                   Successfully completed.

**/
EFI_STATUS
PchPsfEnableVtd (
  VOID
  );

/**
  Get Sc Maximum Usb2 Port Number of XHCI Controller.

  @retval    UINT8                         Sc Maximum Usb2 Port Number of XHCI Controller

**/
UINT8
EFIAPI
GetScXhciMaxUsb2PortNum (
  VOID
  );

/**
  Get Sc Maximum Usb3 Port Number of XHCI Controller.

  @retval    UINT8                         Sc Maximum Usb3 Port Number of XHCI Controller

**/
UINT8
EFIAPI
GetScXhciMaxUsb3PortNum (
  VOID
  );

/**
  Get SPI linear Base address of descriptor region section.

  @param[in] RegNum                        FLREG number of region section defined in the descriptor

  @retval    UINT32                        Base address of the FLREG

**/
UINT32
GetSpiFlashRegionBase (
  IN UINTN        RegNum
  );

/**
  return SPI linear Base address of descriptor region section.

  @param[in] RegNum                       FLREG number of region section defined in the descriptor

  @retval    UINTN                        Base address of the FLREG

**/
UINT32
GetSpiFlashRegionLimit (
  UINTN        RegNum
  );

typedef enum  {
  PcieP1,
  PcieP2,
  PcieP3,
  PcieP4,
  PcieP5,
  PcieP6,
  SataP0,
  SataP1,
  UsbP0,
  UsbP1,
  UsbP2,
  UsbP3,
  UsbP4,
  UsbP5,
  UsbP6,
  UsbP7
} SC_DEVICE_PORT;


/**
  Set TCO base address for legacy Smbus.

  @param[in] Address                       Address for TCO base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid base address passed.
  @retval    EFI_UNSUPPORTED               DMIC.SRL is set.

**/
EFI_STATUS
EFIAPI
SetTcoBase (
  IN UINT16                             Address
  );

/**
  Get TCO base address.

  @param[in] Address                       Address of TCO base address.

  @retval    EFI_SUCCESS                   Successfully completed.
  @retval    EFI_INVALID_PARAMETER         Invalid pointer passed.

**/
EFI_STATUS
EFIAPI
GetTcoBase (
  IN UINT16                            *Address
  );
#endif

