/** @file
  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

DefinitionBlock (
  "Dptf.aml",
  "SSDT",
  2,
  "DptfTab",
  "DptfTab",
  0x1000
  )
{
  External(\P8XH, MethodObj)
  External(\ECON, IntObj)
  External(\OSSL, IntObj)
  External(\PSVT, IntObj)
  External(\CRTT, IntObj)
  External(\ACTT, IntObj)
  External(\PWRS, IntObj)

  External(\_TZ.ETMD, IntObj)
  External(\_TZ.TZ01, ThermalZoneObj)
  External(\_TZ.LEGA, IntObj)
  External(\_TZ.LEGP, IntObj)
  External(\_TZ.LEGC, IntObj)

  External(\_PR.CPU0, DeviceObj)
  External(\_PR.CPU1, DeviceObj)
  External(\_PR.CPU2, DeviceObj)
  External(\_PR.CPU3, DeviceObj)

  External(\DPTE, IntObj) // DptfEnable
  External(\DCFE, IntObj) // EnableDCFG

  External(\DPSR, IntObj) // DptfProcessor
  External(\DPCT, IntObj) // DptfProcCriticalTemperature
  External(\DPPT, IntObj) // DptfProcPassiveTemperature
  External(\DPAT, IntObj) // DptfProcActiveTemperature
  External(\DPC3, IntObj) // DptfProcCriticalTemperatureS3
  External(\DPHT, IntObj) // DptfProcHotThermalTripPoint
  External(\CPUS, IntObj) // ThermalSamplingPeriodTCPU

  External(\DDSP, IntObj) // DptfDisplayDevice

  External(\S1DE, IntObj) // EnableSen1Participant
  External(\S1AT, IntObj) // ActiveThermalTripPointSen1
  External(\S1PT, IntObj) // PassiveThermalTripPointSen1
  External(\S1CT, IntObj) // CriticalThermalTripPointSen1
  External(\S1S3, IntObj) // CriticalThermalTripPointSen1S3
  External(\S1HT, IntObj) // HotThermalTripPointSen1
  External(\SSP1, IntObj) // SensorSamplingPeriodSen1

  External(\GN1E, IntObj) // EnableGen1Participant
  External(\G1AT, IntObj) // ActiveThermalTripPointGen1
  External(\G1PT, IntObj) // PassiveThermalTripPointGen1
  External(\G1CT, IntObj) // CriticalThermalTripPointGen1
  External(\G1C3, IntObj) // CriticalThermalTripPointGen1S3
  External(\G1HT, IntObj) // HotThermalTripPointGen1
  External(\TSP1, IntObj) // ThermistorSamplingPeriodGen1

  External(\GN2E, IntObj) // EnableGen2Participant
  External(\G2AT, IntObj) // ActiveThermalTripPointGen2
  External(\G2PT, IntObj) // PassiveThermalTripPointGen2
  External(\G2CT, IntObj) // CriticalThermalTripPointGen2
  External(\G2C3, IntObj) // CriticalThermalTripPointGen2S3
  External(\G2HT, IntObj) // HotThermalTripPointGen2
  External(\TSP2, IntObj) // ThermistorSamplingPeriodGen2

  External(\GN3E, IntObj) // EnableGen3Participant
  External(\G3AT, IntObj) // ActiveThermalTripPointGen3
  External(\G3PT, IntObj) // PassiveThermalTripPointGen3
  External(\G3CT, IntObj) // CriticalThermalTripPointGen3
  External(\G3C3, IntObj) // CriticalThermalTripPointGen3S3
  External(\G3HT, IntObj) // HotThermalTripPointGen3
  External(\TSP3, IntObj) // ThermistorSamplingPeriodGen3

  External(\GN4E, IntObj) // EnableGen4Participant
  External(\G4AT, IntObj) // ActiveThermalTripPointGen4
  External(\G4PT, IntObj) // PassiveThermalTripPointGen4
  External(\G4CT, IntObj) // CriticalThermalTripPointGen4
  External(\G4C3, IntObj) // CriticalThermalTripPointGen4S3
  External(\G4HT, IntObj) // HotThermalTripPointGen4
  External(\TSP4, IntObj) // ThermistorSamplingPeriodGen4

  External(\VSP1, IntObj) // EnableVS1Participant
  External(\V1AT, IntObj) // ActiveThermalTripPointVS1
  External(\V1PV, IntObj) // PassiveThermalTripPointVS1
  External(\V1CR, IntObj) // CriticalThermalTripPointVS1
  External(\V1C3, IntObj) // CriticalThermalTripPointVS1S3
  External(\V1HT, IntObj) // HotThermalTripPointVS1

  External(\VSP2, IntObj) // EnableVS2Participant
  External(\V2AT, IntObj) // ActiveThermalTripPointVS2
  External(\V2PV, IntObj) // PassiveThermalTripPointVS2
  External(\V2CR, IntObj) // CriticalThermalTripPointVS2
  External(\V2C3, IntObj) // CriticalThermalTripPointVS2S3
  External(\V2HT, IntObj) // HotThermalTripPointVS2

  External(\VSP3, IntObj) // EnableVS3Participant
  External(\V3AT, IntObj) // ActiveThermalTripPointVS3
  External(\V3PV, IntObj) // PassiveThermalTripPointVS3
  External(\V3CR, IntObj) // CriticalThermalTripPointVS3
  External(\V3C3, IntObj) // CriticalThermalTripPointVS3S3
  External(\V3HT, IntObj) // HotThermalTripPointVS3

  External(\DPAP, IntObj) // EnableActivePolicy
  External(\DPPP, IntObj) // EnablePassivePolicy
  External(\TRTV, IntObj) // TrtRevision
  External(\DPCP, IntObj) // EnableCriticalPolicy
  External(\PBPE, IntObj) // EnablePowerBossPolicy
  External(\VSPE, IntObj) // EnableVSPolicy

  External(\DFAN, IntObj) // DptfFanDevice
  External(\CHGE, IntObj) // DptfChargerDevice
  External(\PWRE, IntObj) // EnablePowerParticipant
  External(\PPPR, IntObj) // PowerParticipantPollingRate

  External(\ODV0, IntObj) // OemDesignVariable0
  External(\ODV1, IntObj) // OemDesignVariable1
  External(\ODV2, IntObj) // OemDesignVariable2
  External(\ODV3, IntObj) // OemDesignVariable3
  External(\ODV4, IntObj) // OemDesignVariable4
  External(\ODV5, IntObj) // OemDesignVariable5

  External(\_SB.PCI0, DeviceObj)
  External(\_SB.PCI0.VLVC.MHBR, FieldUnitObj)


  External(\_PR.CPU0._PSS, MethodObj)
  External(\_PR.CPU0.NPSS, PkgObj)
  External(\_PR.CPU0.SPSS, PkgObj)
  External(\_PR.CPU0._PPC, IntObj)
  External(\_PR.CPU0._TSS, MethodObj)
  External(\_PR.CPU0._PTC, MethodObj)
  External(\_PR.CPU0._TSD, MethodObj)
  External(\_PR.CPU0._TPC, IntObj)
  External(\_PR.CPU0._TDL, MethodObj)

  External(\_SB.PCI0.GFX0, DeviceObj)
  External(\_SB.PCI0.GFX0.DD1F, DeviceObj)
  External(\_SB.PCI0.GFX0.DD1F._BCM, MethodObj)
  External(\_SB.PCI0.GFX0.DD1F._BQC, MethodObj)
  External(\_SB.PCI0.GFX0.DD1F._DCS, MethodObj)

Scope(\_SB)
{
  //
  // DPTF Thermal Zone Device
  //
  Device(IETM){
    //
    // Intel DPTF Thermal Framework Device
    //
    Name(_HID, EISAID("INT3400"))

    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If (LEqual(\DPTE,1)) {
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    //
    // Note: the number of GUID package elements in TMPP must be equal or greater than the number
    // of store statements in IDSP in order to prevent an overrun.
    //
    Name(TMPP,Package()
    {
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000"),
      ToUUID("00000000-0000-0000-0000-000000000000")
    })

    // IDSP (Intel DPTF Supported Policies)
    //
    // This object evaluates to a package of packages, with each package containing the UUID
    // values to represent a policy implemented and supported by the Intel DPTF software stack.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Package of Guid packages
    //
    Method(IDSP,0,Serialized,,PkgObj)
    {
      Name(TMPI,0)

      // Passive Policy 2.0 GUID
      If (LAnd(LEqual(\DPPP,2),CondRefOf(DP2P))) {
        Store(DeRefOf(Index(DP2P,0)), Index(TMPP,TMPI))
        Increment(TMPI)
      }

      // Passive Policy 1.0 GUID
      If (LAnd(LEqual(\DPPP,1),CondRefOf(DPSP))) {
        Store(DeRefOf(Index(DPSP,0)), Index(TMPP,TMPI))
        Increment(TMPI)
      }

      // Active Policy GUID
      If (LAnd(LEqual(\DPAP,1),CondRefOf(DASP))) {
        Store(DeRefOf(Index(DASP,0)), Index(TMPP,TMPI))
        Increment(TMPI)
      }

      // Critical Policy GUID
      If (LAnd(LEqual(\DPCP,1),CondRefOf(DCSP))) {
        Store(DeRefOf(Index(DCSP,0)), Index(TMPP,TMPI))
        Increment(TMPI)
      }

      // Power Boss Policy GUID
      If (LAnd(LEqual(\PBPE,1),CondRefOf(POBP))) {
        Store(DeRefOf(Index(POBP,0)), Index(TMPP,TMPI))
        Increment(TMPI)
      }

      // Virtual Sensor Policy GUID
      If (LAnd(LEqual(\VSPE,1),CondRefOf(DVSP))) {
        Store(DeRefOf(Index(DVSP,0)), Index(TMPP,TMPI))
        Increment(TMPI)
      }

      Return(TMPP)
    }

    //
    // Save original trip points so _OSC method can enable/disable Legacy thermal policies by manipulating trip points.
    //
    Name (PTRP,0)  // Passive trip point
    Name (PSEM,0)  // Passive semaphore
    Name (ATRP,0)  // Active trip point
    Name (ASEM,0)  // Active semaphore
    Name (YTRP,0)  // Critical trip point
    Name (YSEM,0)  // Critical semaphore

    // _OSC (Operating System Capabilities)
    //
    // This object is evaluated by each DPTF policy implementation to communicate to the platform of the existence and/or control transfer.
    //
    // Arguments: (4)
    //   Arg0 - A Buffer containing a UUID
    //   Arg1 - An Integer containing a Revision ID of the buffer format
    //   Arg2 - An Integer containing a count of entries in Arg3
    //   Arg3 - A Buffer containing a list of DWORD capabilities
    // Return Value:
    //   A Buffer containing a list of capabilities
    //
    Method(_OSC, 4,Serialized,,BuffObj,{BuffObj,IntObj,IntObj,BuffObj})
    {
      Name (NUMP,0)
      Name (UID2,ToUUID("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"))

      // Point to Status DWORD in the Arg3 buffer (STATUS)
      CreateDWordField(Arg3, 0, STS1)

      // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
      CreateDWordField(Arg3, 4, CAP1)

      //
      // _OSC needs to validate the UUID and Revision.
      //
      // IF Unrecognized UUID
      //  Return Unrecognized UUID _OSC Failure
      // IF Unsupported Revision
      //  Return Unsupported Revision _OSC Failure
      //
      //    STS0[0] = Reserved
      //    STS0[1] = _OSC Failure
      //    STS0[2] = Unrecognized UUID
      //    STS0[3] = Unsupported Revision
      //    STS0[4] = Capabilities masked
      //
      // Get the platform UUID's that are available, this will be a package of packages.
      //
      IDSP()                     // initialize TMPP with GUID's
      Store(SizeOf(TMPP),NUMP)   // how many GUID's in the package?

      // Note:  The comparison method used is necessary due to
      // limitations of certain OSes which cannot perform direct
      // buffer comparisons.
      //
      // Create a set of "Input" UUID fields.
      //
      CreateDWordField(Arg0, 0x0, IID0)
      CreateDWordField(Arg0, 0x4, IID1)
      CreateDWordField(Arg0, 0x8, IID2)
      CreateDWordField(Arg0, 0xC, IID3)
      //
      // Create a set of "Expected" UUID fields.
      //
      CreateDWordField(UID2, 0x0, EID0)
      CreateDWordField(UID2, 0x4, EID1)
      CreateDWordField(UID2, 0x8, EID2)
      CreateDWordField(UID2, 0xC, EID3)
      //
      // Compare the input UUID to the list of UUID's in the system.
      //
      While(NUMP){
        //
        // copy one uuid from TMPP to UID2
        //
        Store(DeRefOf (Index (TMPP, Subtract(NUMP,1))),UID2)
        //
        // Verify the input UUID matches the expected UUID.
        //
        If (LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))) {
          Break  // break out of while loop when matching UUID is found
        }
        Decrement(NUMP)
      }

      If (LEqual(NUMP,0)) {
        //
        // Return Unrecognized UUID _OSC Failure
        //
        And(STS1,0xFFFFFF00,STS1)
        Or(STS1,0x6,STS1)
        Return(Arg3)
      }

      If (LNot(LEqual(Arg1, 1)))
      {
        //
        // Return Unsupported Revision _OSC Failure
        //
        And(STS1,0xFFFFFF00,STS1)
        Or(STS1,0xA,STS1)
        Return(Arg3)
      }

      If (LNot(LEqual(Arg2, 2)))
      {
        //
        // Return Argument 3 Buffer Count not sufficient
        //
        And(STS1,0xFFFFFF00,STS1)
        Or(STS1,0x2,STS1)
        Return(Arg3)
      }

      //
      // Passive Policy 2.0 GUID
      //
      If (CondRefOf(\PSVT)) {
        If (LEqual(PSEM,0)) {
          Store(1,PSEM)  // use semaphore so variable is only initialized once
          Store(\PSVT,PTRP) // save trip point in case we have to restore it
        }
        //
        // copy the GUID to UID2
        //
        If (CondRefOf(DP2P)) {
          Store(DeRefOf (Index (DP2P, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If (LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))) {
          // do passive notify
          If(Not(And(STS1, 0x01))) // Test Query Flag
          { // Not a query operation, so process the request
            If (And(CAP1, 0x01)) {
              // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(110,\PSVT) // spoof legacy trip point with high value
              Store(0, \_TZ.LEGP)  // disable legacy thermal management
            } Else {  // policy unloading, re-enable legacy thermal zone
              Store(PTRP,\PSVT) // restore passive value
              Store(1, \_TZ.LEGP) // enable legacy thermal management
            }
          // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
          Notify (\_TZ.TZ01, 0x81)
          }
          Return (Arg3)
        }
      }

      //
      // Active Policy GUID
      //
      If (CondRefOf(\ACTT)) {
        If (LEqual(ASEM,0)) {
          Store(1,ASEM)  // use semaphore so variable is only initialized once
          Store(\ACTT,ATRP) // save trip point in case we have to restore it
        }
        //
        // copy the GUID to UID2
        //
        If (CondRefOf(DASP)) {
          Store(DeRefOf (Index (DASP, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If (LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))) {
          // do active notify
          If (Not(And(STS1, 0x01))) {
            // Test Query Flag
            // Not a query operation, so process the request
            If(And(CAP1, 0x01)) {
              // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(110,\ACTT) // spoof legacy trip point with high value
              Store(0, \_TZ.LEGA)  // disable legacy thermal management
            } Else {  // policy unloading, re-enable legacy thermal zone
              Store(ATRP,\ACTT) // restore legacy value
              Store(1, \_TZ.LEGA) // enable legacy thermal management
            }
          // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
          Notify(\_TZ.TZ01, 0x81)
          }
          Return(Arg3)
        }
      }

      //
      // Critical Policy GUID
      //
      If (CondRefOf(\CRTT)) {
        If (LEqual(YSEM,0)) {
          Store(1,YSEM)  // use semaphore so variable is only initialized once
          Store(\CRTT,YTRP) // save trip point in case we have to restore it
        }
        //
        // copy the GUID to UID2
        //
        If (CondRefOf(DCSP)) {
          Store(DeRefOf (Index (DCSP, 0)),UID2)
        }
        //
        // Verify the test UUID matches the input UUID.
        //
        If (LAnd(LAnd(LEqual(IID0, EID0), LEqual(IID1, EID1)), LAnd(LEqual(IID2, EID2), LEqual(IID3, EID3)))) {
          // do critical notify
          If (Not(And(STS1, 0x01))) {
            // Test Query Flag
            // Not a query operation, so process the request
            If (And(CAP1, 0x01)) {
              // Enable DPTF
              // Nullify the legacy thermal zone.
              Store(210,\CRTT) // spoof legacy trip point with high value
              Store(0, \_TZ.LEGC)  // disable legacy thermal management
            } Else {  // policy unloading, re-enable legacy thermal zone
              Store(YTRP,\CRTT) // restore legacy value
              Store(1, \_TZ.LEGC) // enable legacy thermal management
            }
          // Send notification to legacy thermal zone for legacy policy to be enabled/disabled
          Notify(\_TZ.TZ01, 0x81)
          }
          Return(Arg3)
        }
      }

      Return(Arg3)
    } // _OSC

    // KTOC (Kelvin to Celsius)
    //
    // This control method converts from 10ths of degree Kelvin to Celsius.
    //
    // Arguments: (1)
    //   Arg0 - Temperature in 10ths of degree Kelvin
    // Return Value:
    //   Temperature in Celsius
    //
    Method(KTOC,1,Serialized)
    {
      If (LGreater(Arg0,2732)) { // make sure we have a temperature above zero Celcius
        Return(Divide(Subtract(Arg0,2732),10))
      } Else {
        Return(0) // negative temperatures Celcius are changed to 0 degrees Celcius
      }
    }

    // CTOK (Celsius to Kelvin)
    //
    // This control method converts from Celsius to 10ths of degree Kelvin.
    //
    // Arguments: (1)
    //   Arg0 - Temperature in Celsius
    // Return Value:
    //   Temperature in 10ths of degree Kelvin
    //
    Method(CTOK,1,Serialized)
    {
      Return(Add(Multiply(Arg0,10),2732))
    }

    // DCFG (DPTF Configuration)
    //
    // Returns a DWORD data representing the desired behavior of DPTF besides supported DSP and participants.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the DPTF Configuration bitmap:
    //    Bit 0 = Generic UI Access Control (0 - enable as default, 1 - disable access)
    //    Bit 1 = Restricted UI Access Control ( 0 - enable as default, 1 - disable access )
    //    Bit 2 = Shell Access Control ( 0 - enable as default, 1 - disable access)
    //    Bit 3 = Environment Monitoring Report Control ( 0 - report is allowed as default, 1 - No environmental monitoring report to Microsoft )
    //    Bit 4 = Thermal Mitigation Report Control ( 0 - No mitigation report to Microsoft as default, 1 - report is allowed)
    //    Bit 5 = Thermal Policy Report Control ( 0 - No policy report to Microsoft as default, 1 - report is allowed)
    //    Bits[31:6] - Reserved (must be cleared).
    //
    Method(DCFG)
    {
      Return(\DCFE)
    }

    // ODVP (Oem Design Variables Package)
    //
    // Variables for OEM's to customize DPTF behavior based on platform changes.
    //
    Name(ODVX,Package(){0,0,0,0,0,0})

    // ODVP (Oem Design Variables Package)
    //
    // Variables for OEM's to customize DPTF behavior based on platform changes.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   Package of integers
    //
    Method(ODVP,0,Serialized,,PkgObj)
    {
      Store(\ODV0,Index(ODVX,0))
      Store(\ODV1,Index(ODVX,1))
      Store(\ODV2,Index(ODVX,2))
      Store(\ODV3,Index(ODVX,3))
      Store(\ODV4,Index(ODVX,4))
      Store(\ODV5,Index(ODVX,5))
      Return(ODVX)
    }

  } // End IETM Device
} // End \_SB Scope




//
// CPU Participant
//
Include("TcpuParticipant.asl")

Include("DPLYParticipant.asl")

//
// Participants using motherboard sensors.
//
Include("SEN1Participant.asl")

//
// Fan participant.
//
Include("TFN1Participant.asl")

//
// Participants using device sensors.
//
Include("TPwrParticipant.asl")

//
// Participants using motherboard thermistors.
//
Include("Gen1Participant.asl")
Include("Gen2Participant.asl")
Include("Gen3Participant.asl")
Include("Gen4Participant.asl")

//
// Policy support files
//
Include("Art.asl")
Include("Trt.asl")
Include("Psvt.asl")
Include("Dppm.asl")

//
// Participants using virtual sensors.
//
Include("Vir1Participant.asl")
Include("Vir2Participant.asl")
Include("Vir3Participant.asl")

Scope(\_SB.IETM)
{
  // GDDV (Get Dptf Data Vault)
  //
  // The data vault can contain APCT, APAT, and PSVT tables.
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    A package containing the data vault
  //
  Method(GDDV,0,Serialized,0,PkgObj)
  {
    Return(Package()
    {
      Buffer()
      {
        Include("BiosDataVault.asl") // empty data vault for documentation purposes
      }
    })
  }
} // End Scope(\_SB.IETM)

} // End SSDT

