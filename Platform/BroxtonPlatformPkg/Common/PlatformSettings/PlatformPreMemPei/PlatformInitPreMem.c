/** @file
  Source code file for Platform Init Pre-Memory PEI module.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <ScAccess.h>
#include <SaAccess.h>
#include <SeCAccess.h>
#include <PlatformBaseAddresses.h>
#include <FrameworkPei.h>
#include <Guid/VariableFormat.h>
#include <Ppi/MfgMemoryTest.h>
#include <Ppi/TemporaryRamSupport.h>
#include <Ppi/BlockIo.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/SeCUma.h>
#include <Ppi/FvLoadFile.h>
#include <Ppi/Stall.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/SiPolicyPpi.h>
#include <Ppi/BiosReservedMemory.h>
#include <Ppi/DramPolicyPpi.h>
#include <Ppi/BoardInitSignalling.h>
#include <Guid/Capsule.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Guid/SystemNvDataGuid.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <Guid/AcpiVariableCompatibility.h>
#include <Guid/FirmwarePerformance.h>
#include <Guid/VariableFormat.h>
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiVariableCacheLib.h>
#include <Library/PeiPlatformConfigUpdateLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PerformanceLib.h>
#include <Library/MtrrLib.h>
#include <Library/I2CLib.h>
#include <Library/PmicLib.h>
#include <Library/PeiVariableCacheLib.h>
#include <Library/PeiSiPolicyLib.h>
#include <Library/PeiPlatformConfigUpdateLib.h>
#include <Library/CpuPolicyLib.h>
#include <Library/ScPlatformLib.h>
#include <Library/PeiPolicyInitLib.h>
#include <Library/PeiScPolicyLib.h>
#include <Library/PeiSiPolicyUpdateLib.h>
#include "Smip.h"
#include "Stall.h"
#include "FvCallback.h"
#include "MemoryCallback.h"
#include "BoardGpiosPreMem.h"
#include "PlatformInitPreMem.h"
#include <Library/SteppingLib.h>
#include <Library/HeciMsgLib.h>
#include <Ppi/SecPlatformInformation.h>
#include <Library/PlatformSecLib.h>
#include <Library/TimerLib.h>

#if (ENBDT_PF_ENABLE == 1)
//
//SSC
//
  #include <Library/PmcIpcLib.h>
  #include <SscRegs.h>
  #include <Library/SideBandLib.h>
#endif

extern EFI_GUID gEfiBootMediaHobGuid;

//
// The global indicator, the FvFileLoader callback will modify it to TRUE after loading PEIM into memory
//
BOOLEAN ImageInMemory = FALSE;
CHAR8   mGdtTable[0x40];

//
//Memory Test Manufacturing mode
//
#define DATA_PATTERN_ARRAY_SIZE (sizeof(DataPatternForMemoryTest) / sizeof(UINT32))
UINT32 DataPatternForMemoryTest[] = {
  0x55555555, 0xAAAAAAAA, 0x55555510, 0x555555EF, 0x55555510, 0x555555EF, 0x55555510, 0x555555EF,
  0x55555555, 0xAAAAAAAA, 0x55551055, 0x5555EF55, 0x55551055, 0x5555EF55, 0x55551055, 0x5555EF55,
  0x55555555, 0xAAAAAAAA, 0x55105555, 0x55EF5555, 0x55105555, 0x55EF5555, 0x55105555, 0x55EF5555,
  0x55555555, 0xAAAAAAAA, 0x10555555, 0xEF555555, 0x10555555, 0xEF555555, 0x10555555, 0xEF555555
};

extern EFI_PEI_PPI_DESCRIPTOR mCseUfsSelectPpiList[];
extern EFI_PEI_PPI_DESCRIPTOR mCseEmmcSelectPpiList[];
extern EFI_PEI_PPI_DESCRIPTOR mCseSpiSelectPpiList[];

#define PEI_STALL_RESOLUTION   1
static EFI_PEI_STALL_PPI  mStallPpi = {
  PEI_STALL_RESOLUTION,
  Stall
};


#if defined(PRAM_SUPPORT)
static PEI_BIOS_RESERVED_MEMORY_POLICY_PPI mPeiBiosReservedMemoryPolicyPpi = {
  GetBiosReservedMemoryPolicy
};

static EFI_PEI_PPI_DESCRIPTOR mBiosReservedMemoryPolicyPpi =
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gBiosReservedMemoryPolicyPpiGuid,
    &mPeiBiosReservedMemoryPolicyPpi
  };
#endif


static EFI_PEI_PPI_DESCRIPTOR mInstallStallPpi =
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiStallPpiGuid,
    &mStallPpi
  };


static PEI_MFG_MEMORY_TEST_PPI mPeiMfgMemoryTestPpi = {
  MfgMemoryTest
};


static EFI_PEI_PPI_DESCRIPTOR mMfgMemTestPpi =
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gPeiMfgMemoryTestPpiGuid,
    &mPeiMfgMemoryTestPpi
  };

static EFI_PEI_PPI_DESCRIPTOR mPeiTemporaryRamSupportPpiPpi[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiTemporaryRamSupportPpiGuid,
    NULL
  }
};


//
// Notify Callbacks for SPI and non-SPI boot devices.
// These are installed by MemoryDiscovered Callback, since they require main memory.
//
EFI_PEI_NOTIFY_DESCRIPTOR mFvNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiVirtualBlockIoPpiGuid,  //non-SPI boot - installed after MemInit
    GetFvNotifyCallback
  },

  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gCseSpiSelectPpiGuid,   //SPI boot - installed by PeiSecUma
    GetFvNotifyCallback
  }
};


EFI_STATUS
EFIAPI
FspTempRamExitCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "FspTempRamExitCallback\n"));
  Status = PeiServicesNotifyPpi (&mFvNotifyList[0]);
  return Status;
}


EFI_PEI_NOTIFY_DESCRIPTOR mFspTempRamExitList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gFspTempRamExitGuid,
    FspTempRamExitCallback
  }
};


EFI_STATUS
EFIAPI
GetBiosReservedMemoryPolicy (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN PEI_BIOS_RESERVED_MEMORY_POLICY_PPI  *This,
  IN OUT BIOS_RESERVED_MEMORY_CONFIG      *BiosReservedMemoryPolicy
  );


VOID
CopyMemSse4 (
  IN VOID* Dst,
  IN VOID* Src,
  IN UINTN SizeInBytes
  )
{
  _asm {
    //
    // Initialize pointers to start of the USWC memory
    //
    mov esi, Src
    mov edx, Src

    //
    // Initialize pointer to end of the USWC memory
    //
    add edx, SizeInBytes

    //
    // Initialize pointer to start of the cacheable WB buffer
    //
    mov edi, Dst

    //
    // save xmm0 ~ xmm3 to stack
    //
    sub     esp, 040h
    movdqu  [esp], xmm0
    movdqu  [esp + 16], xmm1
    movdqu  [esp + 32], xmm2
    movdqu  [esp + 48], xmm3

    //
    // Start of Bulk Load loop
    //
    inner_start:
    //
    // Load data from USWC Memory using Streaming Load
    //
    MOVNTDQA xmm0, xmmword ptr [esi]
    MOVNTDQA xmm1, xmmword ptr [esi + 16]
    MOVNTDQA xmm2, xmmword ptr [esi + 32]
    MOVNTDQA xmm3, xmmword ptr [esi + 48]

    //
    // Copy data to buffer
    //
    MOVDQA xmmword ptr [edi], xmm0
    MOVDQA xmmword ptr [edi + 16], xmm1
    MOVDQA xmmword ptr [edi + 32], xmm2
    MOVDQA xmmword ptr [edi + 48], xmm3

    //
    // Increment pointers by cache line size and test for end of loop
    //
    add esi, 040h
    add edi, 040h
    cmp esi, edx
    jne inner_start

    //
    // restore xmm0 ~ xmm3
    //
    mfence
    movdqu  xmm0, [esp]
    movdqu  xmm1, [esp + 16]
    movdqu  xmm2, [esp + 32]
    movdqu  xmm3, [esp + 48]
    add     esp, 040h // stack cleanup
  }
  // End of Bulk Load loop
}


#if defined(PRAM_SUPPORT)

/**
  This function is to get Bios Reserved Memory in PEI.

  @param[in]       PeiServices                 Pointer to PEI Services.
  @param[in]       This                        Pei memory test PPI pointer.
  @param[in, out]  BiosReservedMemoryPolicy    Pointer to BiosReservedMemorypolicy.

  @retval          EFI_SUCCESS                 The operation completed successfully.

**/
EFI_STATUS
EFIAPI
GetBiosReservedMemoryPolicy (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN PEI_BIOS_RESERVED_MEMORY_POLICY_PPI  *This,
  IN OUT BIOS_RESERVED_MEMORY_CONFIG      *BiosReservedMemoryPolicy
  )
{
  EFI_STATUS                         Status;
  UINTN                              VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *VariableServices;
  SYSTEM_CONFIGURATION               SystemConfiguration;

  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );


  if (Status == EFI_SUCCESS) {
#ifdef PRAM_SUPPORT
    BiosReservedMemoryPolicy->Pram = SystemConfiguration.Pram;
#endif
  } else {
#ifdef PRAM_SUPPORT
    BiosReservedMemoryPolicy->Pram = 0x30;
#endif

  }
  Status = EFI_SUCCESS;

