/** @file
  This protocol is used to configure GPIO.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


**/

///
/// GPIO Protocol GUID.
///
#define EDKII_GPIO_PROTOCOL_GUID \
  { \
    0x239a4037, 0x5231, 0x44d6, {0xa2, 0xab, 0x51, 0x74, 0xcd, 0x81, 0xff, 0x85 } \
  }

typedef struct _EDKII_GPIO_PROTOCOL EDKII_GPIO_PROTOCOL;



typedef enum {
  GpInOut   = 0,
  GpIn      = 1,    ///< GPI, input only in PAD_VALUE
  GpOut     = 2     ///< GPO, output only in PAD_VALUE
} GPIO_DIRECTION;

typedef enum {
  Low  = 0,
  High = 1
} GPIO_LEVEL;

typedef enum {
  Fn0 = 0,         ///< Function 0, GPIO mode
  Fn1 = 1,         ///< Function 1
  Fn2 = 2,         ///< Function 2
  Fn3 = 3,         ///< Function 3
  Fn4 = 4,         ///< Function 4
  Fn5 = 5          ///< Function 5
} PAD_MODE;

/**
  Get Max GPIO count.

  @param[in]   This                  A pointer to the EDKII_GPIO_PROTOCOL instance.
  @param[out]  MaxCount              Max GPIO count

  @retval      EFI_SUCCESS           The operation succeeded.
**/
typedef
EFI_STATUS
(EFIAPI *EDKII_GET_MAX_COUNT)(
  IN EDKII_GPIO_PROTOCOL                 *This,
  OUT UINT32                             *MaxCount
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
typedef
EFI_STATUS
(EFIAPI *EDKII_GET_MODE) (
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
typedef
EFI_STATUS
(EFIAPI *EDKII_SET_MODE) (
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
typedef
EFI_STATUS
(EFIAPI *EDKII_GET_GPIO_DIRECTION)(
  IN EDKII_GPIO_PROTOCOL                 *This,
  IN UINT32                              PinNum,
  OUT GPIO_DIRECTION                     *GpioDirection
  );


/**
  Set GPIO direction to GPI/GPO/GPIO.

  @param[in]   This                  A pointer to the EDKII_GPIO_PROTOCOL instance.
  @param[in]   PinNum                Target GPIO.
  @Param[in]   GpioDirection

  @retval      EFI_SUCCESS           The operation succeeded.
  @retval      EFI_UNSUPPORTED       This function is not supported.
**/
typedef
EFI_STATUS
(EFIAPI *EDKII_SET_GPIO_DIRECTION)(
  IN EDKII_GPIO_PROTOCOL                 *This,
  IN UINT32                              PinNum,
  IN GPIO_DIRECTION                      GpioDirection
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
typedef
EFI_STATUS
(EFIAPI *EDKII_GPIO_GET_GPI_LEVEL)(
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
typedef
EFI_STATUS
(EFIAPI *EDKII_GPIO_SET_GPO_LEVEL)(
  IN EDKII_GPIO_PROTOCOL                *This,
  IN UINT32                             PinNum,
  IN GPIO_LEVEL                         GpoLevel
  );


struct _EDKII_GPIO_PROTOCOL {
  EDKII_GET_MAX_COUNT            GetMaxCount;
  EDKII_GET_MODE                 GetMode;
  EDKII_SET_MODE                 SetMode;
  EDKII_GET_GPIO_DIRECTION       GetGpioDirection;       ///< Get GPIO direction
  EDKII_SET_GPIO_DIRECTION       SetGpioDirection;       ///< Set GPIO direction
  EDKII_GPIO_GET_GPI_LEVEL       GetGpiLevel;            ///< Get GPI level
  EDKII_GPIO_SET_GPO_LEVEL       SetGpoLevel;            ///< Set GPO level
};

extern EFI_GUID gEdkiiGpioProtocolGuid;

