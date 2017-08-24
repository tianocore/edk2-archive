/** @file
  Pei library for I2C bus driver.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "I2CDelayPei.h"
#include "I2CIoLibPei.h"
#include "I2CAccess.h"
#include <Library/I2CLib.h>
#include <PlatformBaseAddresses.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/HobLib.h>
#include <ScRegs/RegsPcu.h>
#include <ScRegs/RegsPmc.h>
#include <ScRegs/RegsLpss.h>
#include <ScRegs/RegsI2c.h>

EFI_GUID  mI2CPeiInitGuid = {
  0x96DED71A, 0xB9E7, 0x4EAD, {0x96, 0x2C, 0x01, 0x69, 0x3C, 0xED, 0x2A, 0x64}
};


#define LPSS_PCI_DEVICE_NUMBER  8


#define GPIO_PAD_CFG_DW0_GPIO_124_OFFSET  0x400  ///< This is the first gpio pin of I2C0

/*  Name    Function       Offset of GPIO WEST
  GPIO_124  LPSS_I2C0_SDA  0x400 + 16*0
  GPIO_125  LPSS_I2C0_SCL

  GPIO_126  LPSS_I2C1_SDA  0x400 + 16*1
  GPIO_127  LPSS_I2C1_SCL

  GPIO_128  LPSS_I2C2_SDA
  GPIO_129  LPSS_I2C2_SCL

  GPIO_130  LPSS_I2C3_SDA
  GPIO_131  LPSS_I2C3_SCL

  GPIO_132  LPSS_I2C4_SDA
  GPIO_133  LPSS_I2C4_SCL

  GPIO_134  LPSS_I2C5_SDA
  GPIO_135  LPSS_I2C5_SCL

  GPIO_136  LPSS_I2C6_SDA
  GPIO_137  LPSS_I2C6_SCL

  GPIO_138  LPSS_I2C7_SDA  0x400 + 16*7
  GPIO_139  LPSS_I2C7_SCL
*/

EFI_STATUS
EFIAPI
IntelI2CPeiLibConstructor (
  VOID
  )
{
  UINTN     Index;
//UINT32    Value;
  UINT32    gpio_pad_cfg_dw0_offset;

  // Program GPIO pins for I2C 0~7, need to set GPIO to Function 1 and misc settings.
  for (Index = 0; Index < 8; Index ++) {
   // Config I2C[Index] SDA pin DW0
    gpio_pad_cfg_dw0_offset = GPIO_PAD_CFG_DW0_GPIO_124_OFFSET + 16 * Index;
    {
      //Value = 0;  // FIXME: the right GPIO setting, need to get updated from GPIO config owner. Dummy code here.
      //SideBandCR32Write (SB_PORTID_GPIOW, gpio_pad_cfg_dw0_offset, Value);
    }

    // Config I2C[Index] SDA pin DW1
    gpio_pad_cfg_dw0_offset += 4;
    {
      //Value = 0;  // FIXME: the right GPIO setting, need to get updated from GPIO config owner. Dummy code here.
      //SideBandCR32Write (SB_PORTID_GPIOW, gpio_pad_cfg_dw0_offset, Value);
    }

    // Config I2C[Index] SCL pin DW0
    gpio_pad_cfg_dw0_offset += 4;
    {
      //Value = 0;  // FIXME: the right GPIO setting, need to get updated from GPIO config owner. Dummy code here.
      //SideBandCR32Write (SB_PORTID_GPIOW, gpio_pad_cfg_dw0_offset, Value);
    }

    // Config I2C[Index] SCL pin DW1
    gpio_pad_cfg_dw0_offset += 4;
    {
      //Value = 0;  // FIXME: the right GPIO setting, need to get updated from GPIO config owner. Dummy code here.
      //SideBandCR32Write (SB_PORTID_GPIOW, gpio_pad_cfg_dw0_offset, Value);
    }
  }

  return EFI_SUCCESS;
}