#ifdef PRAM_SUPPORT
  DEBUG ((DEBUG_INFO, "SystemConfiguration.Pram = %x \n", SystemConfiguration.Pram));
#endif
  return Status;
}

#endif


/**
  This function checks the memory range in PEI.

  @param[in]  PeiServices         Pointer to PEI Services.
  @param[in]  This                Pei memory test PPI pointer.
  @param[in]  BeginAddress        Beginning of the memory address to be checked.
  @param[in]  MemoryLength        Bytes of memory range to be checked.

  @retval     EFI_SUCCESS         The operation completed successfully.
  @retval     EFI_DEVICE_ERROR    Memory test failed. It's not safe to use this range of memory.

**/
EFI_STATUS
EFIAPI
MfgMemoryTest (
  IN  CONST EFI_PEI_SERVICES             **PeiServices,
  IN  PEI_MFG_MEMORY_TEST_PPI            *This,
  IN  UINT32                             BeginAddress,
  IN  UINT32                             MemoryLength
  )
{
  UINT32  i;
  UINT32  memAddr;
  UINT32  readData;
  UINT32  xorData;
  UINT32  TestFlag = 0;

  memAddr = BeginAddress;

  //
  // Output Message for MFG
  //
  DEBUG ((DEBUG_INFO, "MFGMODE SET\n"));

  //
  //  Writing the pattern in defined location.
  //
  while (memAddr < (BeginAddress+MemoryLength)) {
    for (i = 0; i < DATA_PATTERN_ARRAY_SIZE; i++) {
      if (memAddr > (BeginAddress + MemoryLength - 4)) {
        memAddr = memAddr + 4;
        break;
      }

      *((volatile UINT32*) memAddr) = DataPatternForMemoryTest[i];
      memAddr = memAddr + 4;
    }
  }

  //
  // Verify the pattern
  //
  memAddr = BeginAddress;

  while (memAddr < (BeginAddress + MemoryLength)) {
    for (i = 0; i < DATA_PATTERN_ARRAY_SIZE; i++) {
      if (memAddr > (BeginAddress + MemoryLength - 4)) {
        memAddr = memAddr + 4;
        break;
      }

      readData = *((volatile UINT32*) memAddr);
      xorData = readData ^ DataPatternForMemoryTest[i];

      //
      // If xorData is non-zero, this particular memAddr has a failure.
      //
      if (xorData != 0x00000000) {
        DEBUG ((DEBUG_ERROR, "Expected value....: %x\n", DataPatternForMemoryTest[i]));
        DEBUG ((DEBUG_ERROR, "ReadData value....: %x\n", readData));
        DEBUG ((DEBUG_ERROR, "Pattern failure at....: %x\n", memAddr));
        TestFlag = 1;
      }

      memAddr = memAddr + 4;
    }
  }

  if (TestFlag) {
    return EFI_DEVICE_ERROR;
  }

  //
  //Output Message for MFG
  //
  DEBUG ((DEBUG_INFO, "MFGMODE MEMORY TEST PASSED\n"));

  return EFI_SUCCESS;
}


