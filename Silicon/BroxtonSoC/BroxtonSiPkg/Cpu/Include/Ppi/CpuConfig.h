/** @file
  CPU policy PPI produced by a platform driver specifying various
  expected CPU settings. This PPI is consumed by CPU PEI modules.

  Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gCpuConfigGuid;

#pragma pack(push, 1)

//
// CPU_CONFIG revisions
//
#define CPU_CONFIG_REVISION 1

///
/// Enums for EC Command Type
///
typedef enum {
  SendEcCmds = 1,
  SendEcValue,
  ReceiveEcValue,
  EcCmdMax
} EC_COMMAND_TYPE;

/**
  This function is for platform code to provide EC Commands since different BIOS might have different EC.
  Platform code need to provide a function for CPU code to call to communicate with EC.

  @param[in] This                 Driver context.
  @param[in] EcCmdType            EC Command Type.
  @param[in] EcCmd                EC Command Byte to send.
  @param[in] SendData             EC Data Byte to send.
  @param[in] ReceiveData          EC Data Byte received.

  @retval    EFI_SUCCESS          Command Read/ Write Success
  @retval    EFI_DEVICE_ERROR     Command Read/ Write Error
  @retval    EFI_OUT_OF_RESOURCES No enough resources (such as out of memory).

**/
typedef
EFI_STATUS
(EFIAPI *PLATFORM_SEND_EC_COMMAND) (
  IN EC_COMMAND_TYPE   EcCmdType,
  IN UINT8             EcCmd,
  IN UINT8             SendData,
  IN OUT UINT8         *ReceiveData
  );

///
/// Platform Specific Socket Information
///
typedef struct {
  EFI_PHYSICAL_ADDRESS SocketDesignation;
  EFI_PHYSICAL_ADDRESS SerialNumber;
  EFI_PHYSICAL_ADDRESS AssetTag;
  EFI_PHYSICAL_ADDRESS PartNumber;
  UINT16               MaxSpeed;
  UINT8                ProcessorUpgrade;
  UINT8                Rsvd;
} SMBIOS_SOCKET_INFO;