/**
  Program LPSS I2C PCI controller's BAR0 and enable memory decode.

  @retval EFI_SUCCESS           - I2C controller's BAR0 is programmed and memory decode enabled.
**/
EFI_STATUS
ProgramPciLpssI2C (
  VOID
  )
{
  UINT32       PmcBase;
  UINT32       DevID;
  UINTN        PciMmBase=0;
  UINTN        Index;
  UINTN        Bar0;
  UINTN        Bar1;

  DEBUG ((EFI_D_INFO, "Pei ProgramPciLpssI2C() Start\n"));

  //
  // Set the BXT Function Disable Register to ZERO
  //
  PmcBase = PMC_BASE_ADDRESS;

  if (I2CLibPeiMmioRead32 (PmcBase + R_PMC_FUNC_DIS) &
      (B_PMC_FUNC_DIS_LPSS_I2C0 | B_PMC_FUNC_DIS_LPSS_I2C1 | B_PMC_FUNC_DIS_LPSS_I2C2 | B_PMC_FUNC_DIS_LPSS_I2C3|
       B_PMC_FUNC_DIS_LPSS_I2C4 | B_PMC_FUNC_DIS_LPSS_I2C5 | B_PMC_FUNC_DIS_LPSS_I2C6 | B_PMC_FUNC_DIS_LPSS_I2C7)) {
    I2CLibPeiMmioWrite32 (
      PmcBase + R_PMC_FUNC_DIS,
      I2CLibPeiMmioRead32 (PmcBase + R_PMC_FUNC_DIS) &
            ~(B_PMC_FUNC_DIS_LPSS_I2C0| B_PMC_FUNC_DIS_LPSS_I2C1|B_PMC_FUNC_DIS_LPSS_I2C2| B_PMC_FUNC_DIS_LPSS_I2C3|
              B_PMC_FUNC_DIS_LPSS_I2C4| B_PMC_FUNC_DIS_LPSS_I2C5|B_PMC_FUNC_DIS_LPSS_I2C6| B_PMC_FUNC_DIS_LPSS_I2C7)
      );

    DEBUG ((EFI_D_INFO, "ProgramPciLpssI2C() enable all I2C controllers\n"));
  }

  for (Index = 0; Index < LPSS_PCI_DEVICE_NUMBER; Index ++) {
    if (Index < 4) {
      PciMmBase = MmPciAddress (
                    0,
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_LPSS_I2C0,
                    Index,
                    0
                    );
    } else {
      PciMmBase = MmPciAddress (
                    0,
                    DEFAULT_PCI_BUS_NUMBER_SC,
                    PCI_DEVICE_NUMBER_LPSS_I2C1,
                    (Index - 4),
                    0
                    );
    }

    DevID = I2CLibPeiMmioRead32 (PciMmBase);
    Bar0  = LPSS_I2C0_TMP_BAR0 + Index * LPSS_I2C_TMP_BAR0_DELTA;
    Bar1  = Bar0 + LPSS_I2C_TMP_BAR1_OFFSET;

    //
    // Check if device present
    //
    if (DevID != 0xFFFFFFFF) {
      if ((I2CLibPeiMmioRead32 (PciMmBase + R_LPSS_I2C_STSCMD) & B_LPSS_I2C_STSCMD_MSE)) {
        //
        // In PEI stage, we always disable Bus master, and memory space enabling for BAR re-programming
        // In DXE stage, will read existing BAR value instead of re-programming
        //
        I2CLibPeiMmioWrite32 ((UINTN) (PciMmBase + R_LPSS_I2C_STSCMD), 0);
      }
      //
      // Program BAR 0
      //
      I2CLibPeiMmioWrite32 ((UINTN) (PciMmBase + R_LPSS_I2C_BAR), (UINT32) (Bar0 & B_LPSS_I2C_BAR_BA));

      //
      // Program BAR 1
      //
      I2CLibPeiMmioWrite32 ((UINTN) (PciMmBase + R_LPSS_I2C_BAR1), (UINT32) (Bar1 & B_LPSS_I2C_BAR1_BA));

      //
      // Bus Master Enable & Memory Space Enable
      //
      I2CLibPeiMmioWrite32 ((UINTN) (PciMmBase + R_LPSS_I2C_STSCMD), (UINT32) (B_LPSS_I2C_STSCMD_BME | B_LPSS_I2C_STSCMD_MSE));
    }

    //
    // Release Resets
    //
    I2CLibPeiMmioWrite32 (Bar0 + R_LPSS_IO_MEM_RESETS, B_LPSS_IO_MEM_HC_RESET_REL | B_LPSS_IO_MEM_iDMA_RESET_REL);
  }

  DEBUG ((EFI_D_INFO, "Pei ProgramPciLpssI2C() End\n"));

  return EFI_SUCCESS;
}

