/** @file
  Initializes USB Controllers.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Private/Library/UsbCommonLib.h>
#include <Library/SteppingLib.h>
#include <Library/PmcIpcLib.h>
#include <Library/TimerLib.h>
#include <Library/ConfigBlockLib.h>
#include <Library/ScPlatformLib.h>

EFI_BOOT_MODE mBootMode = BOOT_WITH_FULL_CONFIGURATION;


/**
  Set UNUSED bit to "1" if the port has no physical SSIC Device connected.

  @param[in] UsbConfig               The SC Policy for USB configuration
  @param[in] XhciMmioBase            Memory base address of XHCI Controller

  @retval    EFI_INVALID_PARAMETER   The parameter of ScPolicy is invalid
  @retval    EFI_SUCCESS             The function completed successfully

**/
VOID
XhciSsicInit (
 IN  SC_USB_CONFIG *UsbConfig,
 IN  UINT32         XhciMmioBase,
 IN  EFI_BOOT_MODE  BootMode
 );

/**
  Program Xhci Port Disable Override

  @param[in] UsbConfig            The SC Policy for USB configuration
  @param[in] XhciMmioBase         XHCI Memory Space Address

  @retval    None

**/
VOID
XhciPortDisableOverride (
  IN  SC_USB_CONFIG  *UsbConfig,
  IN  UINTN          XhciMmioBase
  );

/**
  Setup XHCI Over-Current Mapping

  @param[in] UsbConfig            The SC Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval    None

**/
VOID
XhciOverCurrentMapping (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINTN                       XhciPciMmBase
  );

/**
  Configures SC USB controller

  @param[in] UsbConfig               The SC Policy for USB configuration
  @param[in] XhciMmioBase            Memory base address of XHCI Controller
  @param[in] BusNumber               PCI Bus Number of the SC device
  @param[in] FuncDisableReg          Function Disable Register
  @param[in] BootMode                current boot mode

  @retval    EFI_INVALID_PARAMETER   The parameter of ScPolicy is invalid
  @retval    EFI_SUCCESS             The function completed successfully

**/
EFI_STATUS
EFIAPI
CommonUsbInit (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINT8                       BusNumber,
  IN OUT UINT32                   *FuncDisableReg,
  IN  EFI_BOOT_MODE               BootMode
  )
{
  UINTN  XhciPciMmBase;

  DEBUG ((DEBUG_INFO, "CommonUsbInit() - Start\n"));
  mBootMode = BootMode;

  XhciPciMmBase = MmPciBase (
                    BusNumber,
                    PCI_DEVICE_NUMBER_XHCI,
                    PCI_FUNCTION_NUMBER_XHCI
                    );
  //
  // If xHCI is disabled by fuse or soft-strap,
  // set the function disable bit and then return.
  //
  if (MmioRead32 ((UINTN) (XhciPciMmBase)) == 0xFFFFFFFF) {
    DEBUG ((EFI_D_INFO , "xHCI not present, skipping.\n"));
    UsbConfig->Usb30Settings.Mode = XHCI_MODE_OFF;
    *FuncDisableReg |= B_PMC_FUNC_DIS_USB_XHCI;
    return EFI_SUCCESS;
  }

  //
  // To disable a host controller, the System BIOS must first place the
  // xHCI controller in RTD3Hot state by program PM_CS.PS, D21:F0:0x74 [1:0] = 11b
  // and then set the Function Disable register to disable the xHCI controller.
  //
  if (UsbConfig->Usb30Settings.Mode == XHCI_MODE_OFF) {

    //
    // Putting xHCI into D3 Hot State
    //
    DEBUG ((EFI_D_INFO , "Putting xHCI into D3 Hot State.\n"));
    MmioOr32 ((UINTN) (XhciPciMmBase + R_XHCI_PWR_CNTL_STS), B_XHCI_PWR_CNTL_STS_PWR_STS);

    SideBandAndThenOr32 (
      0xA9,
      (0x0600 + 0x001C),
      0xFFFFFFFF,
      BIT8
      );
    *FuncDisableReg |= B_PMC_FUNC_DIS_USB_XHCI;
    return EFI_SUCCESS;
  }

  //
  // Assign memory resources
  //
  XhciMemorySpaceOpen (
    UsbConfig,
    XhciMmioBase,
    XhciPciMmBase
    );

  CommonXhciHcInit (
    UsbConfig,
    XhciMmioBase,
    XhciPciMmBase,
    BootMode
    );

  XhciSsicInit (
    UsbConfig,
    XhciMmioBase,
    BootMode
    );

  //
  // Setup USB Over-Current Mapping.
  //
  XhciOverCurrentMapping (
    UsbConfig,
    XhciPciMmBase
    );

  //
  // Program USB Port Disable Override Capability
  //
  XhciPortDisableOverride (
    UsbConfig,
    XhciMmioBase
    );

  //
  // Clear memory resources
  //
  XhciMemorySpaceClose (
    UsbConfig,
    XhciMmioBase,
    XhciPciMmBase
    );

  DEBUG ((EFI_D_INFO, "CommonUsbInit() - End\n"));

  return EFI_SUCCESS;
}


