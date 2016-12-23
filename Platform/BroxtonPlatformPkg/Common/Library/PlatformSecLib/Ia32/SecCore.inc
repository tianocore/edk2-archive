;; @file
;  SecCore constants and macros.
;
;  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>
;
;  This program and the accompanying materials
;  are licensed and made available under the terms and conditions of the BSD License
;  which accompanies this distribution.  The full text of the license may be found at
;  http://opensource.org/licenses/bsd-license.php.
;
;  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
;  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED
;
;;

;
; Set to 1 to enable debug
;
NO_EVICTION_MODE_DEBUG  EQU     1

STATUS_CODE         MACRO status
IF NO_EVICTION_MODE_DEBUG
                    mov  al, status
                    out  080h, al
ENDIF
ENDM

FVHEADER_LEN_OFF    EQU 30h

IMAGE_BASE_ADDRESS  EQU 0FFFF0000h

;
; Commands defined in the AP SIPI code 
;
AP_SIPI_COLLECT_MAX_RATIO   EQU   001h  
AP_SIPI_PROGRAM_MAX_RATIO   EQU   002h
AP_SIPI_SWITCH_BSP          EQU   003h
