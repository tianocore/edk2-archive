;; @file
;  Equates for MP initialization.
;
;  Copyright (c) 2005 - 2016, Intel Corporation. All rights reserved.<BR>
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

VacantFlag                    Equ   00h
NotVacantFlag                 Equ   0ffh
BreakToRunApSignal            Equ   6E755200h

MonitorFilterSize             Equ   40h
WakeUpApCounterInit           Equ   0
WakeUpApPerHltLoop            Equ   1
WakeUpApPerMwaitLoop          Equ   2
WakeUpApPerRunLoop            Equ   3
WakeUpApPerMwaitLoop32        Equ   4
WakeUpApPerRunLoop32          Equ   5

LockLocation                  equ        1000h - 0400h
StackStartAddressLocation     equ        LockLocation + 08h
StackSizeLocation             equ        LockLocation + 10h
CProcedureLocation            equ        LockLocation + 18h
GdtrLocation                  equ        LockLocation + 20h
IdtrLocation                  equ        LockLocation + 2Ah
BufferStartLocation           equ        LockLocation + 34h
Cr3OffsetLocation             equ        LockLocation + 38h
InitFlagLocation              equ        LockLocation + 3Ch
WakeUpApManner                equ        LockLocation + 40h
BistBuffer                    equ        LockLocation + 44h

PAUSE32   MACRO
            DB      0F3h
            DB      090h
            ENDM