BOOLEAN
IsRtcUipAlwaysSet (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_PEI_STALL_PPI  *StallPpi;
  UINTN              Count;

  PeiServicesLocatePpi (&gEfiPeiStallPpiGuid, 0, NULL, (VOID **) &StallPpi);

  for (Count = 0; Count < 500; Count++) { // Maximum waiting approximates to 1.5 seconds (= 3 msec * 500)
    IoWrite8 (R_RTC_INDEX2, R_RTC_REGISTERA);
    if ((IoRead8 (R_RTC_TARGET2) & B_RTC_REGISTERA_UIP) == 0) {
      return FALSE;
    }

    StallPpi->Stall (PeiServices, StallPpi, 3000);
  }

  return TRUE;
}


VOID
RtcPowerFailureHandler (
  VOID
  )
{
  UINT16          Data16;
  UINT8           Data8;

  // When the RTC_PWR_STS bit is set, it indicates that the RTCRST# signal went low.
  // Software should clear this bit. For example, changing the RTC battery sets this bit.
  // System BIOS should reset CMOS to default values if the RTC_PWR_STS bit is set.
  // The System BIOS should execute the sequence below if the RTC_PWR_STS bit is set
  // before memory initialization. This will ensure that the RTC state machine has been
  // initialized.
  // 1. If the RTC_PWR_STS bit is set which indicates a new coin-cell battery insertion or a
  //    battery failure, steps 2 through 5 should be executed.
  // 2. Set RTC Register 0x0A[6:4] to 110b or 111b.
  // 3. Set RTC Register 0x0B[7].
  // 4. Set RTC Register 0x0A[6:4] to 010b.
  // 5. Clear RTC Register 0x0B[7].
  //

  Data16 = MmioRead16 (PMC_BASE_ADDRESS + R_PMC_GEN_PMCON_1);

  if ((Data16 & B_PMC_GEN_PMCON_RTC_PWR_STS) != 0) {
    //
    // 2. Set RTC Register 0Ah[6:4] to 110b or 111b
    //
    IoWrite8 (R_RTC_INDEX2, (UINT8) R_RTC_REGISTERA);
    Data8 = IoRead8 (R_RTC_TARGET2) & (UINT8) ~(B_RTC_REGISTERA_DV);
    Data8 |= (UINT8) (V_RTC_REGISTERA_DV_DIV_RST1);
    IoWrite8 (R_RTC_TARGET2, Data8);

    //
    // 3. Set RTC Register 0Bh[7].
    //
    IoWrite8 (R_RTC_INDEX2, (UINT8) R_RTC_REGISTERB);
    IoOr8 (R_RTC_TARGET2, (UINT8) B_RTC_REGISTERB_SET);

    //
    // 4. Set RTC Register 0Ah[6:4] to 010b
    //
    IoWrite8 (R_RTC_INDEX2, (UINT8) R_RTC_REGISTERA);
    Data8 = IoRead8 (R_RTC_TARGET2) & (UINT8) ~(B_RTC_REGISTERA_DV);
    Data8 |= (UINT8) (V_RTC_REGISTERA_DV_NORM_OP);
    IoWrite8 (R_RTC_TARGET2, Data8);

    //
    // 5. Clear RTC Register 0Bh[7].
    //
    IoWrite8 (R_RTC_INDEX2, (UINT8) R_RTC_REGISTERB);
    IoAnd8 (R_RTC_TARGET2, (UINT8) ~B_RTC_REGISTERB_SET);
  }

  return;
}


VOID
ScBaseInit (
  VOID
  )
{
  //
  // Set BARs for PMC SSRAM (0/13/3)
  // Allocation for these regions is done in PlatformInitFinalConfig() via call to BuildResourceDescriptorHob()
  //
  MmioWrite32 (
    MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PMC, PCI_FUNCTION_NUMBER_PMC_SSRAM, 0x10),  //Write BAR0-lower
    PcdGet32 (PcdPmcSsramBaseAddress0)
    );
  MmioWrite32 (
    MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PMC, PCI_FUNCTION_NUMBER_PMC_SSRAM, 0x18),  //Write BAR1-lower
    PcdGet32 (PcdPmcSsramBaseAddress1)
    );
  MmioWrite16 (
    MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PMC, PCI_FUNCTION_NUMBER_PMC_SSRAM, 0x4),   //Set BME and MSE
    0x6
    );

  //
  // Set SPI Base Address
  //
  MmioWrite32 (
    MmPciAddress (0,DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SPI, PCI_FUNCTION_NUMBER_SPI, R_SPI_BASE),
    (UINT32) ((SPI_BASE_ADDRESS & B_SPI_BASE_BAR))
    );

  //
  // Enable SPI Memory decode
  //
  MmioWrite16 (
    MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_SPI, PCI_FUNCTION_NUMBER_SPI, R_SPI_COMMAND),
    EFI_PCI_COMMAND_MEMORY_SPACE
    );

  //
  // Set P2SB Base Address
  //
  MmioWrite32 (
    MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_P2SB, PCI_FUNCTION_NUMBER_P2SB, R_P2SB_BASE),
    (UINT32) ((PcdGet32 (PcdP2SBBaseAddress)))
    );

  //
  // Enable P2SB Memory decode
  //
  MmioWrite16 (
    MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_P2SB, PCI_FUNCTION_NUMBER_P2SB, R_P2SB_STSCMD),
    B_P2SB_STSCMD_BME | B_P2SB_STSCMD_MSE
    );

  PchLpcIoDecodeRangesSet (
    (V_PCH_LPC_IOD_LPT_378  << N_PCH_LPC_IOD_LPT)  |
    (V_PCH_LPC_IOD_COMB_3E8 << N_PCH_LPC_IOD_COMB) |
    (V_PCH_LPC_IOD_COMA_3F8 << N_PCH_LPC_IOD_COMA)
    );

  PchLpcIoEnableDecodingSet (
    B_PCH_LPC_IOE_ME2  |
    B_PCH_LPC_IOE_SE   |
    B_PCH_LPC_IOE_ME1  |
    B_PCH_LPC_IOE_KE   |
    B_PCH_LPC_IOE_HGE  |
    B_PCH_LPC_IOE_LGE  |
    B_PCH_LPC_IOE_FDE  |
    B_PCH_LPC_IOE_PPE  |
    B_PCH_LPC_IOE_CBE  |
    B_PCH_LPC_IOE_CAE
    );

}



