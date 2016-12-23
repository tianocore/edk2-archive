;; @file
;  This is the equates file for HT (Hyper-threading) support
;
;  Copyright (c) 2000 - 2016, Intel Corporation. All rights reserved.<BR>
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

VacantFlag             EQU   00h
NotVacantFlag          EQU   0ffh
StartupApSignal        EQU   6E755200h
MonitorFilterSize      EQU   40h
ApCounterInit          EQU   0
ApInHltLoop            EQU   1
ApInMwaitLoop          EQU   2
ApInRunLoop            EQU   3
WakeUpApPerMwaitLoop32 Equ   4
WakeUpApPerRunLoop32   Equ   5

LockLocation           EQU   1000h - 0400h
StackStart             EQU   LockLocation + 4h
StackSize              EQU   LockLocation + 8h
RendezvousProc         EQU   LockLocation + 0Ch
GdtrProfile            EQU   LockLocation + 10h
IdtrProfile            EQU   LockLocation + 16h
BufferStart            EQU   LockLocation + 1Ch
Cr3Location            EQU   LockLocation + 20h
InitFlag               EQU   LockLocation + 24h
WakeUpApManner         EQU   LockLocation + 28h
BistBuffer             EQU   LockLocation + 2Ch
PAUSE32   MACRO
            DB         0F3h
            DB         090h
          ENDM