/**
  Set UNUSED bit to "1" if the port has no physical SSIC Device connected.

  @param[in] UsbConfig               The SC Policy for USB configuration
  @param[in] XhciMmioBase            Memory base address of XHCI Controller

  @retval    EFI_INVALID_PARAMETER   The parameter of ScPolicy is invalid
  @retval    EFI_SUCCESS             The function completed successfully

**/
VOID
XhciSsicInit (
 IN  SC_USB_CONFIG     *UsbConfig,
 IN UINT32              XhciMmioBase,
 IN  EFI_BOOT_MODE      BootMode
 )
{
  UINT32  Data32;
  UINT32  Iteration;

  DEBUG ((DEBUG_INFO, "XhciSsicInit() - Start\n"));

  if (UsbConfig->SsicConfig.SsicPort[0].Enable == TRUE) {
    Data32 = B_XHCI_PROG_DONE;
  } else {
    //
    // Set UNUSED bit to "1" if the port has no physical SSIC Device connected.
    //
    Data32 = B_XHCI_SSIC_PORT_UNUSED | B_XHCI_PROG_DONE;
  }
  MmioOr32 (
    XhciMmioBase + R_XHCI_SSIC_CFG_2_PORT1,
    Data32
    );

  if (UsbConfig->SsicConfig.SsicPort[1].Enable == TRUE) {
    Data32 = B_XHCI_PROG_DONE;
  } else {
    //
    // Set UNUSED bit to "1" if the port has no physical SSIC Device connected.
    //
    Data32 = B_XHCI_SSIC_PORT_UNUSED | B_XHCI_PROG_DONE;
  }
  MmioOr32 (
    XhciMmioBase + R_XHCI_SSIC_CFG_2_PORT2,
    Data32
    );

  DEBUG ((DEBUG_INFO, "XhciSsicInit:base:0x%x\n",XhciMmioBase));
  Data32 = MmioRead32(XhciMmioBase+R_XHCI_SSIC_CFG_2_PORT1);
  DEBUG ((DEBUG_INFO, "0x%x:0x%x\n", (XhciMmioBase+R_XHCI_SSIC_CFG_2_PORT1), Data32));

  Data32 = MmioRead32(XhciMmioBase+R_XHCI_SSIC_CFG_2_PORT2);
  DEBUG ((DEBUG_INFO, "0x%x:0x%x\n", (XhciMmioBase+R_XHCI_SSIC_CFG_2_PORT2), Data32));

  if ((UsbConfig->SsicConfig.SsicPort[0].Enable == TRUE) ||
      (UsbConfig->SsicConfig.SsicPort[1].Enable == TRUE)) {
    if (BootMode == BOOT_ON_S5_RESUME) {
      DEBUG ((DEBUG_INFO, "Boot on S5 resume...\n"));
      MicroSecondDelay (100);  // 100us
      //
      // Clear SSIC ports PP bits
      //
      if (GetBxtSeries () == BxtP) {
        //
        // Clear SSIC ports PP bits
        //
        MmioAnd32 (
          XhciMmioBase + R_BXTP_XHCI_PORTSC2USB3,
          (UINT32) ~(B_XHCI_PORTSCXUSB3_PP)
          );
        MmioAnd32 (
          XhciMmioBase + R_BXTP_XHCI_PORTSC3USB3,
          (UINT32) ~(B_XHCI_PORTSCXUSB3_PP)
          );
        //
        // Wait for 150ms for Host to complete the DSP Disconnect protocol
        //
        MicroSecondDelay (150 * 1000);

        MmioOr32 (
          XhciMmioBase + R_BXTP_XHCI_PORTSC2USB3,
          (UINT32) B_XHCI_PORTSCXUSB3_PP
          );

        MmioOr32 (
          XhciMmioBase + R_BXTP_XHCI_PORTSC3USB3,
          (UINT32) B_XHCI_PORTSCXUSB3_PP
          );
      } else {
        MmioAnd32 (
          XhciMmioBase + R_BXT_XHCI_PORTSC2USB3,
          (UINT32) ~(B_XHCI_PORTSCXUSB3_PP)
          );
        MmioAnd32 (
          XhciMmioBase + R_BXT_XHCI_PORTSC3USB3,
          (UINT32) ~(B_XHCI_PORTSCXUSB3_PP)
          );
        //
        // Wait for 150ms for Host to complete the DSP Disconnect protocol
        //
        MicroSecondDelay (150 * 1000);

        MmioOr32 (
          XhciMmioBase + R_BXT_XHCI_PORTSC2USB3,
          (UINT32) B_XHCI_PORTSCXUSB3_PP
          );
        MmioOr32 (
          XhciMmioBase + R_BXT_XHCI_PORTSC3USB3,
          (UINT32) B_XHCI_PORTSCXUSB3_PP
          );
      }
      //
      // Set HCRST after S5, HCRST is cleared by HW when reset process is complete
      //
      MmioOr32 (
        XhciMmioBase + R_XHCI_USBCMD,
        (UINT32) B_XHCI_USBCMD_HCRST
        );
      //
      // Waiting for reset complete
      // The controller requires that its MMIO space not be accessed for a minimum of 1 ms after
      // an HCRST is triggered which includes reading the HCRST bit
      //
      MicroSecondDelay (1000); // 1ms
      for (Iteration = 0; Iteration < 8000; Iteration++) {
        if ((MmioRead32 (XhciMmioBase + R_XHCI_USBCMD) & B_XHCI_USBCMD_HCRST) == 0) {
          break;
        }
        MicroSecondDelay (100); // 100us
      }
    }
  }

  DEBUG ((DEBUG_INFO, "XhciSsicInit() - End\n"));

  return;
}