/**
  Disable I2C host controller

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected

  @retval EFI_SUCCESS           - I2C host controller is completely inactive.
  @retval EFI_NOT_READY         - I2C host controller is still in an enabled state.
**/
EFI_STATUS
I2cDisable (
  IN UINT8 BusNo
  )
{
  UINTN    I2CBaseAddress;
  UINT32   NumTries = 10000;  // 0.1 seconds

  I2CBaseAddress = (UINT32) LPSS_I2C0_TMP_BAR0 + (BusNo * LPSS_I2C_TMP_BAR0_DELTA);
  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_ENABLE, 0);
  while (0 != (I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_ENABLE) & 1)) {
    MicroSecondDelay (10);
    NumTries--;
    if (NumTries == 0) return EFI_NOT_READY;
  }
  return EFI_SUCCESS;
}


/**
  Enable I2C host controller

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected

  @retval EFI_SUCCESS           - I2C host controller is in an enabled state.
  @retval EFI_NOT_READY         - I2C host controller is still inactive.
**/
EFI_STATUS
I2cEnable (
  IN UINT8    BusNo
  )
{
  UINTN    I2CBaseAddress;
  UINT32   NumTries = 10000;  //0.1 seconds

  I2CBaseAddress = (UINT32) LPSS_I2C0_TMP_BAR0 + (BusNo * LPSS_I2C_TMP_BAR0_DELTA);
  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_ENABLE, 1);
  while (0 == (I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_ENABLE ) & 1)) {
    MicroSecondDelay (10);
    NumTries --;
    if (NumTries == 0) return EFI_NOT_READY;
  }
  return EFI_SUCCESS;
}


/**
  Set the I2C controller bus clock frequency.

  The software and controller do a best case effort of using the specified
  frequency for the I2C bus.  If the frequency does not match exactly then
  the controller will use a slightly lower frequency for the I2C to avoid
  exceeding the operating conditions for any of the I2C devices on the bus.
  For example if 400 KHz was specified and the controller's divide network
  only supports 402 KHz or 398 KHz then the controller would be set to 398
  KHz.  However if the desired frequency is 400 KHz and the controller only
  supports 1 MHz and 100 KHz then this routine would return EFI_UNSUPPORTED.

  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller
  @param[in]  BusClockHertz     - New I2C bus clock frequency in Hertz
  @param[out] I2cMode           - I2C operation mode.
                                  Standard Speed: 100 KHz
                                  Fast Speed    : 400 KHz
                                  High Speed    : 3.4 MHz

  @retval EFI_SUCCESS           - The bus frequency was set successfully.
**/

EFI_STATUS
I2cBusFrequencySet (
  IN  UINTN     I2CBaseAddress,
  IN  UINTN     BusClockHertz,
  OUT UINT16    *I2cMode,
  IN  BOOLEAN   DebugFlag
  )
{
  if (DebugFlag) DEBUG ((EFI_D_INFO, "InputFreq BusClockHertz: %d\r\n", BusClockHertz));

  *I2cMode = B_IC_RESTART_EN | B_IC_SLAVE_DISABLE | B_MASTER_MODE;

  //
  //  Set the 100 KHz clock divider
  //  From Table 10 of the I2C specification
  //    High: 4.00 uS
  //    Low:  4.70 uS
  //
  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_SS_SCL_HCNT, (UINT16)0x214);
  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_SS_SCL_LCNT, (UINT16)0x272);

  //
  // Set the 400 KHz clock divider
  // From Table 10 of the I2C specification
  //   High: 0.60 uS
  //   Low:  1.30 uS
  //
  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_FS_SCL_HCNT, (UINT16)0x50);
  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_FS_SCL_LCNT, (UINT16)0xAD);

  switch (BusClockHertz) {
    case 100 * 1000:
      I2CLibPeiMmioWrite32 (I2CBaseAddress + R_IC_SDA_HOLD, (UINT16)0x40);//100K
      *I2cMode |= V_SPEED_STANDARD;
      break;
    case 400 * 1000:
      I2CLibPeiMmioWrite32 (I2CBaseAddress + R_IC_SDA_HOLD, (UINT16)0x32);//400K
      *I2cMode |= V_SPEED_FAST;
      break;
    default:
      I2CLibPeiMmioWrite32 (I2CBaseAddress + R_IC_SDA_HOLD, (UINT16)0x09);//3.4M
      *I2cMode |= V_SPEED_HIGH;
  }

  return EFI_SUCCESS;
}