/**
  This function performs SC PreMem Policy initialization.

  @param[in]  StartTimerTicker       The Start Timer Ticker for PFET# enabled

  @retval     EFI_SUCCESS            The PPI is installed and initialized.
  @retval     EFI ERRORS             The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES   Do not have enough resources to initialize the driver

**/
EFI_STATUS
EFIAPI
PeiScPreMemPolicyInit (
  IN UINT64              *StartTimerTicker
  )
{
  EFI_STATUS             Status;
  SC_PREMEM_POLICY_PPI   *ScPreMemPolicy;
  SC_PCIE_PREMEM_CONFIG  *PciePreMemConfig;

  //
  // Call ScCreatePreMemConfigBlocks to initialize SC Policy structure
  // and get all Intel default policy settings.
  //
  Status = ScCreatePreMemConfigBlocks (&ScPreMemPolicy);
  ASSERT_EFI_ERROR (Status);

  //
  // Update and override all platform related and customized settings below.
  //
  Status = GetConfigBlock ((VOID *) ScPreMemPolicy, &gPcieRpPreMemConfigGuid, (VOID *) &PciePreMemConfig);
  ASSERT_EFI_ERROR (Status);

  //
  // Update PCIe PERST# and CLK# policies
  //
  PciePreMemConfig->StartTimerTickerOfPfetAssert = (UINTN) *StartTimerTicker;
  PciePreMemConfig->RootPort[0].Perst = N_GPIO_13;  // Slot2
  PciePreMemConfig->RootPort[1].Perst = N_GPIO_15;  // NGFF
  PciePreMemConfig->RootPort[2].Perst = W_GPIO_152; // Slot1
  PciePreMemConfig->RootPort[3].Perst = 0;
  PciePreMemConfig->RootPort[4].Perst = N_GPIO_37;  // LOM
  PciePreMemConfig->RootPort[5].Perst = 0;
  PciePreMemConfig->RootPort[0].Clock = W_GPIO_211;  // Slot2
  PciePreMemConfig->RootPort[1].Clock = W_GPIO_212;  // NGFF
  PciePreMemConfig->RootPort[2].Clock = W_GPIO_209; // Slot1
  PciePreMemConfig->RootPort[3].Clock = 0;
  PciePreMemConfig->RootPort[4].Clock = 0;
  PciePreMemConfig->RootPort[5].Clock = 0;

  //
  // Install ScPreMemPolicyPpi.
  // While installed, RC assumes the Policy is ready and finalized. So please
  // update and override any setting before calling this function.
  //
  Status = ScInstallPreMemPolicyPpi (ScPreMemPolicy);
  ASSERT_EFI_ERROR (Status);

  return Status;
}



#if (ENBDT_PF_ENABLE == 1)
//
// DDR SSC
//
EFI_STATUS
EFIAPI
PeiDDRSSCInit (
  VOID
  )
{
  EFI_STATUS                       Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  SSC_IPC_BUFFER                   WBuf;
  UINT32                           BufferSize = 0;
  SYSTEM_CONFIGURATION             SystemConfiguration;
  UINTN                            VariableSize;

  //
  // static table for the SSC settings (corresponding with the SSC settings 0~-0.5%, 0.1% stepping)
  // Modulation Freq = 32KHz
  //
  SSC_SETTING                     SSC_Select_Table[] = {{No_SSC, 0x12B, 0},
                                                        {M01_SSC, 0x12B, 0x1062},
                                                        {M02_SSC, 0x12B, 0x2BB0},
                                                        {M03_SSC, 0x12B, 0x46FF},
                                                        {M04_SSC, 0x12B, 0x624D},
                                                        {M05_SSC, 0x12B, 0x7D9C}};

  //
  //static table for the clock bending settings (corresponding with the clock bending settings 1.3%, 0.6%, 0, -0.9%)
  //
  CLOCK_BENDING_SETTING           CLK_Bending_Table[] = {{Clk_Bending_13, 0xA00000, 0x7E},
                                                         {Clk_Bending_06, 0xC00000, 0x7D},
                                                         {No_Clk_Bending, 0x0, 0x7D},
                                                         {Clk_Bending_M09, 0xDB6C20, 0x7B}};

  //
  // default value of the 4 SSC setting registers
  //
  WBuf.LJ1PLL_CTRL_1.Data = 0x00;
  WBuf.LJ1PLL_CTRL_2.Data = 0x0888812B;
  WBuf.LJ1PLL_CTRL_3 = 0x7D000000;
  WBuf.LJ1PLL_CTRL_5.Data = 0x7D000000;
  BufferSize = sizeof (UINT32) * 4;

  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  VariableSize = sizeof (SYSTEM_CONFIGURATION);

  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Setup Variable is not ready for SSC setting! Used default value!!\n\r"));
    //
    // Set default value of SSC
    //
    WBuf.LJ1PLL_CTRL_2.Fields.ssc_cyc_to_peak_m1 = SSC_Select_Table[SSC_DEFAULT_SETTING].Ssc_Cyc_To_Peak;
    WBuf.LJ1PLL_CTRL_2.Fields.ssc_frac_step = SSC_Select_Table[SSC_DEFAULT_SETTING].Ffs_Frac_Step;
    //
    // Set default value of Clock bending
    //
    WBuf.LJ1PLL_CTRL_5.Fields.pll_ratio_frac = CLK_Bending_Table[CLK_BENDING_DEFAULT_SETTING].Pll_Ratio_Frac;
    WBuf.LJ1PLL_CTRL_5.Fields.pll_ratio_int = CLK_Bending_Table[CLK_BENDING_DEFAULT_SETTING].Pll_Ratio_Int;

    //
    // send the IPC command for SSC
    //
    Status = IpcSendCommandEx (IPC_CMD_ID_EMI_RFI_SUPPORT, IPC_SUBCMD_ID_SSC_APPLY_NOW, &WBuf, BufferSize);

    //
    // Delay for 1ms to avoid the SSC doesn't set correctly sometimes
    //
    MicroSecondDelay (1000);

    //
    // set the ssc_en to Disable!
    //
    WBuf.LJ1PLL_CTRL_1.Fields.ssc_en = SSC_DISABLE;
    WBuf.LJ1PLL_CTRL_1.Fields.ssc_en_ovr = SSC_DISABLE;
    Status = IpcSendCommandEx (IPC_CMD_ID_EMI_RFI_SUPPORT, IPC_SUBCMD_ID_SSC_APPLY_NOW, &WBuf, BufferSize);
    return Status;
  }

  if (SystemConfiguration.DDRSSCEnable) {
    //
    // get the correct register values of the SSC setting
    //
    WBuf.LJ1PLL_CTRL_2.Fields.ssc_cyc_to_peak_m1 = SSC_Select_Table[SystemConfiguration.DDRSSCSelection].Ssc_Cyc_To_Peak;
    WBuf.LJ1PLL_CTRL_2.Fields.ssc_frac_step = SSC_Select_Table[SystemConfiguration.DDRSSCSelection].Ffs_Frac_Step;
    //
    // get the correct register values of the clock bending setting

    WBuf.LJ1PLL_CTRL_5.Fields.pll_ratio_frac = CLK_Bending_Table[SystemConfiguration.DDRCLKBending].Pll_Ratio_Frac;
    WBuf.LJ1PLL_CTRL_5.Fields.pll_ratio_int = CLK_Bending_Table[SystemConfiguration.DDRCLKBending].Pll_Ratio_Int;

    //
    // send the IPC command for SSC settings
    //
    Status = IpcSendCommandEx (IPC_CMD_ID_EMI_RFI_SUPPORT, IPC_SUBCMD_ID_SSC_APPLY_NOW, &WBuf, BufferSize);

    //
    // Delay for 1ms to avoid the SSC doesn't set correctly sometimes
    //
    MicroSecondDelay (1000);

    //
    // set the ssc_en and ssc_en_ovr to Enable!
    //
    WBuf.LJ1PLL_CTRL_1.Fields.ssc_en = SSC_ENABLE;
    WBuf.LJ1PLL_CTRL_1.Fields.ssc_en_ovr = SSC_ENABLE;

    //
    // send the IPC command for SSC EN
    //
    Status = IpcSendCommandEx (IPC_CMD_ID_EMI_RFI_SUPPORT, IPC_SUBCMD_ID_SSC_APPLY_NOW, &WBuf, BufferSize);
  } else {
    // get the correct register values of the clock bending setting
    WBuf.LJ1PLL_CTRL_5.Fields.pll_ratio_frac = CLK_Bending_Table[SystemConfiguration.DDRCLKBending].Pll_Ratio_Frac;
    WBuf.LJ1PLL_CTRL_5.Fields.pll_ratio_int = CLK_Bending_Table[SystemConfiguration.DDRCLKBending].Pll_Ratio_Int;

    Status = IpcSendCommandEx (IPC_CMD_ID_EMI_RFI_SUPPORT, IPC_SUBCMD_ID_SSC_APPLY_NOW, &WBuf, BufferSize);
    return Status;
  }

  return Status;
}