/**
  Performs basic configuration of SC USB3 (xHCI) controller.

  @param[in] UsbConfig            The SC Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval    None

**/
VOID
CommonXhciHcInit (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase,
  IN  EFI_BOOT_MODE               BootMode
  )
{
  EFI_STATUS   Status;
  UINT32       BitMask;
  UINT32       BitValue;
  BXT_STEPPING BxtStep;
  UINT32       Data32And;
  UINT32       Data32Or;

  Status   = EFI_TIMEOUT;
  BitMask  = 0;
  BitValue = 0;
  BxtStep  = BxtStepping ();

  //
  // Set xHCI MSI_NEXT to 0x90 to point to the DevIdle capability structure
  //
  MmioAndThenOr32(XhciPciMmBase + R_XHCI_MSI_CAPID, 0xFFFF00FF, V_XHCI_MSI_NEXT << 8);

  //
  // HCSPARAMS1 - Structural Parameters 1
  // Address Offset:   0x04 - 0x07
  //
  BitValue = MmioRead32 (XhciMmioBase + R_XHCI_XECP_SUPP_USB3_2);
  BitValue = ((BitValue >> 8) & 0xff) + ((BitValue & 0xff) - 1);
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HCSPARAMS1),
    (UINT32) 0x00FFFFFF,
    (UINT32) (BitValue << 24)
    );
  BitValue = 0;
  //
  // HCSPARAMS3 - Structural Parameters 3
  // Address Offset:   0x0C - 0x0F
  // Set xHCIBAR + 0Ch[7:0] = 0Ah and [31:16] = 200h
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HCSPARAMS3),
    (UINT32) 0x0000FF00,
    (UINT32) 0x0200000A
    );

  //
  // XHCC2 - XHC System Bus Configuration 2
  // Address Offset: 44-47h
  // Value: [25] 1b, [24:22] 111b, [19:14] 3fh, [11] 0b, [10] 1b, [9:8] 10b, [7:6] 10b, [5:3] 001b, [2:0] 111b
  //
  Data32And = 0xFC303000;
  Data32Or =  0x03CFC68F;
  if (BxtStep == BxtPA0 && BootMode == BOOT_ON_S3_RESUME) {

    Data32Or = 0x000FC688;
  }
  MmioAndThenOr32 (
    XhciPciMmBase + R_XHCI_XHCC2,
    Data32And,
    Data32Or
    );


  if (GetBxtSeries() == BxtP) {
  //
  // PCE - Power Control Enables
  // Address Offset:   A2h - A3h
  // Value: [5] 0b, [3] 1b, [2] 1b, [1] 1b, [0] 0b
    MmioAndThenOr16 (
      XhciPciMmBase + R_XHCI_PCE,
      (UINT16)~(B_XHCI_PCE_SPE | B_XHCI_PCE_HAE),
      (UINT16)(B_XHCI_PCE_SE | B_XHCI_PCE_D3HE | B_XHCI_PCE_I3E)
      );
  } else {
  //
  // PCE - Power Control Enables
  // Address Offset:   A2h - A3h
  // Value: [5] 1b, [3] 1b, [2] 1b, [1] 1b, [0] 0b
  //
    MmioAndThenOr16 (
      XhciPciMmBase + R_XHCI_PCE,
      (UINT16) ~(B_XHCI_PCE_SPE),
      (UINT16) (B_XHCI_PCE_SE | B_XHCI_PCE_D3HE | B_XHCI_PCE_I3E | B_XHCI_PCE_HAE)
      );
  }

  //
  // HSCFG2 - High Speed Configuration 2
  // Address Offset:   A4h - A5h
  // Value: [15] 0b, [14] 0b, [13] 0b, [12:11]  11b
  //
  MmioAndThenOr16 (
    XhciPciMmBase + R_XHCI_HSCFG2,
    (UINT16) ~(B_XHCI_HSCFG2_HSAAIM | B_XHCI_HSCFG2_HSOAAPEPM | B_XHCI_HSCFG2_HSIAAPEPM),
    (UINT16) (B_XHCI_HSCFG2_HSIIPAPC)
    );

  //
  // SSCFG1 - SuperSpeed Configuration 1
  // Address Offset:   A8h - ABh
  // Value: [17] 1b, [14] 1b
  //
  MmioAndThenOr32 (
    XhciPciMmBase + R_XHCI_SSCFG1,
    0xFFFFFFFF,
    (UINT32) (B_XHCI_SSCFG1_LFPS | B_XHCI_SSCFG1_PHY_U3)
    );

  //
  // XECP_CMDM_CTRL_REG1 - Command Manager Control 1
  // Address Offset: 818C-818Fh
  // Value: [20] 0b, [16] 1b, [8] 0b
  //
  MmioAndThenOr32 (
    (XhciMmioBase+R_XHCI_XECP_CMDM_CTRL_REG1),
    (UINT32) ~(B_XHCI_XECP_MAX_EP | B_XHCI_XECP_CLR_CTX_ENSLOT),
    (UINT32) (B_XHCI_XECP_TSP)
    );

  //
  // XECP_CMDM_CTRL_REG3 - Command Manager Control 3
  // Address Offset: 8194-8197h
  // Value: [25] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase+R_XHCI_XECP_CMDM_CTRL_REG3),
    0xFFFFFFFF,
    (UINT32) (B_XHCI_XECP_STOP_EP)
    );

  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HOST_CTRL_BW_MAX_REG + 4),
    0xFFFF000F,
    (UINT32) (V_XHCI_HOST_CTRL_BW_MAX_REG_TT << 4)
    );

  //
  // PMCTRL - Power Management Control
  // Address Offset: 80A4-80A7h
  // Value: [31] 0b, [30] 1b, [29] 0b, [25:22] 1111b, [16] 1b, [7:4] 9h, [3] 1 for BXTA0, 0 for all BXT [2] 1b
  //
  Data32And = 0x143EFF03;
  if (BxtStep <= BxtA1) {
    Data32Or = 0x4BC1009C;
  } else {
    Data32Or = 0x4BC10094;
  }
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_PMCTRL),
    Data32And,
    Data32Or
    );

  //
  // PGCBCTRL - PGCB Control
  // Address Offset: 80A8-80ABh
  // Value: [24] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_PGCBCTRL),
    ~0u,
    (UINT32) (B_XHCI_PGCBCTRL_RESET_PREP_DIS)
    );

  //
  // SSPE - Super Speed Port Enables
  // Address Offset: 0x80B8 - 0x80BC
  // Value: [30] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HOST_CONTROLLER_SSPE),
    ~0u,
    (UINT32) (B_XHCI_ENCLCCS)
    );

  //
  // AUX_CTRL_REG1 - AUX Power Management Control
  // Address Offset: 80E0-80E3h
  // Value: [22] 0b, [16] 0b, [9] 0b, [6] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase+R_XHCI_AUX_CTRL_REG1),
    (UINT32) ~(B_XHCI_AUX_ISOLATION | B_XHCI_AUX_PORT_CG | B_XHCI_AUX_CG),
    (UINT32) (B_XHCI_AUX_P2)
    );

  //
  // HOST_CTRL_SCH_REG - Host Control Scheduler
  // Address Offset: 8094-8097h
  // Value: [23] 1b, [22] 1b, [21] 0b, [14] 0b, [6] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HOST_CTRL_SCH_REG),
    (UINT32) ~(B_XHCI_HOST_CTRL_DIS_SCH_PKT | B_XHCI_HOST_CTRL_DIS_PKT_CHK),
    (UINT32) (B_XHCI_HOST_CTRL_DIS_ASYNC | B_XHCI_HOST_CTRL_EN_EP_PPL | B_XHCI_HOST_CTRL_EN_1P_EP_PPL)
    );

  //
  // HOST_CTRL_PORT_LINK_REG - SuperSpeed Port Link Control
  // Address Offset: 0x80EC - 0x80EF
  // Value: [19] 1b, [17] 1b
  //
  Data32And = ~0u;
  Data32Or = (UINT32) (B_XHCI_HOST_CTRL_EN_TS_EXIT | B_XHCI_HOST_CTRL_PORT_INIT_TIMEOUT);
  if (UsbConfig->DisableComplianceMode == TRUE) {
    Data32Or |= B_XHCI_DIS_LINK_CM;
  }
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HOST_CTRL_PORT_LINK_REG),
    Data32And,
    Data32Or
    );
  //
  // USB2_LINK_MGR_CTRL_REG1 - USB2 Port Link Control 1, 2, 3, 4
  // Address Offset: 0x80F0 - 0x80FF [121] 1b, [20] 0b
  // [127:96] is mapped to DW4 at offset 80FCh-80FFh [25] 1b
  // [31:0]   is mapped to DW1 at offset 80F0h-80F3h [20] 0b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_USB2_LINK_MGR_CTRL_REG1),
    (UINT32) ~(B_XHCI_USB2_LINK_L1_EXIT),
    0x0
    );

  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_USB2_LINK_MGR_CTRL_REG1_CONTROL4),
    0xFFFFFFFF,
    (UINT32)(B_XHCI_USB2_LINK_PRV_L1_ENTRY)
    );

  //
  // HOST_CTRL_TRM_REG2 - Host Controller Transfer Manager Control 2
  // Address Offset: 8110-8113h
  // Value: [2] 0b, [20] 1b, [11] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HOST_CTRL_TRM_REG2),
    (UINT32) ~(B_XHCI_HOST_CTRL_REDEEM),
    (UINT32) (B_XHCI_HOST_CTRL_MAX_BURST_CHK | B_XHCI_HOST_CTRL_TRF_PIPE)
    );

  //
  // Set xHCIBAR + 8154h[31, 21,13] to 1b, 0b, 1b
  //
  MmioAndThenOr32(
    (XhciMmioBase + R_XHCI_AUX_CTRL_REG2),
    (UINT32)~(B_XHCI_AUX2_P2_D3HOT),
    (UINT32) (B_XHCI_AUX2_L1P2_EXIT|B_XHCI_AUX2_PHY_P3)
    );

  //
  // xHCI Aux Clock Control Register
  // Address Offset: 0x816C - 0x816F
  // [13:12] 00b, [11:8] 0h
  // [19] 1b, [18] 1b, [17] 1b, [14] 1b, [5] 0b, [4] 1b, [3] 1b, [2] 1b,
  //

  Data32And = 0xFFF180C3;
  Data32Or  = 0x0002401C;
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_AUX_CLOCK_CTRL_REG),
    Data32And,
    Data32Or
    );

  //
  // HOST_IF_PWR_CTRL_REG0 - Power Scheduler Control 0
  // Address Offset: 8140-8143h
  // Value: [31:24] 0xFF, [11:0] 0x03C
  //         [23:12] 0x080 for BXT A step
  //         [23:12] 0x00F for all others steppings
  //
  Data32And = 0;
  Data32Or  = 0xFF00F03C;
  if (BxtStep <= BxtA1) {
    Data32Or  = 0xFF08003C;
  }
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HOST_IF_PWR_CTRL_REG0),
    Data32And,
    Data32Or
    );
  //
  // HOST_IF_PWR_CTRL_REG1 - Power Scheduler Control 1
  // Address Offset: 8144-8147h
  // Value: [8] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HOST_IF_PWR_CTRL_REG1),
    ~0u,
    (UINT32) (B_XHCI_HSII)
    );

  //
  // xHC Latency Tolerance Parameters - LTV Control
  // Address Offset: 0x8174 - 0x8177
  // Value: [24] 1b, 0b for BXTPA0 [11:0] 0xC0A
  //
  Data32And = (UINT32) ~(B_XHCI_USB2_PORT_L0_LTV | B_XHCI_XLTRE);
  Data32Or = 0x01000C0A;
  if (BxtStep <= BxtPA0){
    Data32Or = 0x00000C0A;
  }
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_LATENCY_TOLERANCE_PARAMETERS_LTV_CONTROL),
    Data32And,
    Data32Or
    );
  //
  // xHC Latency Tolerance Parameters - High Idle Time Control
  // Address Offset: 0x817C - 0x817F
  // Value: [28:16] 0x0332, [12:0] 0x00A3
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_LATENCY_TOLERANCE_PARAMETERS_HIGH_IDLE_TIME_CONTROL),
    0xE000E000,
    0x033200A3
    );

  //
  // xHC Latency Tolerance Parameters - Medium Idle Time Control
  // Address Offset: 0x8180 - 0x8183
  // Value: [28:16] 0x00CB, [12:0] 0x0028
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_LATENCY_TOLERANCE_PARAMETERS_MEDIUM_IDLE_TIME_CONTROL),
    0xE000E000,
    0x00CB0028
    );

  //
  // xHC Latency Tolerance Parameters - Low Idle Time Control
  // Address Offset: 0x8184 - 0x8187
  // Value: [28:16] 0x0064, [12:0] 0x001E
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_LATENCY_TOLERANCE_PARAMETERS_LOW_IDLE_TIME_CONTROL),
    0xE000E000,
    0x0064001E
    );

  //
  // USB2 PHY Power Management Control
  // Address Offset: 8164-8167h
  // Value: [7:0] 11111100b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_USB2_PHY_POWER_MANAGEMENT_CONTROL),
    (UINT32) ~(B_XHCI_RX_BIAS_CHT_DIS | B_XHCI_TX_BIAS_CHT_DIS),
    (UINT32) (B_XHCI_CMAI | B_XHCI_TTEAI | B_XHCI_IDMAAI | B_XHCI_ODMAAI | B_XHCI_TMAI | B_XHCI_SAI)
    );

  //
  // Host Controller Misc Reg
  // Address Offset: 0x80B0 - 0x80B3
  // Value: [24:23] 01b, [18:16] 000b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_HOST_CONTROLLER_MISC_REG),
    (UINT32) ~(B_XHCI_EXTRA_UFRAME | B_XHCI_LATE_FID_CHK_DIS),
    (UINT32) (B_XHCI_LATE_FID_TTE_DIS)
    );

  //
  // Host Controller Misc Reg 2
  // Address Offset: 0x80B4 - 0x80B7
  // Value: [2] 1b
  //             0b for BXT-P B0 and above
  //         [5] 1b for BXT A0 step
  //         [5] 0b for all others steppings
  //
  if (BxtStep == BxtA0) {
    MmioOr32 (
      (XhciMmioBase + R_XHCI_HOST_CONTROLLER_MISC2REG),
      (UINT32) (B_XHCI_FRAME_TIM_SEL | B_XHCI_WARM_PORT_RESET_ON_DISC_PORT_DIS)
      );
  } else if (BxtStep >= BxtPB0) {
    MmioAnd32 (
      (XhciMmioBase + R_XHCI_HOST_CONTROLLER_MISC2REG),
      (UINT32) ~(B_XHCI_FRAME_TIM_SEL | B_XHCI_WARM_PORT_RESET_ON_DISC_PORT_DIS)
      );
  } else {
    MmioAndThenOr32 (
      (XhciMmioBase + R_XHCI_HOST_CONTROLLER_MISC2REG),
      (UINT32) ~(B_XHCI_FRAME_TIM_SEL),
      (UINT32) (B_XHCI_WARM_PORT_RESET_ON_DISC_PORT_DIS)
      );
  }
  //
  // LFPSONCOUNT - LFPS On Count
  // Address Offset: 0x81B8 - 0x81BB
  // Value: [15:10] 02h, [9:0] 02h
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_LFPS_ON_COUNT),
    (UINT32) ~(B_XHCI_XLFPSONCNTSSIC | B_XHCI_XLFPSONCNTSS),
    (UINT32) (0x0802)
    );

  //
  // D0I2CTRL - D0I2 Control Register
  // Address Offset: 81BC-81BFh
  // Value: [31] 1b, [29:26] 04h, [25:22] 04h, [21] 0b, [20:16] 04h, [15:4] 20h, [3:0] 0h
  //

  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_D0I2_CONTROL),
    0x40000000,
    0x91040200
    );

  //
  // D0I2SchAlarmCtrl - D0i2 Scheduler Alram Control Reg
  // Address Offset: 81C0-81C3h
  // Value: [28:16] 0Fh, [12:0] 05h
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_D0I2_SCH_ALARM_CTRL),
    0xE000E000,
    0x000F0005
    );

  //
  // USB2PMCTRL - USB2 Power Management Control
  // Address Offset:   0x81C4 - 0x81C7
  // Value: [11] 1b, [10:8] 001b, [3:2] 00b, [1:0] 11b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_XHCI_USB2_PM_CTRL),
    (UINT32) ~(B_XHCI_U2PSPGEHC | B_XHCI_U2PSUSPGP),
    (UINT32) (B_XHCI_U2PSPGPSCBP | BIT8 | B_XHCI_U2PSUSPGP_Shadow)
    );

  //
  // Set xHCIBAR + 8178h[12:0] to 0h
  //
  MmioAndThenOr32 (
    (XhciMmioBase + 0x8178),
    (UINT32)~(0xFFFFE000),
    (UINT32) (0x00)
    );

  if (UsbConfig->XdciConfig.Enable != ScDisabled) {
    MmioAndThenOr32 (
      (XhciMmioBase + R_XHCI_MEM_DUAL_ROLE_CFG0),
      0,
      (UINT32) BIT24
      );

    MmioAndThenOr32 (
      (XhciMmioBase + R_XHCI_MEM_DUAL_ROLE_CFG0),
      0,
      (UINT32) (BIT21 | BIT20)
      );
    //
    // Step 2
    // Poll xHCI BAR + DUAL_ROLE_CFG1 [29] until it reads 0b or else 5ms timeout.
    //
    BitMask  = (UINT32) (BIT29);
    BitValue = 0;
    Status = ScMmioPoll32 (
              (UINTN) (XhciMmioBase + R_XHCI_MEM_DUAL_ROLE_CFG1),
              BitMask,
              BitValue,
              50
              );
    if (Status == EFI_TIMEOUT) {
      DEBUG ((DEBUG_ERROR | DEBUG_INFO, "USBCommonlib.c Timeout while polling on xHCI BAR + R_XHCI_MEM_DUAL_ROLE_CFG1 [29] for 0b\n"));
    }
  }
  DEBUG ((DEBUG_INFO, "R_XHCI_MEM_DUAL_ROLE_CFG0=%x\n", MmioRead32(XhciMmioBase + R_XHCI_MEM_DUAL_ROLE_CFG0)));
}


