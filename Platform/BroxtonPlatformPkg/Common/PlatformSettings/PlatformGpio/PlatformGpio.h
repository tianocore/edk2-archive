/** @file
  Internal header file for GPIO protocol.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <Protocol/GpioProtocol.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/GpioLib.h>


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
  );

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
  );

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
  );

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
  );

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
  IN EDKII_GPIO_PROTOCOL                *This,
  IN UINT32                             PinNum,
  IN GPIO_DIRECTION                     GpioDirection
  );

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
  );

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
  );

