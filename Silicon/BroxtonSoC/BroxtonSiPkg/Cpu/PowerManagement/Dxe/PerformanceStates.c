/** @file
  This file contains P States and Turbo Power Management configuration functions for
  processors.

  <b>Acronyms:</b>
     PPM: Processor Power Management
     TM:  Thermal Monitor
     IST: Intel(R) Speedstep technology
     HT:  Hyper-Threading Technology

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PowerMgmtCommon.h"
#include <CpuDataStruct.h>

extern UINT16 mCpuConfigTdpBootRatio;

//
// Update ACPI PerfomanceStates tables
//

/**
  Patch the native _PSS package with the EIST values
  Uses ratio/VID values from the FVID table to fix up the control values in the _PSS.

  (1) Find _PSS package:
    (1.1) Find the _PR_CPU0 scope.
    (1.2) Save a pointer to the package length.
    (1.3) Find the _PSS AML name object.
  (2) Resize the _PSS package.
  (3) Fix up the _PSS package entries
    (3.1) Check Turbo mode support.
    (3.2) Check Dynamic FSB support.
  (4) Fix up the Processor block and \_PR_CPU0 Scope length.
  (5) Update SSDT Header with new length.

  @retval EFI_SUCCESS      On success
  @retval EFI_NOT_FOUND    If _PR_.CPU0 scope is not foud in the ACPI tables

**/
EFI_STATUS
AcpiPatchPss (
  VOID
  )
{
  UINT8               *CurrPtr;
  UINT8               *EndOfTable;
  UINT8               index;
  UINT16              NewPackageLength;
  UINT16              MaxPackageLength;
  UINT16              Temp;
  UINT16              *PackageLength;
  UINT16              *ScopePackageLengthPtr;
  UINT32              *Signature;
  PSS_PACKAGE_LAYOUT  *PssPackage;
  MSR_REGISTER        TempMsr;
  UINT16              MinBusRatio;
  UINT16              MaxNonTurboRatio;
  UINT16              NumberOfStatesTurbo;
  UINT16              Turbo;

  ScopePackageLengthPtr = NULL;
  PssPackage            = NULL;

  //
  // Get Maximum Efficiency bus ratio (LFM) from Platform Info MSR Bits[47:40]
  // Get Maximum Non Turbo bus ratio from Platform Info MSR Bits[15:8]
  //
  TempMsr.Qword = AsmReadMsr64 (MSR_PLATFORM_INFO);
  MinBusRatio = TempMsr.Bytes.SixthByte;
  MaxNonTurboRatio = TempMsr.Bytes.SecondByte;
  mNumberOfStates = mFvidPointer[0].FvidHeader.EistStates;
  NumberOfStatesTurbo = mNumberOfStates - (MaxNonTurboRatio - MinBusRatio + 1);
  Turbo = ((gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TURBO) ? 1 : 0);

  DEBUG ((EFI_D_INFO, "mNumberOfStates = %x\n", mNumberOfStates));
  DEBUG ((EFI_D_INFO, "NumberOfStatesTurbo = %x\n", NumberOfStatesTurbo));

  //
  // Locate the SSDT package in the IST table
  //
  CurrPtr     = (UINT8 *) mCpu0IstTable;
  EndOfTable  = (UINT8 *) (CurrPtr + mCpu0IstTable->Length);
  for (CurrPtr = (UINT8 *) mCpu0IstTable; CurrPtr <= EndOfTable; CurrPtr++) {
    Signature = (UINT32 *) (CurrPtr + 1);
    //
    // If we find the _PR_CPU0 scope, save a pointer to the package length
    //
    if ((*CurrPtr == AML_SCOPE_OP) &&
        (*(Signature + 1) == SIGNATURE_32 ('_', 'P', 'R', '_')) &&
        (*(Signature + 2) == SIGNATURE_32 ('C', 'P', 'U', '0'))
        ) {
      ScopePackageLengthPtr = (UINT16 *) (CurrPtr + 1);
    }
    //
    // Patch the NPSS package for Windows usage
    //
    if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('N', 'P', 'S', 'S'))) {
      DEBUG ((EFI_D_INFO, "N P S S start here\n"));
      ///
      /// Calculate new package length
      ///
      NewPackageLength      = Temp = (UINT16) ((Turbo + MaxNonTurboRatio - MinBusRatio + 1) * sizeof (PSS_PACKAGE_LAYOUT) + 3);
      MaxPackageLength      = (UINT16) (FVID_MAX_STATES * sizeof (PSS_PACKAGE_LAYOUT) + 3);
      //
      // Check table dimensions.
      // PSS package reserve space for FVID_MAX_STATES number of P-states so check if the
      // current number of P- states is more than FVID_MAX_STATES. Also need to update the SSDT contents
      // if the current number of P-states is less than FVID_MAX_STATES.
      //
      ASSERT (mNumberOfStates <= FVID_MAX_STATES);
      if (mNumberOfStates <= FVID_MAX_STATES) {
        *(CurrPtr + 8)  = (UINT8) (Turbo + MaxNonTurboRatio - MinBusRatio + 1);
        PackageLength   = (UINT16 *) (CurrPtr + 6);
        //
        // Update the Package length in AML package length format
        //
        *PackageLength = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);
        //
        // Move SSDT contents
        //
        CopyMem (
          (CurrPtr + NewPackageLength),
          (CurrPtr + MaxPackageLength),
          EndOfTable - (CurrPtr + MaxPackageLength)
          );
        //
        // Save the new end of the SSDT
        //
        EndOfTable = EndOfTable - (MaxPackageLength - NewPackageLength);
      }
      PssPackage = (PSS_PACKAGE_LAYOUT *) (CurrPtr + 9);
      for (index = 1; index <= (Turbo + MaxNonTurboRatio - MinBusRatio + 1); index++) {
        //
        // If Turbo mode is supported, add one to the Max Non-Turbo frequency
        //
        if ((gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TURBO) && (index == 1)) {
          PssPackage->CoreFrequency = (UINT32) ((mFvidPointer[index + NumberOfStatesTurbo].FvidState.BusRatio)* 100) + 1;
          PssPackage->Control       = (UINT32) LShiftU64 (mFvidPointer[index].FvidState.BusRatio, 8);
          PssPackage->Status        = (UINT32) LShiftU64 (mFvidPointer[index].FvidState.BusRatio, 8);
          PssPackage->Power         = (UINT32) mFvidPointer[index].FvidState.Power;
        } else if (gCpuGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TURBO) {
          PssPackage->CoreFrequency = (UINT32) (mFvidPointer[index + NumberOfStatesTurbo - 1].FvidState.BusRatio) * 100;
          PssPackage->Control       = (UINT32) LShiftU64 (mFvidPointer[index + NumberOfStatesTurbo - 1].FvidState.BusRatio, 8);
          PssPackage->Status        = (UINT32) LShiftU64 (mFvidPointer[index + NumberOfStatesTurbo - 1].FvidState.BusRatio, 8);
          PssPackage->Power         = (UINT32) mFvidPointer[index + NumberOfStatesTurbo - 1].FvidState.Power;
        } else {
          PssPackage->CoreFrequency = (UINT32) (mFvidPointer[index].FvidState.BusRatio) * 100;
          PssPackage->Control       = (UINT32) LShiftU64 (mFvidPointer[index].FvidState.BusRatio, 8);
          PssPackage->Status        = (UINT32) LShiftU64 (mFvidPointer[index].FvidState.BusRatio, 8);
          PssPackage->Power         = (UINT32) mFvidPointer[index].FvidState.Power;
        }

        PssPackage->TransLatency  = NATIVE_PSTATE_LATENCY;
        PssPackage->BMLatency     = PSTATE_BM_LATENCY;

        //
        // Check Pss Package
        //
        DEBUG ((EFI_D_INFO, "PssPackage->CoreFrequency = %x\n", PssPackage->CoreFrequency));
        DEBUG ((EFI_D_INFO, "PssPackage->Control = %x\n", PssPackage->Control));
        DEBUG ((EFI_D_INFO, "PssPackage->Power = %x\n", PssPackage->Power));

        PssPackage++;
      }
    }
    if ((*CurrPtr == AML_NAME_OP) && (*Signature == SIGNATURE_32 ('S', 'P', 'S', 'S'))) {
      DEBUG ((EFI_D_INFO, "S P S S start here\n"));
      //
      // Calculate new package length
      //
      NewPackageLength    = Temp = (UINT16) (mNumberOfStates * sizeof (PSS_PACKAGE_LAYOUT) + 3);
      MaxPackageLength    = (UINT16) (FVID_MAX_STATES * sizeof (PSS_PACKAGE_LAYOUT) + 3);
      //
      // Check table dimensions.
      // PSS package reserve space for FVID_MAX_STATES number of P-states so check if the
      // current number of P- states is more than FVID_MAX_STATES. Also need to update the SSDT contents
      // if the current number of P-states is less than FVID_MAX_STATES.
      //
      ASSERT (mNumberOfStates <= FVID_MAX_STATES);
      if (mNumberOfStates <= FVID_MAX_STATES) {
        *(CurrPtr + 8)  = (UINT8) mNumberOfStates;
        PackageLength   = (UINT16 *) (CurrPtr + 6);
        //
        // Update the Package length in AML package length format
        //
        *PackageLength = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);
        //
        // Move SSDT contents
        //
        CopyMem (
          (CurrPtr + NewPackageLength),
          (CurrPtr + MaxPackageLength),
          EndOfTable - (CurrPtr + MaxPackageLength)
          );
        //
        // Save the new end of the SSDT
        //
        EndOfTable = EndOfTable - (MaxPackageLength - NewPackageLength);
      }
      PssPackage = (PSS_PACKAGE_LAYOUT *) (CurrPtr + 9);
      for (index = 1; index <= mNumberOfStates; index++) {
        //
        // If Turbo mode is supported, expose all entries
        //
        PssPackage->CoreFrequency = (UINT32)((mFvidPointer[index].FvidState.BusRatio)* 100);
        PssPackage->Control       = (UINT32) LShiftU64 (mFvidPointer[index].FvidState.BusRatio, 8);
        PssPackage->Status        = (UINT32) LShiftU64 (mFvidPointer[index].FvidState.BusRatio, 8);
        PssPackage->Power         = (UINT32) mFvidPointer[index].FvidState.Power;
        PssPackage->TransLatency  = NATIVE_PSTATE_LATENCY;
        PssPackage->BMLatency     = PSTATE_BM_LATENCY;

        //
        // Check Pss Package
        //
        DEBUG ((EFI_D_INFO, "PssPackage->CoreFrequency = %x\n", PssPackage->CoreFrequency));
        DEBUG ((EFI_D_INFO, "PssPackage->Control = %x\n", PssPackage->Control));
        DEBUG ((EFI_D_INFO, "PssPackage->Power = %x\n", PssPackage->Power));

        PssPackage++;
      }
    }
  }
  ASSERT (ScopePackageLengthPtr != NULL);
  if (ScopePackageLengthPtr == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  // Update the Package length in AML package length format
  //
  CurrPtr                 = (UINT8 *) ScopePackageLengthPtr;
  NewPackageLength        = Temp = (UINT16) (EndOfTable - CurrPtr);
  *ScopePackageLengthPtr  = ((NewPackageLength & 0x0F) | 0x40) | ((Temp << 4) & 0x0FF00);
  mCpu0IstTable->Length   = (UINT32) (EndOfTable - (UINT8 *) mCpu0IstTable);

  return EFI_SUCCESS;
}


