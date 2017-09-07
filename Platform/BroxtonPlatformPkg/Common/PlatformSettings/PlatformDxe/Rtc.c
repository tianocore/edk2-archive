/** @file
  Adjust Default System Time.
  
  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
                                                                                   
  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License that accompanies this distribution.  
  The full text of the license may be found at                                     
  http://opensource.org/licenses/bsd-license.php.                                  
                                                                                   
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,            
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.    
                                                                                   
--*/

#include <PlatformDxe.h>

//
// Date and time initial values.
// They are used if the RTC values are invalid during driver initialization
//
#define RTC_INIT_SECOND 0
#define RTC_INIT_MINUTE 0
#define RTC_INIT_HOUR   0

#define RTC_ADDRESS_CENTURY      50

#define RTC_ADDRESS_REGISTER     0x70
#define RTC_DATA_REGISTER        0x71

/**
  Set RTC century to 20 in case RTC core driver failed to initialize it.

**/
VOID
EFIAPI
AdjustRtcCentury (
  VOID
  )
{

  UINT8  Century;

  Century = 20;
  Century = DecimalToBcd8 (20);
  IoWrite8 (RTC_ADDRESS_REGISTER, (UINT8) (RTC_ADDRESS_CENTURY | (UINT8) (IoRead8 (PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  IoWrite8 (RTC_DATA_REGISTER, Century);
  Century = IoRead8(RTC_DATA_REGISTER);

  return;
}