//
// USB3, PCie, SATA, eDP, DP, eMMC, SD and SDIO SSC
//
EFI_STATUS
EFIAPI
PeiHighSpeedSerialInterfaceSSCInit (
  VOID
  )
{
  EFI_STATUS                        Status;
  LCPLL_CR_RW_CONTROL_1             LCPLL_CTRL_1;
  LCPLL_CR_RW_CONTROL_2             LCPLL_CTRL_2;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices;
  SYSTEM_CONFIGURATION              SystemConfiguration;
  UINTN                             VariableSize;

  //
  // static table for the SSC settings (corresponding with the SSC settings 0~-0.5%, 0.1% stepping)
  // Modulation Freq = 32KHz
  //
  SSC_SETTING                     HSSIO_SSC_Select_Table[] = {{ No_SSC, 0x12B, 0 },
                                                              { M01_SSC, 0x12B, 0x1062 },
                                                              { M02_SSC, 0x12B, 0x2BB0 },
                                                              { M03_SSC, 0x12B, 0x46FF },
                                                              { M04_SSC, 0x12B, 0x624D },
                                                              { M05_SSC, 0x12B, 0x7D9C }};

  LCPLL_CTRL_1.Data = SideBandRead32 (0x99, 0x9910);
  LCPLL_CTRL_2.Data = SideBandRead32 (0x99, 0x9914);

  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  VariableSize = sizeof (SYSTEM_CONFIGURATION);

  Status = VariableServices->GetVariable (
                               VariableServices,
                               PLATFORM_SETUP_VARIABLE_NAME,
                               &gEfiSetupVariableGuid,
                               NULL,
                               &VariableSize,
                               &SystemConfiguration
                               );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "\nSetup Variable is not ready for SSC setting! Leave the default system HSSIO SSC settings!!\n\r"));
    return EFI_SUCCESS;
  }

  if (SystemConfiguration.HSSIOSSCEnable) {
    LCPLL_CTRL_2.Fields.ssc_cyc_to_peak_m1 = HSSIO_SSC_Select_Table[SystemConfiguration.HSSIOSSCSelection].Ssc_Cyc_To_Peak;
    LCPLL_CTRL_2.Fields.ssc_frac_step = HSSIO_SSC_Select_Table[SystemConfiguration.HSSIOSSCSelection].Ffs_Frac_Step;
    SideBandWrite32 (0x99, 0x9914, LCPLL_CTRL_2.Data);
    SideBandWrite32 (0x99, 0x9910, 0);
  } else {
    LCPLL_CTRL_1.Fields.ssc_en = SSC_DISABLE;
    LCPLL_CTRL_1.Fields.ssc_en_ovr = SSC_ENABLE;
    SideBandWrite32 (0x99, 0x9910, LCPLL_CTRL_1.Data);
  }
  return EFI_SUCCESS;
}
#endif