/**
  Initialization XHCI Clock Gating registers

  @retval   None

**/
VOID
ConfigureXhciClockGating (
  VOID
  )
{
  UINTN           XhciPciMmBase;
  UINT32          Data32And;
  UINT32          Data32Or;
  BXT_STEPPING    BxtStep;

  XhciPciMmBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_XHCI,
                    PCI_FUNCTION_NUMBER_XHCI
                    );

  if (XhciPciMmBase == 0xFFFFFFFF) { // controller absence or function disable
    DEBUG ((DEBUG_INFO , "XHCI controller absence or function disable, skip setting clock gating.\n"));
    return;
  }

  BxtStep = BxtStepping ();

  //
  // XHCC1 - XHC System Bus Configuration 1
  // Address Offset: 40-43h
  // Value: [21:19] 110b, [18] 1b, [8] 1b
  // Writes to this registers needs to be performed per bytes to avoid touching bit 31
  // Bit 31 is used to lock RW/L bits and can be writen once.
  //
  MmioOr8 (
    XhciPciMmBase + R_XHCI_XHCC1 + 1,
    BIT0
    );
  MmioOr8 (
    XhciPciMmBase + R_XHCI_XHCC1 + 2,
    (BIT5 | BIT4 | BIT2)
    );

  //
  // HSCFG1 - High Speed Configuration 1
  // Address Offset: AC-AFh
  // Value: [19] 0b, [18] 1b
  //
  MmioAndThenOr32 (
    (UINTN) (XhciPciMmBase + R_XHCI_HSCFG1),
    (UINT32) ~(B_XHCI_HSCFG1_UTMI_SUSPEND_CG),
    (UINT32) (B_XHCI_HSCFG1_UTMI_SPEED_CG)
    );

  //
  // XHCLKGTEN - Clock Gating
  // Address Offset: 50-53h
  // Value: [28] 0b,  [27:24] 1111b, [23:20] 1100b, [19:16] 1110b, [15:8] 01101110b, [7:0] 01011111b
  //
  Data32And = 0xE0000000;
  if (BxtStep == BxtPA0 && mBootMode == BOOT_ON_S3_RESUME) {

    Data32Or  = 0x0FDF6D3F;
    MmioAndThenOr32 (
      (UINTN) (XhciPciMmBase + R_XHCI_XHCLKGTEN),
      Data32And,
      Data32Or
      );
    DEBUG ((DEBUG_INFO , "Putting xHCI into D3 Hot State.\n"));
    MmioOr32 ((UINT32) (XhciPciMmBase + R_XHCI_PWR_CNTL_STS), B_XHCI_PWR_CNTL_STS_PWR_STS);
    MicroSecondDelay (20);
    DEBUG ((DEBUG_INFO , "Putting xHCI into D0 State.\n"));
    MmioAnd32 ((UINT32) (XhciPciMmBase + R_XHCI_PWR_CNTL_STS), (UINT32) ~ (B_XHCI_PWR_CNTL_STS_PWR_STS));
  } else {
    Data32Or  = 0x0FCE6E5F;
    MmioAndThenOr32 (
      (UINTN) (XhciPciMmBase + R_XHCI_XHCLKGTEN),
      Data32And,
      Data32Or
      );
  }
}