/**
  Initializes the host controller to execute I2C commands.

  @param[in]  BusNo               - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        - Slave address of the I2C device

  @retval  EFI_SUCCESS            - Opcode initialization on the I2C host controller completed.
  @retval  EFI_INVALID_PARAMETER  - Invalid slave address
  @retval  EFI_DEVICE_ERROR       - Operation failed, device error
  @retval  Others                 - Failed to initialize I2C host controller
**/
EFI_STATUS
I2CInit (
  IN UINT8     BusNo,
  IN UINT16    SlaveAddress
  )
{
  EFI_STATUS    Status;
  UINT32        NumTries = 0;
  UINTN         I2CBaseAddress;
  UINT16        I2cMode;
  UINTN         PciMmBase = 0;
  BOOLEAN       DebugFlag = TRUE;

  if (BusNo < 4) {
    PciMmBase = MmPciAddress (
                  0,
                  DEFAULT_PCI_BUS_NUMBER_SC,
                  PCI_DEVICE_NUMBER_LPSS_I2C0,
                  BusNo,
                  0
                  );
  } else {
    PciMmBase = MmPciAddress (
                  0,
                  DEFAULT_PCI_BUS_NUMBER_SC,
                  PCI_DEVICE_NUMBER_LPSS_I2C1,
                  (BusNo - 4),
                  0
                  );
  }

  I2CBaseAddress = (I2CLibPeiMmioRead32 (PciMmBase + R_LPSS_I2C_BAR) & 0xFFFFFFF8);

  //
  //  Verify the parameters
  //
  if (1023 < SlaveAddress) {
    Status =  EFI_INVALID_PARAMETER;
    if (DebugFlag) DEBUG ((DEBUG_ERROR, "I2cStartRequest Exit with Status %r\r\n", Status));
    return Status;
  }

  if (I2CBaseAddress == (LPSS_I2C0_TMP_BAR0 + (BusNo * LPSS_I2C_TMP_BAR0_DELTA))) {
    DebugFlag = FALSE;
  } else {
    //
    // Need to enable the I2C PCI device
    //
    ProgramPciLpssI2C ();

    I2CBaseAddress = (UINT32) (LPSS_I2C0_TMP_BAR0 + (BusNo * LPSS_I2C_TMP_BAR0_DELTA));
    if (DebugFlag) DEBUG ((DEBUG_INFO, "I2CBaseAddress = 0x%x \n", I2CBaseAddress));
  }

  NumTries = 10000; // 1 seconds 
  while ((1 == (I2CLibPeiMmioRead32 (I2CBaseAddress + R_IC_STATUS) & STAT_MST_ACTIVITY ))) {
    MicroSecondDelay (10);
    NumTries --;
    if (0 == NumTries)
      return EFI_DEVICE_ERROR;
  }

  Status = I2cDisable (BusNo);
  if (DebugFlag) DEBUG ((DEBUG_INFO, "I2cDisable Status = %r\r\n", Status));

  I2cBusFrequencySet (I2CBaseAddress, 400 * 1000, &I2cMode, DebugFlag); // Set I2cMode

  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_INTR_MASK, 0x0);
  if (0x7F < SlaveAddress) {
    SlaveAddress = (SlaveAddress & 0x3FF) | IC_TAR_10BITADDR_MASTER;
  }

  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_TAR,   (UINT16) SlaveAddress);
  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_RX_TL, 0);
  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_TX_TL, 0);
  I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_CON,   I2cMode);

  Status = I2cEnable (BusNo);
  if (DebugFlag) DEBUG ((DEBUG_INFO, "I2cEnable Status = %r\r\n", Status));

  I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_CLR_TX_ABRT);
  return EFI_SUCCESS;
}