/**
  This is the entry point of PEIM

  @param[in]  FileHandle  Handle of the file being invoked.
  @param[in]  PeiServices Describes the list of possible PEI Services.

  @retval     EFI_SUCCESS if it completed successfully.

**/
EFI_STATUS
EFIAPI
PlatformInitPreMemEntryPoint (
  IN       EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_PLATFORM_INFO_HOB            PlatformInfo;
  EFI_STATUS                       Status = EFI_SUCCESS;
  EFI_PEI_PPI_DESCRIPTOR           *PeiPpiDescriptor;
  FIRMWARE_SEC_PERFORMANCE         Performance;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  VOID                             *Memory;
  IA32_DESCRIPTOR                  GdtDscriptor;
  UINT32                           Temp32;
  UINT32                           IfwiVerAddr;
  DRAM_POLICY_PPI                  *DramPolicy;
  EFI_PEI_PPI_DESCRIPTOR           *NewPeiPpiDescriptor;
  EFI_BOOT_MODE                    BootMode;
  CarMapStruc                      *CarMap;
  SYSTEM_CONFIGURATION             SystemConfiguration;
  UINTN                            VariableSize;
  EFI_PEI_HOB_POINTERS             Hob;
  EFI_PLATFORM_INFO_HOB            *PlatformInfoPtr;
  EFI_HOB_GUID_TYPE                *FdoEnabledGuidHob = NULL;
  UINT64                           StartTimerTicker = 0;
  UINT64                           Tick;
  UINTN                            AcpiVarHobSize;
  #if (ENBDT_PF_ENABLE == 1)
  MBP_CURRENT_BOOT_MEDIA           BootMediaData;
  #endif
  PEI_BOARD_PRE_MEM_INIT_PPI       *BoardPreMemInitPpi;
  UINTN                            Instance;
  UINT64                           AcpiVariableSetCompatibility;

  Status = (*PeiServices)->RegisterForShadow (FileHandle);

  if (Status == EFI_ALREADY_STARTED) {
    ImageInMemory = TRUE;
  } else if (Status == EFI_NOT_FOUND) {
    ASSERT_EFI_ERROR (Status);
  }

  if (!ImageInMemory) {

    //
    // Since PEI has no PCI enumerator, set the BAR & I/O space enable ourselves
    //
    ScBaseInit ();
    MultiPlatformGpioProgramPreMem (&StartTimerTicker);
  }

  Status = InstallMonoStatusCode (FileHandle, PeiServices);

  if (!ImageInMemory) {
    //
    // Locate all Board Pre Mem Init PPI instances and call them one by one
    //
    Instance = 0;
    do {
      Status = PeiServicesLocatePpi (
                 &gBoardPreMemInitPpiGuid,
                 Instance,
                 &PeiPpiDescriptor,
                 &BoardPreMemInitPpi
                 );

      if (Status == EFI_NOT_FOUND) {
        break;
      }

      ASSERT_EFI_ERROR (Status);
      DEBUG ((EFI_D_INFO,  "Call Board Pre Mem Init PPI\n"));
      Status = BoardPreMemInitPpi->PreMemInit (PeiServices, BoardPreMemInitPpi);
      ASSERT_EFI_ERROR (Status);

      Instance ++;
    } while (TRUE);
  }

  AsmReadGdtr (&GdtDscriptor);
  DEBUG ((DEBUG_INFO, "GdtDscriptor Base Address:0x%X\n", (UINT32) GdtDscriptor.Base));

  PERF_START_EX (NULL, NULL, NULL, 0, 0x9100);
  SeCUmaEntry (FileHandle, PeiServices);
  PERF_END_EX (NULL, NULL, NULL, 0, 0x9101);

  Status = PeiScPreMemPolicyInit (&StartTimerTicker);
  ASSERT_EFI_ERROR (Status);

  if (!ImageInMemory) {
    if (GdtDscriptor.Base >= 0xFE000000) {
      IfwiVerAddr = GdtDscriptor.Base;
      IfwiVerAddr &= 0xfffff000;  // 4K alignment to get IBBL base address.
      IfwiVerAddr +=0x1000;  // the address of IBBL end
      for (Temp32 = 0; Temp32 < 0x8000; Temp32 += 0x10) {
        CarMap = (CarMapStruc *) (IfwiVerAddr-Temp32);
        if (CarMap->Sign == SIGNATURE_32 ('$','S','I','G')) {
          DEBUG ((DEBUG_INFO, "CarMap Address:0x%X\n", (UINT32) CarMap));
          break;
        }
      }
    }

    //
    // Set PcdIafwPlatformInfo = Real_Silicon + Max_RevId
    //
    PcdSet32S (PcdIafwPlatformInfo, 0x0000FF00);

    //
    // Initialize PlatformInfo HOB
    //
    ZeroMem (&PlatformInfo, sizeof (PlatformInfo));

    PlatformInfo.SsidSvid = (UINT32) CarMap;

    Status = ReadBxtIPlatformIds (PeiServices, &PlatformInfo);

    ASSERT_EFI_ERROR (Status);

    //
    // Build HOB for PlatformInfo
    //
    BuildGuidDataHob (
      &gEfiPlatformInfoGuid,
      &PlatformInfo,
      sizeof (EFI_PLATFORM_INFO_HOB)
      );
    //
    // Attempt to locate SMIP and publish its data to PPI's and PCDs.
    // Currently no reason to check Status, but could add in future.
    //
    // This currently installs gDramPolicyPpiGuid, but may move in future
    //
    Status = SmipInit ((VOID *)CarMap->FITBase, PlatformInfo.BoardId);

    MultiPlatformGpioUpdatePreMem ();

    //
    //Print out Patch version string (BXT)
    //
    AsmWriteMsr64 (0x8B, 0);
    AsmCpuid (0x1, NULL, NULL, NULL, NULL);
    Temp32 = (UINT32) (AsmReadMsr64 (0x8B) >> 32);
    DEBUG ((DEBUG_INFO, "PatchInfo:  0x%08x ", Temp32 ));
    DEBUG ((DEBUG_INFO, "%08x \n", (UINT32) (AsmReadMsr64 (0x8B))));

    //
    // Set the new boot mode for MRC
    //
    Status = UpdateBootMode (PeiServices, &PlatformInfo);
    ASSERT_EFI_ERROR (Status);

    //
    // Initialize MfgMemoryTest PPIs
    //
    Status = PeiServicesInstallPpi (&mMfgMemTestPpi);
    ASSERT_EFI_ERROR (Status);

    //
    // Setting 8254
    // Program timer 1 as refresh timer
    //
    IoWrite8 (0x43, 0x54);
    IoWrite8 (0x41, 0x12);

    //
    // RTC power failure handling
    //
    RtcPowerFailureHandler ();

    #if (ENBDT_PF_ENABLE == 1)
    if (GetBxtSeries() == BxtP) {
      //
      // DDR SSC
      //
      PeiDDRSSCInit ();

      //
      // USB3, PCie, SATA, eDP, DP, eMMC, SD and SDIO SSC
      //
    PeiHighSpeedSerialInterfaceSSCInit ();
    }
    #endif

    #if defined(PRAM_SUPPORT)
    //
    // Install Ppi for BIOS reserved memory
    //
    Status = PeiServicesInstallPpi (&mBiosReservedMemoryPolicyPpi);
    #endif

    Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }
 
    VariableSize = sizeof (AcpiVariableSetCompatibility);
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 ACPI_GLOBAL_VARIABLE,
                                 &gEfiAcpiVariableCompatiblityGuid,
                                 NULL,
                                 &VariableSize,
                                 &AcpiVariableSetCompatibility
                                 );

    AcpiVarHobSize = sizeof (UINT64);
    BuildGuidDataHob (
      &gEfiAcpiVariableCompatiblityGuid,
      &AcpiVariableSetCompatibility,
      sizeof (AcpiVarHobSize)
      );

    DEBUG ((DEBUG_INFO, "AcpiVariableAddr : 0x%08x\n", AcpiVariableSetCompatibility)); 

    PERF_START_EX (NULL, "RstVctr", "IBBL", 1, 0x1000);
    Tick = CarMap->IbblPerfRecord0;
    PERF_END_EX (NULL, "RstVctr", "IBBL", Tick, 0x1001);

    PERF_START_EX (NULL, "InitNEM", "IBBL", Tick, 0x1010);
    Tick = CarMap->IbblPerfRecord1;
    PERF_END_EX (NULL, "InitNEM", "IBBL", Tick, 0x1011);

    PERF_START_EX (NULL, "IBBLSdw", "IBBL", Tick, 0x1020);
    Tick = CarMap->IbblPerfRecord2;
    PERF_END_EX (NULL, "IBBLSdw", "IBBL", Tick, 0x1021);

    PERF_START_EX (NULL, "IBBMLod", "IBBL", Tick, 0x1030);
    Tick = CarMap->IbblPerfRecord3;
    PERF_END_EX (NULL, "IBBMLod", "IBBL", Tick, 0x1031);

    PERF_START_EX (NULL, "IBBMVer", "IBBL", Tick, 0x1040);
    Tick = CarMap->IbblPerfRecord4;
    PERF_END_EX (NULL, "IBBMVer", "IBBL", Tick, 0x1041);

    //
    // Normal boot - build Hob for SEC performance data.
    //
    Performance.ResetEnd = GetTimeInNanoSecond (CarMap->IbblPerfRecord0);
    if (!EFI_ERROR (Status)) {
      BuildGuidDataHob (
        &gEfiFirmwarePerformanceGuid,
        &Performance,
        sizeof (FIRMWARE_SEC_PERFORMANCE)
      );
      DEBUG ((EFI_D_INFO, "FPDT: SEC Performance Hob ResetEnd = %ld\n", Performance.ResetEnd));
    }

  } else {  //PostMem

    Hob.Raw = GetFirstGuidHob (&gEfiPlatformInfoGuid);
    ASSERT (Hob.Raw != NULL);
    PlatformInfoPtr = GET_GUID_HOB_DATA (Hob.Raw);
    CarMap = (CarMapStruc *) (UINT32) PlatformInfoPtr->SsidSvid;
    //
    // Locate and Reinstall necessary PPI's before MemoryCallback is run
    //
    Status = PeiServicesLocatePpi (
               &gDramPolicyPpiGuid,
               0,
               &PeiPpiDescriptor,
               NULL // PPI
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Couldn't locate DRAM Policy PPI, LocatePpi returned %r.\n", Status));
    } else {
      DramPolicy          = (DRAM_POLICY_PPI *)        AllocateZeroPool (sizeof (DRAM_POLICY_PPI));
      NewPeiPpiDescriptor = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
      if ((DramPolicy == NULL) || (NewPeiPpiDescriptor == NULL)) {
        DEBUG ((DEBUG_ERROR, "Couldn't allocate memory for DRAM Policy PPI.\n"));
      } else {
        (*PeiServices)->CopyMem (
                          (VOID *) DramPolicy,
                          (VOID *) PeiPpiDescriptor->Ppi,
                          sizeof (DRAM_POLICY_PPI)
                          );

        NewPeiPpiDescriptor->Ppi = DramPolicy;
        NewPeiPpiDescriptor->Guid = &gDramPolicyPpiGuid;
        NewPeiPpiDescriptor->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
        Status = (**PeiServices).ReInstallPpi (
                                   PeiServices,
                                   PeiPpiDescriptor,
                                   NewPeiPpiDescriptor
                                   );
      }
    }

    if (GdtDscriptor.Base >= 0xFE000000) {
      (*PeiServices)->CopyMem (
                        (VOID *) mGdtTable,
                        (VOID *) GdtDscriptor.Base,
                        GdtDscriptor.Limit + 1
                        );
      GdtDscriptor.Base = (UINT32) mGdtTable;
      AsmWriteGdtr (&GdtDscriptor);
    }

    //
    // Set "Force Volatile Mode" in the variable driver
    // If Firmware Descriptor Override (FDO) boot is enabled
    //
    FdoEnabledGuidHob = GetFirstGuidHob (&gFdoModeEnabledHobGuid);
    if (FdoEnabledGuidHob != NULL) {
      PcdSetBoolS (PcdForceVolatileVariable, TRUE);
    }

    //
    // locate the MfgMemoryTest PPI
    //
    Status = PeiServicesLocatePpi (
               &gPeiMfgMemoryTestPpiGuid,  // GUID
               0,                          // INSTANCE
               &PeiPpiDescriptor,          // EFI_PEI_PPI_DESCRIPTOR
               NULL // PPI
               );
    if (Status == EFI_SUCCESS) {
      //
      // Reinstall the MfgMemoryTest PPI
      //
      Status = PeiServicesReInstallPpi (
                 PeiPpiDescriptor,
                 &mMfgMemTestPpi
                 );
    }

    //
    // locate the TemporaryRamSupport PPI
    //
    Status = PeiServicesLocatePpi (
               &gEfiTemporaryRamSupportPpiGuid,  // GUID
               0,                                // INSTANCE
               &PeiPpiDescriptor,                // EFI_PEI_PPI_DESCRIPTOR
               NULL                              // PPI
               );

    if (Status == EFI_SUCCESS) {
      //
      // Reinstall the Variable PPI
      //
      Status = PeiServicesReInstallPpi (
                 PeiPpiDescriptor,
                 mPeiTemporaryRamSupportPpiPpi
                 );
    }

#if defined(PRAM_SUPPORT)
    //
    // locate the BiosReservedMemory PPI
    //
    Status = PeiServicesLocatePpi (
               &gBiosReservedMemoryPolicyPpiGuid,
               0,
               &PeiPpiDescriptor,
               NULL // PPI
               );

    if (Status == EFI_SUCCESS) {
      Status = PeiServicesReInstallPpi (
                 PeiPpiDescriptor,
                 &mBiosReservedMemoryPolicyPpi
                 );
    }
#endif

    //
    // Initialize Stall PPIs
    //
    Status = PeiServicesInstallPpi (&mInstallStallPpi);
    ASSERT_EFI_ERROR (Status);

    Status = PeiServicesGetBootMode (&BootMode);
#if (ENBDT_PF_ENABLE == 1)
    if (BootMode == BOOT_ON_S3_RESUME) {
      Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariableServices);
      if (EFI_ERROR (Status)) {
        ASSERT_EFI_ERROR (Status);
        return Status;
      }
      VariableSize = sizeof (SYSTEM_CONFIGURATION);
      Status = VariableServices->GetVariable (
                                   VariableServices,
                                   PLATFORM_SETUP_VARIABLE_NAME,
                                   &gEfiSetupVariableGuid,
                                   NULL,
                                   &VariableSize,
                                   &SystemConfiguration
                                   );

      if (!EFI_ERROR (Status)) {
        SetMem (&BootMediaData, sizeof (MBP_CURRENT_BOOT_MEDIA), 0x0);
        switch (SystemConfiguration.CseBootDevice) {
          case 0:
            DEBUG ((DEBUG_INFO, "CSE Boot Device is EMMC.\n"));
            Status = (*PeiServices)->InstallPpi (PeiServices, mCseEmmcSelectPpiList);
            break;
          case 1:
            DEBUG ((DEBUG_INFO, "CSE Boot Device is UFS.\n"));
            Status = (*PeiServices)->InstallPpi (PeiServices, mCseUfsSelectPpiList);
            break;
          case 2:
            DEBUG ((DEBUG_INFO, "CSE Boot Device is SPI.\n"));
            Status = (*PeiServices)->InstallPpi (PeiServices, mCseSpiSelectPpiList);
            break;
          default:
            DEBUG ((EFI_D_ERROR, "\nCSE Boot device is unknown. Cannot continue!\n"));
            CpuDeadLoop();
            break;

        }
        BootMediaData.PhysicalData = SystemConfiguration.CseBootDevice;
        //
        // Build HOB for BootMediaData
        //
        BuildGuidDataHob (
          &gEfiBootMediaHobGuid,
          &BootMediaData,
          sizeof (MBP_CURRENT_BOOT_MEDIA)
          );
      }
    }
