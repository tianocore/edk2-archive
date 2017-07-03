/** @file
  Implementation for GPIO protocol.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformGpio.h"
#include "GpioPadTable.h"

EFI_HANDLE                    mHandle = NULL;

EDKII_GPIO_PROTOCOL mGpioProtocol = {
  GetMaxCount,
  GetMode,
  SetMode,
  GetGpioDirection,
  SetGpioDirection,
  GetGpiLevel,
  SetGpoLevel
  };


/**
  Get Max GPIO count.

  @param[in]   This                  A pointer to the EDKII_GPIO_PROTOCOL instance.
  @param[out]  MaxCount              Max GPIO count

  @retval      EFI_SUCCESS           The operation succeeded.
**/
EFI_STATUS
EFIAPI
GetMaxCount (
  IN EDKII_GPIO_PROTOCOL                *This,
  OUT UINT32                            *MaxCount
  )
{
  UINT8 Index;
  *MaxCount = sizeof (mGpioPadTable)/sizeof (mGpioPadTable[0]);
  DEBUG ((DEBUG_INFO, "Max PinNum is %d\n", *MaxCount));

  for( Index =0; Index < *MaxCount; Index++ ) {
    DEBUG ((DEBUG_INFO, "PinNum: %d,  PadName: %s\n", Index, mGpioPadTable[Index].pad_name));
  }

  return EFI_SUCCESS;
}