/**
  Read bytes from I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress      - Slave address of the I2C device (7-bit)
  @param[in]  ReadBytes         - Number of bytes to be read
  @param[out] ReadBuffer        - Address to which the value read has to be stored
  @param[in]  Start             - It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End               - It controls whether a STOP is issued after the byte is sent or received.

  @retval EFI_SUCCESS           - The byte value read successfully
  @retval EFI_DEVICE_ERROR      - Operation failed
  @retval EFI_TIMEOUT           - Hardware retry timeout
  @retval Others                - Failed to read a byte via I2C
**/
EFI_STATUS
ByteReadI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  )
{
  EFI_STATUS    Status;
  UINT32        I2cStatus;
  UINT16        ReceiveData;
  UINT8         *ReceiveDataEnd;
  UINT8         *ReceiveRequest;
  UINT16        raw_intr_stat;
  UINTN         I2CBaseAddress;

  I2CBaseAddress = (UINT32) (LPSS_I2C0_TMP_BAR0 + (BusNo * LPSS_I2C_TMP_BAR0_DELTA));

  Status = EFI_SUCCESS;

  I2CInit (BusNo, SlaveAddress);

  ReceiveDataEnd = &ReadBuffer [ReadBytes];
  if (ReadBytes) {
    ReceiveRequest = ReadBuffer;

    while ((ReceiveDataEnd > ReceiveRequest) ||
           (ReceiveDataEnd > ReadBuffer)) {
      //
      // Check for NACK
      //
      raw_intr_stat = I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_RAW_INTR_STAT);
      if (0 != (raw_intr_stat & I2C_INTR_TX_ABRT)) {
        I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_CLR_TX_ABRT);
        Status = RETURN_DEVICE_ERROR;
        DEBUG ((DEBUG_INFO,"TX ABRT ,%d bytes hasn't been transferred\r\n", ReceiveDataEnd - ReceiveRequest));
        break;
      }

      //
      // Determine if another byte was received
      //
      I2cStatus = I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_STATUS);
      if (0 != (I2cStatus & STAT_RFNE)) {
        ReceiveData = I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_DATA_CMD);
        *ReadBuffer++ = (UINT8)ReceiveData;
      }

      if (ReceiveDataEnd == ReceiveRequest) {
        continue; //Waiting the last request to get data and make (ReceiveDataEnd > ReadBuffer) =TRUE.
      }

      //
      // Wait until a read request will fit
      //
      if (0 == (I2cStatus & STAT_TFNF)) {
        MicroSecondDelay (10);
        continue;
      }

      //
      // Issue the next read request
      //
      if (End && Start) {
        I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_DATA_CMD, B_READ_CMD | B_CMD_RESTART | B_CMD_STOP);
      } else if (!End && Start) {
        I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_DATA_CMD, B_READ_CMD | B_CMD_RESTART);
      } else if (End && !Start) {
        I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_DATA_CMD, B_READ_CMD | B_CMD_STOP);
      } else if (!End && !Start) {
        I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_DATA_CMD, B_READ_CMD);
      }
      ReceiveRequest += 1;
    }
  }
  return Status;

}