#endif

    //
    // copy IBBM from Cache to DRAM. the hardcoded address need to be changed
    // to use the parameter in IBBL.
    //
    Memory = AllocatePages (EFI_SIZE_TO_PAGES (PcdGet32 (PcdFlashFvIBBMSize)));
    if (Memory != NULL) {
      CopyMem (Memory , (VOID *) CarMap->IBBBase, PcdGet32 (PcdFlashFvIBBMSize));
      DEBUG ((DEBUG_INFO, "IBBM address: %x\n", Memory));
      PeiServicesInstallFvInfoPpi (
        NULL,
        (VOID *) Memory,
        PcdGet32 (PcdFlashFvIBBMSize),
        NULL,
        NULL
        );
    } else  {
      ASSERT (FALSE);
    }

    DEBUG ((DEBUG_INFO, "PreMem Policy Init - Start\n"));
    //
    // Initialize Pre-Mem PEI Platform Policy
    //
    Status = PeiPolicyInitPreMem ();
    ASSERT_EFI_ERROR (Status);
    DEBUG ((DEBUG_INFO, "PreMem Policy Init - End\n\n"));

    //
    // Register Notify Callback to process OBB loading.
    // In FSP+Wrapper, the MTRRs are set after TempRamExit, not gEfiPeiMemoryDiscoveredPpiGuid.
    //
    Status = PeiServicesNotifyPpi (&mFspTempRamExitList[0]);
    ASSERT_EFI_ERROR (Status);
  } //end PostMem

  DEBUG ((DEBUG_INFO, "PeiInitPlatform end\n"));

  return Status;
}

//
// Read Platform ID for IOTG Platforms
//
EFI_STATUS
ReadBxtIPlatformIds (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN OUT EFI_PLATFORM_INFO_HOB  *PlatformInfoHob
  )
{
  UINT8                       BoardId = 0;
  UINT8                       FabId = 0;

  DEBUG ((DEBUG_INFO, "Port(0x62) = %02X\n", IoRead8 (0x62)));

  PlatformInfoHob->ECPresent = 0;
  BoardId = (UINT8) PcdGet8 (PcdBoardId);
  FabId = (UINT8) PcdGet8 (PcdFabId);

  PlatformInfoHob->BoardId = BoardId;
  PlatformInfoHob->BoardRev = FabId;

  DEBUG ((DEBUG_INFO, "BoardId:  [0x%08x]\n", PlatformInfoHob->BoardId));
  DEBUG ((DEBUG_INFO, "FabId:    [0x%08x]\n", PlatformInfoHob->BoardRev));

  return EFI_SUCCESS;
}