/**
  Get Pad Mode.

  @param[in]   This                  A pointer to the EDKII_GPIO_PROTOCOL instance.
  @param[in]   PinNum                Target GPIO.
  @param[out]  PadMode               0: Function 0 (GPIO mode),
                                     1: Function 1, 2: Function 2, 3: Function 3, 4: Function 4, 5: Function 5

  @retval      EFI_SUCCESS           The operation succeeded.
  @retval      EFI_UNSUPPORTED       This function is not supported.
**/
EFI_STATUS
EFIAPI
GetMode (
  IN EDKII_GPIO_PROTOCOL                *This,
  IN UINT32                             PinNum,
  OUT PAD_MODE                          *PadMode
  )
{
  BXT_CONF_PAD0  PadConfig0;

  if (PinNum>=sizeof (mGpioPadTable)/sizeof (mGpioPadTable[0])) {
    DEBUG ((DEBUG_INFO, "PinNum out of range.\n"));
    return EFI_UNSUPPORTED;
  }

  PadConfig0.padCnf0 = GpioPadRead (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  *PadMode = PadConfig0.r.PMode;
  DEBUG ((DEBUG_INFO, "Pad Mode is %x\n", *PadMode));
  return EFI_SUCCESS;
}


/**
  Set Pad Mode to Fn0/Fn1/Fn2/Fn3/Fn4/Fn5.

  @param[in]   This                  A pointer to the EDKII_GPIO_PROTOCOL instance.
  @param[in]   GPIO_NAME             Target GPIO.
  @param[out]  PMode                 GPIO mode to set.

  @retval      EFI_SUCCESS           The operation succeeded.
  @retval      EFI_UNSUPPORTED       This function is not supported.
**/
EFI_STATUS
EFIAPI
SetMode (
  IN EDKII_GPIO_PROTOCOL                *This,
  IN UINT32                             PinNum,
  IN PAD_MODE                           PadMode
  )
{
  BXT_CONF_PAD0  PadConfig0;
  
  if (PinNum>=sizeof (mGpioPadTable)/sizeof (mGpioPadTable[0])) {
    DEBUG ((DEBUG_INFO, "PinNum out of range.\n"));
    return EFI_UNSUPPORTED;
  }

  PadConfig0.padCnf0 = GpioPadRead (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfig0.r.PMode = PadMode;
  GpioPadWrite (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, PadConfig0.padCnf0);

  DEBUG ((DEBUG_INFO, "Set GPIO Mode to %x\n", PadMode));
  return EFI_SUCCESS;
}


/**
  Check GPIO direction.

  @param[in]   This                  A pointer to the EDKII_GPIO_PROTOCOL instance.
  @param[in]   PinNum                Target GPIO.
  @param[out]  GpioDirection

  @retval      EFI_SUCCESS           The operation succeeded.
  @retval      EFI_UNSUPPORTED       This function is not supported.
**/
EFI_STATUS
EFIAPI
GetGpioDirection (
  IN EDKII_GPIO_PROTOCOL                *This,
  IN UINT32                             PinNum,
  OUT GPIO_DIRECTION                    *GpioDirection
  )
{
  BXT_CONF_PAD0  PadConfig0;

  if (PinNum>=sizeof (mGpioPadTable)/sizeof (mGpioPadTable[0])) {
    DEBUG ((DEBUG_INFO, "PinNum out of range.\n"));
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "CommAndOffset = 0x%08x, PadName = %s\n",mGpioPadTable[PinNum].CommAndOffset,mGpioPadTable[PinNum].pad_name));
  //
  // Check pad mode
  //
  PadConfig0.padCnf0 = GpioPadRead (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);

  if (PadConfig0.r.PMode != M0) {
    DEBUG ((DEBUG_INFO, "Not GPIO mode\n"));
    return EFI_UNSUPPORTED;
  } else {
    DEBUG ((DEBUG_INFO, "GPIO mode."));
    *GpioDirection = PadConfig0.r.GPIORxTxDis;
    if (*GpioDirection == GpIn) {
      DEBUG ((DEBUG_INFO, "Pad direction is GPI\n"));
    } else if (*GpioDirection == GpOut) {
      DEBUG ((DEBUG_INFO, "Pad direction is GPO\n"));
    } else if (*GpioDirection == GpInOut) {
      DEBUG ((DEBUG_INFO, "Pad direction is GPIO\n"));
    } else {
      return EFI_UNSUPPORTED;
    }
  }
  return EFI_SUCCESS;
}


/**
  Set GPIO direction to GPI/GPO/GPIO.

  @param[in]   This                  A pointer to the EDKII_GPIO_PROTOCOL instance.
  @param[in]   PinNum                Target GPIO.
  @Param[in]   GpioDirection

  @retval      EFI_SUCCESS           The operation succeeded.
  @retval      EFI_UNSUPPORTED       This function is not supported.
**/
EFI_STATUS
EFIAPI
SetGpioDirection (
  IN EDKII_GPIO_PROTOCOL              *This,
  IN UINT32                           PinNum,
  IN GPIO_DIRECTION                   GpioDirection
  )
{

  BXT_CONF_PAD0  PadConfig0;

  if (PinNum>=sizeof (mGpioPadTable)/sizeof (mGpioPadTable[0])) {
    DEBUG ((DEBUG_INFO, "PinNum out of range.\n"));
    return EFI_UNSUPPORTED;
  }

  PadConfig0.padCnf0 = GpioPadRead (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  //
  // Check pad mode
  //
  if (PadConfig0.r.PMode != M0) {
    DEBUG ((DEBUG_INFO, "Not GPIO mode\n"));
    return EFI_UNSUPPORTED;
  }

  PadConfig0.r.GPIORxTxDis = GpioDirection;   //Set to GPI/GPO/GPIO
  GpioPadWrite (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, PadConfig0.padCnf0);

  PadConfig0.padCnf0 = GpioPadRead (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  DEBUG ((DEBUG_INFO, "PAD_CONF0: 0x%08x\n", PadConfig0.padCnf0));

  return EFI_SUCCESS;
}


/**
  Check GPIO direction, if it is GPI, get input value.

  @param[in]   This                  A pointer to the EDKII_GPIO_PROTOCOL instance.
  @param[in]   PinNum                Target GPIO.
  @param[out]  InputVal              GPIO Input value
                                     0: InputLow, 1: InputHigh

  @retval      EFI_SUCCESS           The operation succeeded.
  @retval      EFI_UNSUPPORTED       This function is not supported.
**/
EFI_STATUS
EFIAPI
GetGpiLevel (
  IN EDKII_GPIO_PROTOCOL                *This,
  IN UINT32                             PinNum,
  OUT GPIO_LEVEL                        *GpiLevel
  )
{
  BXT_CONF_PAD0  PadConfig0;

  if (PinNum>=sizeof (mGpioPadTable)/sizeof (mGpioPadTable[0])) {
    DEBUG ((DEBUG_INFO, "PinNum out of range.\n"));
    return EFI_UNSUPPORTED;
  }

  PadConfig0.padCnf0 = GpioPadRead (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  //
  // Check pad mode
  //
  if (PadConfig0.r.PMode != M0) {
    DEBUG ((DEBUG_INFO, "Not GPIO mode\n"));
    return EFI_UNSUPPORTED;
  }

  if (PadConfig0.r.GPIORxTxDis == GPI) {
    *GpiLevel = (PadConfig0.padCnf0 & 0x02) >> 1;
    DEBUG ((DEBUG_INFO, "Pad direction is GPI, value is %x\n", *GpiLevel));
  } else {
    DEBUG ((DEBUG_INFO, "Not GPI. Return.\n"));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}


/**
  Check GPIO direction, if it is GPO, Set output value.

  @param[in]   This                  A pointer to the EDKII_GPIO_PROTOCOL instance.
  @param[in]   PinNum                Target GPIO.
  @param[in]   GpoState              GPO output value
                                     0: Low, 1: High

  @retval      EFI_SUCCESS           The operation succeeded.
  @retval      EFI_UNSUPPORTED       This function is not supported.
**/
EFI_STATUS
EFIAPI
SetGpoLevel (
  IN EDKII_GPIO_PROTOCOL               *This,
  IN UINT32                            PinNum,
  IN GPIO_LEVEL                        GpoLevel
  )
{
  BXT_CONF_PAD0  PadConfig0;

  if (PinNum>=sizeof (mGpioPadTable)/sizeof (mGpioPadTable[0])) {
    DEBUG ((DEBUG_INFO, "PinNum out of range.\n"));
    return EFI_UNSUPPORTED;
  }

  PadConfig0.padCnf0 = GpioPadRead (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  //
  // Check pad mode
  //
  if (PadConfig0.r.PMode != M0) {
    DEBUG ((DEBUG_INFO, "Not GPIO mode\n"));
    return EFI_UNSUPPORTED;
  }
  if ( PadConfig0.r.GPIORxTxDis == GPO ) {
    GpioSetOutputValue (mGpioPadTable[PinNum].CommAndOffset, GpoLevel);
  } else {
    DEBUG ((DEBUG_INFO, "Not GPO. Return.\n"));
    return EFI_UNSUPPORTED;
  }

  PadConfig0.padCnf0 = GpioPadRead (mGpioPadTable[PinNum].CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  DEBUG ((DEBUG_INFO, "PAD_CONF0: 0x%08x\n", PadConfig0.padCnf0));
  return EFI_SUCCESS;
}


/**
  Entry point of GPIO protocol.

  @param[in]  ImageHandle             Handle for this drivers loaded image protocol.
  @param[in]  SystemTable             EFI system table.

  @retval     EFI_SUCCESS             Succesfully installed GpioProtocol driver.
  @retval     other                   Errors occured.

**/
EFI_STATUS
EFIAPI
PlatformGpioConfigEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  return  gBS->InstallProtocolInterface (
                 &mHandle,
                 &gEdkiiGpioProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &mGpioProtocol
                 );
}