/**
  Write bytes to I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress      - Slave address of the I2C device (7-bit)
  @param[in]  WriteBytes        - Number of bytes to be written
  @param[in]  WriteBuffer       - Address to which the byte value has to be written
  @param[in]  Start             - It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End               - It controls whether a STOP is issued after the byte is sent or received.

  @retval EFI_SUCCESS           - The byte value written successfully
  @retval EFI_DEVICE_ERROR      - Operation failed
  @retval EFI_TIMEOUT           - Hardware retry timeout
  @retval Others                - Failed to write a byte via I2C
**/
EFI_STATUS
ByteWriteI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  )
{
  UINT16        Data16;
  EFI_STATUS    Status;
  UINT32        I2cStatus;
  UINT8         *TransmitPtr;
  UINT8         *TransmitEnd;
  UINT16        raw_intr_stat;
  UINTN         I2CBaseAddress;


  I2CBaseAddress = (UINT32)LPSS_I2C0_TMP_BAR0 + (BusNo * LPSS_I2C_TMP_BAR0_DELTA);
  Status = EFI_SUCCESS;
  I2CInit (BusNo, SlaveAddress);

  TransmitPtr = WriteBuffer;
  TransmitEnd = &WriteBuffer[WriteBytes];
  if (WriteBytes > 0x00) {


    while (TransmitEnd > TransmitPtr) {
      I2cStatus = I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_STATUS);
      raw_intr_stat = I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_RAW_INTR_STAT);
      if (0 != (raw_intr_stat & I2C_INTR_TX_ABRT)) {
        I2CLibPeiMmioRead16 (I2CBaseAddress + R_IC_CLR_TX_ABRT);
        Status = RETURN_DEVICE_ERROR;
        DEBUG ((DEBUG_ERROR,"TX ABRT TransmitEnd:0x%x WritePtr:0x%x\r\n", TransmitEnd, TransmitPtr));
        break;
      }
      if (0 == (I2cStatus & STAT_TFNF)) {
        DEBUG ((DEBUG_INFO,"%a(#%d) - 0 == (I2cStatus & STAT_TFNF)\n", __FUNCTION__, __LINE__));
        continue;
      }

      Data16 = (UINT16) *TransmitPtr;
      if (End && Start) {
        Data16 |= (B_CMD_RESTART | B_CMD_STOP);
      } else if (!End && Start) {
        Data16 |= B_CMD_RESTART;
      } else if (End && !Start) {
        Data16 |= B_CMD_STOP;
      }
      Data16 = I2CLibPeiMmioWrite16 (I2CBaseAddress + R_IC_DATA_CMD, Data16);
      TransmitPtr++;

      //
      // Add a small delay to work around some odd behavior being seen.  Without this delay bytes get dropped.
      //
      MicroSecondDelay (FIFO_WRITE_DELAY);
    }

  }

  if (EFI_ERROR (Status))
    DEBUG ((EFI_D_INFO,"I2cStartRequest Exit with Status %r\r\n", Status));

  return Status;
}


/**
  Read bytes from I2C Device

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress      - Slave address of the I2C device (7-bit)
  @param[in]  Offset            - Register offset from which the data has to be read
  @param[in]  ReadBytes         - Number of bytes to be read
  @param[out] ReadBuffer        - Address to which the value read has to be stored

  @retval EFI_SUCCESS           - Read bytes from I2C device successfully
  @retval Others                - Return status depends on ByteReadI2C_Basic
**/
EFI_STATUS
ByteReadI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer
  )
{
  EFI_STATUS    Status;

  Status = ByteWriteI2C_Basic (BusNo, SlaveAddress, 1, &Offset, TRUE, FALSE);
  if (EFI_ERROR (Status)) return Status;
  Status = ByteReadI2C_Basic (BusNo, SlaveAddress, ReadBytes, ReadBuffer, TRUE, TRUE);

  return Status;
}

/**
  Write bytes to I2C Device

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress      - Slave address of the I2C device (7-bit)
  @param[in]  Offset            - Register offset from which the data has to be read
  @param[in]  WriteBytes        - Number of bytes to be written
  @param[in]  WriteBuffer       - Address to which the byte value has to be written

  @retval EFI_SUCCESS           - Write bytes to I2C device successfully
  @retval Others                - Return status depends on ByteWriteI2C_Basic
**/
EFI_STATUS
ByteWriteI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer
  )
{
  EFI_STATUS        Status;

  Status = ByteWriteI2C_Basic (BusNo, SlaveAddress, 1, &Offset, TRUE, FALSE);
  if (EFI_ERROR (Status)) return Status;
  Status = ByteWriteI2C_Basic (BusNo, SlaveAddress, WriteBytes, WriteBuffer, FALSE, TRUE);

  return Status;
}
