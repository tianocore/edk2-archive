# @file
#  SecCore constants and macros.
#
#  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED
#
#

#
# Set to 1 to enable debug
#
.equ NO_EVICTION_MODE_DEBUG,      1

.macro STATUS_CODE  status
.if NO_EVICTION_MODE_DEBUG
  movb  \status, %al
  outb  %al, $0x80
.endif
.endm

.equ FVHEADER_LEN_OFF,            0x30

.equ IMAGE_BASE_ADDRESS,          0xFFFF0000

#
# Commands defined in the AP SIPI code 
#
.equ AP_SIPI_COLLECT_MAX_RATIO,   0x01  
.equ AP_SIPI_PROGRAM_MAX_RATIO,   0x02
.equ AP_SIPI_SWITCH_BSP,          0x03
