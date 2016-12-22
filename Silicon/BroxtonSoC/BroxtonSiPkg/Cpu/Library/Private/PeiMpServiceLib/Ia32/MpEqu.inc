;; @file
;  This is the equates file used in MpFuncs.asm, for MP support.
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

VacantFlag             Equ   00h
NotVacantFlag          Equ   0ffh

LockLocation                  equ        1000h - 0400h
StackStartAddressLocation     equ        LockLocation + 04h
StackSizeLocation             equ        LockLocation + 08h
CProcedureLocation            equ        LockLocation + 0Ch
GdtrLocation                  equ        LockLocation + 10h
IdtrLocation                  equ        LockLocation + 16h
BufferStartLocation           equ        LockLocation + 1Ch
PmodeOffsetLocation           equ        LockLocation + 20h
AcpiCpuDataAddressLocation    equ        LockLocation + 24h
MtrrValuesAddressLocation     equ        LockLocation + 28h
FinishedCountAddressLocation  equ        LockLocation + 2Ch
WakeupCountAddressLocation    equ        LockLocation + 30h
SerializeLockAddressLocation  equ        LockLocation + 34h
MicrocodeAddressLocation      equ        LockLocation + 38h
BootScriptAddressLocation     equ        LockLocation + 3Ch
StartStateLocation            equ        LockLocation + 40h
VirtualWireMode               equ        LockLocation + 44h
SemaphoreCheck                equ        LockLocation + 48h
PeiServices                   equ        LockLocation + 4Ch
PeiStall                      equ        LockLocation + 50h
CpuPerfCtrlValue              equ        LockLocation + 54h
SiCpuPolicyPpi                equ        LockLocation + 5Ch
MpSystemDataAddressLocation   equ        LockLocation + 64h
MpServicePpiAddressLocation   equ        LockLocation + 68h
CArgumentLocation             equ        LockLocation + 6Ch
BistBufferLocation            equ        LockLocation + 70h

PAUSE32   MACRO
            DB      0F3h
            DB      090h
            ENDM
