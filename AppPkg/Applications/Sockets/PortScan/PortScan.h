/** @file
  Definitions for the port scan application

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PORT_SCAN_H_
#define _PORT_SCAN_H_

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include "OsTypes.h"
#include "OsTimer.h"
#include <stdio.h>

//------------------------------------------------------------------------------
//  Constants
//------------------------------------------------------------------------------

#define PORT_RESPONSE_MSEC  100   //  Milliseconds to wait for a response
#define RX_TIMEOUT_MSEC     100   //  Milliseconds to wait for a receive packet

//------------------------------------------------------------------------------
//  API
//------------------------------------------------------------------------------

/**
  Determine which TCP ports are open on a remote system

  @param [in] ArgC      Argument count
  @param [in] ArgV      Argument value array

  @retval 0             Successfully operation
 **/

int
PortScan (
  IN int ArgC,
  IN char **ArgV
  );

//------------------------------------------------------------------------------

#endif  //  _PORT_SCAN_H_