/**
  Lock USB registers before boot

  @param[in] ScPolicy    The SC Policy

  @retval    None

**/
VOID
UsbInitBeforeBoot (
  IN SI_POLICY_HOB *SiPolicyHob,
  IN SC_POLICY_HOB *ScPolicyHob
  )
{
  UINTN         XhciPciMmBase;
  SC_USB_CONFIG *UsbConfig;
  EFI_STATUS    Status;

  XhciPciMmBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_XHCI,
                    PCI_FUNCTION_NUMBER_XHCI
                    );
  Status = GetConfigBlock ((VOID *) ScPolicyHob, &gUsbConfigGuid, (VOID *) &UsbConfig);
  ASSERT_EFI_ERROR (Status);

  if (UsbConfig->Usb30Settings.Mode != XHCI_MODE_OFF) {
    //
    // Set xHCI MSI_MCTL According to OS:
    //  Windows: D3 Supported - Set to 1h to enable MSI
    //  Android: D0i3/DevIdle Supported (legacy interrupts used in D0i3 flows) - Set to 0h to disable MSI
    //
    if (MmioRead32((UINTN)(XhciPciMmBase)) != 0xFFFFFFFF) {
      if (SiPolicyHob->OsSelection == SiAndroid) {
        //
        // MSI capability structure must not be reported for Android to use line based interrupts.
        // Linux driver will use MSI if reported and wake flows require line based interrupts.
        //
        MmioAnd16(XhciPciMmBase + R_XHCI_MSI_MCTL, (UINT16)~(B_XHCI_MSI_MCTL_MSIENABLE));
      } else {
        MmioOr16(XhciPciMmBase + R_XHCI_MSI_MCTL, B_XHCI_MSI_MCTL_MSIENABLE);
      }
    } else {
      DEBUG ((EFI_D_ERROR, "xHCI not present, cannot disable the xHCI MSI capability structure for Android.\n"));
    }

    MmioOr32 (XhciPciMmBase + R_XHCI_XHCC2, B_XHCI_XHCC2_OCCFDONE);
    INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciPciMmBase + R_XHCI_XHCC2),
      1,
      (VOID *) (UINTN) (XhciPciMmBase + R_XHCI_XHCC2)
      );

    MmioOr32 (XhciPciMmBase + R_XHCI_XHCC1, B_XHCI_XHCC1_ACCTRL | B_XHCI_XHCC1_URD);
    INIT_COMMON_SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (XhciPciMmBase + R_XHCI_XHCC1),
      1,
      (VOID *) (UINTN) (XhciPciMmBase + R_XHCI_XHCC1)
      );
  }
}