/**
  Platform Policies for CPU features configuration Platform code can enable/disable/configure features through this structure.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>.
  - <b>(Required)</b>: This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization.
  - <b>(Optional)</b>: This policy is recommended for validation purpose only.
**/
typedef struct {
  CONFIG_BLOCK_HEADER Header;                ///< Offset 0 GUID number for main entry of config block
  //
  // Bit definition for functionality enable/disable.
  //
  UINT32 HyperThreading                  : 1; ///< <b>(Optional)</b> Enable or Disable Hyper Threading; 0: Disable; <b>1: Enable</b>.
  UINT32 VmxEnable                       : 1; ///< <b>(Optional)</b> Enable or Disable VMX; 0: Disable; <b>1: Enable</b>.
  UINT32 MlcStreamerPrefetcher           : 1; ///< <b>(Optional)</b> Enable or Disable MLC Streamer Prefetcher; 0: Disable; <b>1: Enable</b>.
  UINT32 MlcSpatialPrefetcher            : 1; ///< <b>(Optional)</b> Enable or Disable MLC Spatial Prefetcher; 0: Disable; <b>1: Enable</b>.
  UINT32 SmxEnable                       : 1; ///< <b>(Optional)</b> Enable or Disable Secure Mode Extensions feature; 0: Disable; <b>1: Enable</b>.
  UINT32 MonitorMwaitEnable              : 1; ///< <b>(Optional)</b> Enable or Disable Monitor /MWAIT instructions; 0: Disable; <b>1: Enable</b>.
  UINT32 MachineCheckEnable              : 1; ///< <b>(Optional)</b> Enable or Disable initialization of machine check registers; 0: Disable; <b>1: Enable</b>.
  UINT32 AesEnable                       : 1; ///< <b>(Optional)</b> Enable or Disable Advanced Encryption Standard (AES) feature; 0: Disable; <b>1: Enable</b>.
  UINT32 DebugInterfaceEnable            : 1; ///< <b>(Optional)</b> Enable or Disable processor debug features; <b>0: Disable</b>; 1: Enable.
  UINT32 DebugInterfaceLockEnable        : 1; ///< <b>(Optional)</b> Lock or Unlock debug interface features; 0: Disable; <b>1: Enable</b>.
  /**
  <b>(Required)</b> Policies to obtain CPU temperature.
   - <b>0: ACPI thermal management uses EC reported temperature values</b>.
   - 1: ACPI thermal management uses DTS SMM mechanism to obtain CPU temperature values.
   - 2: ACPI Thermal Management uses EC reported temperature values and DTS SMM is used to handle Out of Spec condition.
  **/
  UINT32 EnableDts                       : 2;
  UINT32 ApIdleManner                    : 2; ///< <b>(Optional)</b> AP Idle Manner of waiting for SIPI; <b>1: HALT loop</b>; 2: MWAIT loop; 3: RUN loop.
  UINT32 ApHandoffManner                 : 2; ///< <b>(Debug)</b> Settings for AP Handoff to OS; <b>1: HALT loop</b>; 2: MWAIT loop.
  UINT32 ActiveProcessorCores            : 3; ///< <b>(Debug)</b> Number of active cores. <b>0: All</b>.
  UINT32 DisableCore1                    : 1; ///< <b>(Debug)</b> Enable/Disable Core 1 <b>
  UINT32 DisableCore2                    : 1; ///< <b>(Debug)</b> Enable/Disable Core 2 <b>
  UINT32 DisableCore3                    : 1; ///< <b>(Debug)</b> Enable/Disable Core 3 <b>
  /**
  <b>(Debug)</b> Processor Early Power On Configuration FCLK setting.
     - <b>0: 800 MHz (ULT/ULX)</b>.
     - <b>1: 1 GHz (DT/Halo)</b>
        - 2: 400 MHz.
        - 3: Reserved.
  **/
  UINT32 FClkFrequency                   : 2;
  UINT32 JtagC10PowerGateDisable         : 1; ///< <b>(Debug)</b> Power JTAG in C10 and deeper power states; <b>0: Disable</b>; 1: Enable.
  UINT32 ProcTraceOutputScheme           : 1; ///< <b>(Debug)</b> Control on Processor Trace output scheme; <b>0: Single Range Output</b>; 1: ToPA Output.
  UINT32 ProcTraceEnable                 : 1; ///< <b>(Debug)</b> Enable or Disable Processor Trace feature; <b>0: Disable</b>; 1: Enable.
  UINT32 ThreeStrikeCounterDisable       : 1; ///< <b>(Debug)</b> Disable Three strike counter; <b>0: Enable counter</b>; 1: Disable counter.
  UINT32 VoltageOptimizer                : 1; ///< <b>(Optional)</b> Enable or Disable Voltage Optimizer feature <b>0: Disable</b>; 1: Enable
  UINT32 FlashWearOutProtection          : 1; ///< Flash Wear-out Protection; <b>0: Disable</b>; 1: Enable
  UINT32 SkipMpInit                      : 2; /// < For Fsp only, Silicon Initialization will skip MP Initialization (including BSP) if enabled. For non-FSP, this should always be 0.
  UINT32 PackageDts                      : 1; ///< <b>(Debug)</b> Power JTAG in C10 and deeper power states; <b>0: Disable</b>; 1: Enable.
  UINT32 RsvdBits                        :31; ///< Bits reserved for DWORD alignment.
  UINT8  CpuRatio;                            ///< <b>(Debug)</b> CPU ratio value.
  UINT8  SmmbaseSwSmiNumber;                  ///< <b>(Required)</b> Software SMI Number from Smbase.
  UINT8  ProcTraceMemSize;                    ///< <b>(Optional)</b> Memory region allocation for Processor Trace, allowed range is from 4K (0x0) to 128MB (0xF); <b>0xFF: Disable</b>.
  UINT8  Rsvd;                                ///< Reserved for DWORD alignment.
  EFI_PHYSICAL_ADDRESS MicrocodePatchAddress; ///< <b>(Required)</b> Platform code provides the address of the correct microcode patch.
  PLATFORM_SEND_EC_COMMAND SendEcCmd;         ///< Platform code can provide interface to communicate with EC through this function.
  EFI_PHYSICAL_ADDRESS SmbiosSocketInfo;      ///< This points to SMBIOS_SOCKET_INFO structure which is used to fill in the socket-related info for SMBIOS table type 4.
} CPU_CONFIG;

#pragma pack(pop)
#endif