/**
  Setup XHCI Over-Current Mapping

  @param[in] UsbConfig            The SC Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval    None

**/
VOID
XhciOverCurrentMapping (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINTN                       XhciPciMmBase
  )
{
  //
  //  BIOS responsibility on Overcurrent protection.
  //  ----------------------------------------------
  //  The max can be total 8 overcurrent pins
  //  OC0: Used for the OTG port (port 0)
  //  OC1: Used for the 2 host walk-up ports (ports 1 to 5)
  //  It is ok to map multiple ports to a single pin.
  //  It is not ok to map a single ports to a multiple pins.
  //  All USB ports routed out of the package must have Overcurrent protection.
  //  USB Ports not routed out from the package should not be assigned OC pins.
  //
  UINT32                   Index;
  UINT32                   U2OCMBuf[V_XHCI_NUMBER_OF_OC_PINS] = {0};
  UINT32                   U3OCMBuf[V_XHCI_NUMBER_OF_OC_PINS] = {0};
  UINT32                   OCPin;

  for (Index = 0; Index < GetScXhciMaxUsb3PortNum (); Index++) {
    if (UsbConfig->PortUsb30[Index].OverCurrentPin == ScUsbOverCurrentPinSkip) {
      //
      // No OC pin assigned, skip this port
      //
    } else {
      OCPin = UsbConfig->PortUsb30[Index].OverCurrentPin;
      ASSERT (OCPin < V_XHCI_NUMBER_OF_OC_PINS);
      U3OCMBuf[OCPin] |= (UINT32) (BIT0 << Index);
      }
    }
  for (Index = 0; Index < V_XHCI_NUMBER_OF_OC_PINS; Index++) {
    MmioWrite32 (XhciPciMmBase + R_XHCI_U3OCM + (Index * 4), U3OCMBuf[Index]);
  }

  for (Index = 0; Index < GetScXhciMaxUsb2PortNum (); Index++) {
    if (UsbConfig->PortUsb20[Index].OverCurrentPin == ScUsbOverCurrentPinSkip) {
      //
      // No OC pin assigned, skip this port
      //
    } else {
      OCPin = UsbConfig->PortUsb20[Index].OverCurrentPin;
      ASSERT (OCPin < V_XHCI_NUMBER_OF_OC_PINS);
      U2OCMBuf[OCPin] |= (UINT32) (BIT0 << Index);
      }
    }

  for (Index = 0; Index < V_XHCI_NUMBER_OF_OC_PINS; Index++) {
    MmioWrite32 (XhciPciMmBase + R_XHCI_U2OCM + (Index * 4), U2OCMBuf[Index]);
  }

}


/**
  Program Xhci Port Disable Override

  @param[in] UsbConfig            The SC Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciPortDisableOverride (
  IN  SC_USB_CONFIG  *UsbConfig,
  IN  UINTN          XhciMmioBase
  )
{
  UINT32          Index;
  UINT32          XhciUsb2Pdo = 0;
  UINT32          XhciUsb3Pdo = 0;
  BXT_SERIES      BxtSeries;

  BxtSeries = GetBxtSeries ();
  switch (BxtSeries) {
   case Bxt:
     XhciUsb2Pdo = MmioRead32 (XhciMmioBase + R_XHCI_USB2PDO) & B_XHCI_BXT_USB2PDO_MASK;
     XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_XHCI_USB3PDO) & B_XHCI_BXT_USB3PDO_MASK;
     break;
   case BxtP:
     XhciUsb2Pdo = MmioRead32 (XhciMmioBase + R_XHCI_USB2PDO) & B_XHCI_BXTP_USB2PDO_MASK;
     XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_XHCI_USB3PDO) & B_XHCI_BXTP_USB3PDO_MASK;
     break;
   default:
     break;
  }
  //
  // XHCI PDO for HS
  //
  for (Index = 0; Index < GetScXhciMaxUsb2PortNum(); Index++) {
    if (UsbConfig->PortUsb20[Index].Enable == FALSE) {
      XhciUsb2Pdo |= (UINT32) (B_XHCI_USB2PDO_DIS_PORT0 << Index);
    } else {
      XhciUsb2Pdo &= (UINT32)~(B_XHCI_USB2PDO_DIS_PORT0 << Index);
    }
  }
  //
  // XHCI PDO for SS
  //
  for (Index = 0; Index < GetScXhciMaxUsb3PortNum(); Index++) {
    if (UsbConfig->PortUsb30[Index].Enable == FALSE) {
      XhciUsb3Pdo |= (UINT32) (B_XHCI_USB3PDO_DIS_PORT0 << Index);
    } else {
      XhciUsb3Pdo &= (UINT32)~(B_XHCI_USB3PDO_DIS_PORT0 << Index);
    }
  }
  //
  // USB2PDO and USB3PDO are Write-Once registers and bits in them are in the SUS Well.
  //
  MmioWrite32 (XhciMmioBase + R_XHCI_USB2PDO, XhciUsb2Pdo);
  MmioWrite32 (XhciMmioBase + R_XHCI_USB3PDO, XhciUsb3Pdo);
}


/**
  Program and enable XHCI Memory Space

  @param[in] UsbConfig            The SC Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval    None

**/
VOID
XhciMemorySpaceOpen (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  //
  // Assign memory resources
  //
  MmioAnd16 (
    XhciPciMmBase + R_XHCI_COMMAND_REGISTER,
    (UINT16)~(B_XHCI_COMMAND_MSE | B_XHCI_COMMAND_BME)
    );
  MmioWrite32 (XhciPciMmBase + R_XHCI_MEM_BASE, XhciMmioBase);
  MmioOr16 (
    XhciPciMmBase + R_XHCI_COMMAND_REGISTER,
    (UINT16) (B_XHCI_COMMAND_MSE | B_XHCI_COMMAND_BME)
    );
}


/**
  Clear and disable XHCI Memory Space

  @param[in] UsbConfig            The SC Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval    None

**/
VOID
XhciMemorySpaceClose (
  IN  SC_USB_CONFIG               *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  //
  // Clear memory resources
  //
  MmioAnd16 (
    XhciPciMmBase + R_XHCI_COMMAND_REGISTER,
    (UINT16)~(B_XHCI_COMMAND_MSE | B_XHCI_COMMAND_BME)
    );

  MmioWrite32 ((XhciPciMmBase + R_XHCI_MEM_BASE), 0);

}

/**
  Initialization USB Clock Gating registers

  @retval    None

**/
VOID
ConfigureUsbClockGating (
  VOID
  )
{
  ConfigureXhciClockGating ();
}

