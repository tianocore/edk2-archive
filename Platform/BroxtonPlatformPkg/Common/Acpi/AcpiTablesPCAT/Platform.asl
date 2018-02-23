/** @file
  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

// Define the following External variables to prevent a WARNING when
// using ASL.EXE and an ERROR when using IASL.EXE.

External(PDC0)
External(PDC1)
External(PDC2)
External(PDC3)
External(\_PR.CPU0._PPC, IntObj)
Name(ECUP, 1)  // EC State indicator: 1- Normal Mode 0- Low Power Mode
Mutex(EHLD, 0) // EC Hold indicator: 0- No one accessing the EC Power State 1- Someone else is accessing the EC Power State

External(\_SB.IETM,DeviceObj)
External(\_SB.IETM.ODVP,MethodObj)
External(\_SB.IETM.ODVX,PkgObj)
External(\_SB.TPM.PTS, MethodObj)
Name (ADW1, 0)

//
// Create a Global MUTEX.
//
Mutex(MUTX,0)

// Define Port 80 as an ACPI Operating Region to use for debugging.  Please
// note that the Intel CRBs have the ability to ouput an entire DWord to
// Port 80h for debugging purposes, so the model implemented here may not be
// able to be used on OEM Designs.

OperationRegion(PRT0,SystemIO,0x80,4)
Field(PRT0,DwordAcc,Lock,Preserve)
{
  P80H, 32
}

// Port 80h Update:
//    Update 8 bits of the 32-bit Port 80h.
//
//  Arguments:
//    Arg0: 0 = Write Port 80h, Bits 7:0 Only.
//            1 = Write Port 80h, Bits 15:8 Only.
//            2 = Write Port 80h, Bits 23:16 Only.
//            3 = Write Port 80h, Bits 31:24 Only.
//    Arg1: 8-bit Value to write
//
//  Return Value:
//    None

Method(P8XH,2,Serialized)
{
  If (LEqual(Arg0,0))    // Write Port 80h, Bits 7:0.
  {
    Store(Or(And(P80D,0xFFFFFF00),Arg1),P80D)
  }

  If (LEqual(Arg0,1))    // Write Port 80h, Bits 15:8.
  {
    Store(Or(And(P80D,0xFFFF00FF),ShiftLeft(Arg1,8)),P80D)
  }

  If (LEqual(Arg0,2))    // Write Port 80h, Bits 23:16.
  {
    Store(Or(And(P80D,0xFF00FFFF),ShiftLeft(Arg1,16)),P80D)
  }

  If (LEqual(Arg0,3))    // Write Port 80h, Bits 31:24.
  {
    Store(Or(And(P80D,0x00FFFFFF),ShiftLeft(Arg1,24)),P80D)
  }

  Store(P80D,P80H)
}

//
// Define SW SMI port as an ACPI Operating Region to use for generate SW SMI.
//
OperationRegion (SPRT, SystemIO, 0xB2, 2)
Field (SPRT, ByteAcc, Lock, Preserve) {
  SSMP, 8
}

// The _PIC Control Method is optional for ACPI design.  It allows the
// OS to inform the ASL code which interrupt controller is being used,
// the 8259 or APIC.  The reference code in this document will address
// PCI IRQ Routing and resource allocation for both cases.
//
// The values passed into _PIC are:
//   0 = 8259
//   1 = IOAPIC

Method(\_PIC,1)
{
  Store(Arg0,GPIC)
  Store(Arg0,PICM)
}

OperationRegion(SWC0, SystemIO, 0x610, 0x0F)
Field(SWC0, ByteAcc, NoLock, Preserve)
{
  G1S, 8,      //SWC GPE1_STS
  Offset(0x4),
  G1E, 8,
  Offset(0xA),
  G1S2, 8,     //SWC GPE1_STS_2
  G1S3, 8      //SWC GPE1_STS_3
}

OperationRegion (SWC1, SystemIO, \PMBS, 0x2C)
Field(SWC1, DWordAcc, NoLock, Preserve)
{
  Offset(0x20),
  G0S, 32,      //GPE0_STS
  Offset(0x28),
  G0EN, 32      //GPE0_EN
}

OperationRegion (PMCM, SystemMemory, Add(DD1A,0x1000), 0x1000)
Field (PMCM, ByteAcc, NoLock, Preserve)
{
  Offset (0x94),
  DHPD, 32,       // DISPLAY_HPD_CTL
}

// Prepare to Sleep.  The hook is called when the OS is about to
// enter a sleep state.  The argument passed is the numeric value of
// the Sx state.

Method(_PTS,1)
{
  Store(0,P80D)   // Zero out the entire Port 80h DWord.
  P8XH(0,Arg0)    // Output Sleep State to Port 80h, Byte 0.

  //clear the 3 SWC status bits
  Store(Ones, G1S3)
  Store(Ones, G1S2)
  Store(1, G1S)

  //set SWC GPE1_EN
  Store(1,G1E)

  //clear GPE0_STS
  Store(Ones, G0S)

  If (CondRefOf(\_SB.TPM.PTS)) {
    //
    // Call TPM PTS method
    //
    \_SB.TPM.PTS (Arg0)
  }

  //
  // Set GPIO_116 (SOC_CODEC_IRQ) 20k pull-down for device I2S audio codec INT343A before enter S3/S4
  //
  If (LAnd (LEqual (IOBF, 3), LEqual (IS3A, 1))) {
    Store (\_SB.GPC1 (NW_GPIO_116), ADW1)
    Or (ADW1, 0xFFFFC3FF, ADW1)
    And (ADW1, 0x00001000, ADW1)
    \_SB.SPC1 (NW_GPIO_116, ADW1)
  }
}

// Wake.  This hook is called when the OS is about to wake from a
// sleep state.  The argument passed is the numeric value of the
// sleep state the system is waking from.

Method(_WAK,1,Serialized)
{
  P8XH(1,0xAB) // Beginning of _WAK.

  Notify(\_SB.PWRB,0x02)

  If (NEXP) {
    // Reinitialize the Native PCI Express after resume
    If (And(OSCC,0x02)) {
      \_SB.PCI0.NHPG()
    }

    If (And(OSCC,0x04)) {
    // PME control granted?
      \_SB.PCI0.NPME()
    }
  }

  If (LOr(LEqual(Arg0,3), LEqual(Arg0,4))) { // If S3 or S4 Resume
    If (LEqual(PFLV,FMBL)) {
      //
      // if battery has changed from previous state i.e after Hibernate
      // then update the PWRS and update the SMM Power state
      //
      If (LEqual(Arg0,4)) {
        // Perform needed ACPI Notifications.
        PNOT()
      }
    }


    // Windows XP SP2 does not properly restore the P-State
    // upon resume from S4 or S3 with degrade modes enabled.
    // Use the existing _PPC methods to cycle the available
    // P-States such that the processor ends up running at
    // the proper P-State.
    //
    // Note:  For S4, another possible W/A is to always boot
    // the system in LFM.
    //
    If (LEqual(OSYS,2002)) {
      If (LGreater(\_PR.CPU0._PPC,0)) {
        Subtract(\_PR.CPU0._PPC,1,\_PR.CPU0._PPC)
        PNOT()
        Add(\_PR.CPU0._PPC,1,\_PR.CPU0._PPC)
        PNOT()
        } Else {
          Add(\_PR.CPU0._PPC,1,\_PR.CPU0._PPC)
          PNOT()
          Subtract(\_PR.CPU0._PPC,1,\_PR.CPU0._PPC)
          PNOT()
        }
    }

    //
    // Invoke SD card wake up method
    //
    \_SB.PCI0.SDC.WAK()
  }
  Return(Package(){0,0})
}


// Power Notification:
//    Perform all needed OS notifications during a
//    Power Switch.
//
//  Arguments:
//    None
//
//  Return Value:
//    None

Method(PNOT,0,Serialized)
{
  //
  // If MP enabled and driver support is present, notify both
  // processors.
  //
  If (MPEN) {
    If (And(PDC0,0x0008)) {
      Notify(\_PR.CPU0,0x80)    // Eval CPU0 _PPC.

      If (And(PDC0,0x0010)) {
        Sleep(100)
        Notify(\_PR.CPU0,0x81)  // Eval _CST.
      }
    }

    If (And(PDC1,0x0008)) {
      Notify(\_PR.CPU1,0x80)    // Eval CPU1 _PPC.

      If (And(PDC1,0x0010)) {
        Sleep(100)
        Notify(\_PR.CPU1,0x81)  // Eval _CST.
      }
    }

    If (And(PDC2,0x0008)) {
      Notify(\_PR.CPU2,0x80)    // Eval CPU2 _PPC.

      If (And(PDC2,0x0010)) {
        Sleep(100)
        Notify(\_PR.CPU2,0x81)  // Eval _CST.
      }
    }

    If (And(PDC3,0x0008)) {
      Notify(\_PR.CPU3,0x80)    // Eval CPU3 _PPC.

      If (And(PDC3,0x0010)) {
        Sleep(100)
        Notify(\_PR.CPU3,0x81)  // Eval _CST.
      }
    }
  } Else {
    Notify(\_PR.CPU0,0x80)      // Eval _PPC.
    Sleep(100)
    Notify(\_PR.CPU0,0x81)      // Eval _CST
  }

  If (LEqual(\DPTE,1)) {
    Notify(\_SB.IETM, 0x86) // Notification sent to DPTF driver (Policy) for PDRT reevaluation after AC/DC transtion has occurred.
  }
} //end of PNOT

Name(CLMP, 0) // save the clamp bit
Name(PLEN, 0) // save the power limit enable bit
Name(PLSV, 0x8000) // save value of PL1 upon entering CS
Name(CSEM, 0) //semaphore to avoid multiple calls to SPL1.  SPL1/RPL1 must always be called in pairs, like push/pop off a stack
//
// SPL1 (Set PL1 to 4.5 watts with clamp bit set)
//   Per Legacy Thermal management CS requirements, we would like to set the PL1 limit when entering CS to 4.5W with clamp bit set via MMIO.
//   This can be done in the ACPI object which gets called by graphics driver during CS Entry.
//   Likewise, during CS exit, the BIOS must reset the PL1 value to the previous value prior to CS entry and reset the clamp bit.
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(SPL1,0,Serialized)
{
    If (LEqual(CSEM, 1))
    {
      Return() // we have already been called, must have CS exit before calling again
    }
    Store(1, CSEM) // record first call
}
//
// RPL1 (Restore the PL1 register to the values prior to CS entry)
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(RPL1,0,Serialized)
{
    Store(0, CSEM)      // restore semaphore
}

Name(DDPS, 0) // Current Display Power Status. 0= D0; non-zero = Dx state;. Initial value is zero.
Name(UAMS, 0) // User Absent Mode state, Zero - User Present; non-Zero - User not present
// GUAM - Global User Absent Mode
//    Run when a change to User Absent mode is made,  e.g. screen/display on/off events.
//    Any device that needs notifications of these events includes its own UAMN Control Method.
//
//    Arguments:
//      Power State:
//        00h = On
//        01h = Standby
//        02h = Suspend
//        04h = Off
//        08h = Reduced On
//
//    Return Value:
//      None
//
Method(GUAM,1,Serialized)
{
  If (LNotEqual(Arg0, DDPS)) { // Display controller D-State changed?
    Store(Arg0, DDPS) //Update DDPS to current state
    Store(LAnd(Arg0, LNot(PWRS)), UAMS) // UAMS: User Absent Mode state, Zero - User Present; non-Zero - User not present

    //Port 80 code for CS
    If (Arg0) {


      If (PLCS) {
        SPL1() // set PL1 to low value upon CS entry
      }
    } Else {

    }

    P_CS() // Powergating during CS
  }
}

// Power CS Powergated Devices:
//    Method to enable/disable power during CS
Method(P_CS,0,Serialized)
{
}

//
// System Bus
//
Scope(\_SB)
{
  Scope(PCI0)
  {
    Method(_INI,0)
    {
      // Determine the OS and store the value, where:
      //
      //   OSYS = 2000 = WIN2000.
      //   OSYS = 2001 = WINXP, RTM or SP1.
      //   OSYS = 2002 = WINXP SP2.
      //   OSYS = 2006 = Vista.
      //   OSYS = 2009 = Windows 7 and Windows Server 2008 R2.
      //   OSYS = 2012 = Windows 8 and Windows Server 2012.
      //   OSYS = 2013 = Windows Blue.
      //   OSYS = 2015 = Windows 10.
      //
      // Assume Windows 2000 at a minimum.

      Store(2000,OSYS)

      // Check for a specific OS which supports _OSI.

      If (CondRefOf(\_OSI)) {
        // Linux returns _OSI = TRUE for numerous Windows
        // strings so that it is fully compatible with
        // BIOSes available in the market today.  There are
        // currently 2 known exceptions to this model:
        //  1) Video Repost - Linux supports S3 without
        //    requiring a Driver, meaning a Video
        //    Repost will be required.
        //  2) On-Screen Branding - a full CMT Logo
        //    is limited to the WIN2K and WINXP
        //    Operating Systems only.

        // Use OSYS for Windows Compatibility.

        If(\_OSI("Windows 2001")) // Windows XP
        {
          Store(2001,OSYS)
        }

        If(\_OSI("Windows 2001 SP1")) // Windows XP SP1
        {
          Store(2001,OSYS)
        }

        If(\_OSI("Windows 2001 SP2")) // Windows XP SP2
        {
          Store(2002,OSYS)
        }

        If(\_OSI("Windows 2006")) // Windows Vista
        {
          Store(2006,OSYS)
        }

        If(\_OSI("Windows 2009")) // Windows 7 and Windows Server 2008 R2
        {
          Store(2009,OSYS)
        }

        If(\_OSI("Windows 2012")) //Windows 8 and Windows Server 2012
        {
          Store(2012,OSYS)
        }

        If(\_OSI("Windows 2013")) //Windows 8.1 and Windows Server 2012 R2
        {
          Store(2013,OSYS)
        }

        If(\_OSI("Windows 2015")) //Windows 10
        {
          Store(2015,OSYS)
        }
        //
        // If CMP is enabled, enable SMM C-State
        // coordination.  SMM C-State coordination
        // will be disabled in _PDC if driver support
        // for independent C-States deeper than C1
        // is indicated.
      }
    }

    Method(NHPG,0,Serialized)
    {
      Store(0,^RP01.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP02.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP03.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP04.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP05.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP06.HPEX) // clear the hot plug SCI enable bit
      Store(1,^RP01.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP02.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP03.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP04.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP05.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP06.HPSX) // clear the hot plug SCI status bit
    }

    Method(NPME,0,Serialized)
    {
      Store(0,^RP01.PMEX) // clear the PME SCI enable bit
      Store(0,^RP02.PMEX) // clear the PME SCI enable bit
      Store(0,^RP03.PMEX) // clear the PME SCI enable bit
      Store(0,^RP04.PMEX) // clear the PME SCI enable bit
      Store(0,^RP05.PMEX) // clear the PME SCI enable bit
      Store(0,^RP06.PMEX) // clear the PME SCI enable bit
      Store(1,^RP01.PMSX) // clear the PME SCI status bit
      Store(1,^RP02.PMSX) // clear the PME SCI status bit
      Store(1,^RP03.PMSX) // clear the PME SCI status bit
      Store(1,^RP04.PMSX) // clear the PME SCI status bit
      Store(1,^RP05.PMSX) // clear the PME SCI status bit
      Store(1,^RP06.PMSX) // clear the PME SCI status bit
    }
  } // end Scope(PCI0)

  //--------------------
  //  GPIO
  //--------------------

  Device (GPO0) // North Community for DFx GPIO, SATA GPIO, PWM, LPSS/ISH UARTs, IUnit GPIO, JTAG, and SVID
  {
    Name (_ADR, 0)
    Name (_HID, "INT3452")
    Name (_CID, "INT3452")
    Name (_DDN, "General Purpose Input/Output (GPIO) Controller - North" )
    Name (_UID, 1)
    Name (LINK, "\\_SB.GPO0") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, BAR0)
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {14}
    })

    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
      CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
      Store(GP0A, B0BA)
      Store(GP0L, B0LN)
      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized)
    {
      Return(0xf)
    }
    // Track status of GPIO OpRegion availability for this controller
    Name(AVBL, 0)
    Method(_REG,2) {
       If (Lequal(Arg0, 8)) {
         Store(Arg1, ^AVBL)
       }
    }

    OperationRegion(GPOP, SystemMemory, GP0A, GP0L)
    Field(\_SB.GPO0.GPOP, ByteAcc, NoLock, Preserve) {
      Offset(0x550), //PIN 10:10 * 8 + 0x500  // WiFi Reset
      CWLE, 1,
      Offset(0x5B0), //PIN 22:22 * 8 + 0x500  // SATA_ODD_PWRGT_R
      ODPW, 1
    }
  }   //  Device (GPO0)

  Device (GPO1) // Northwest Community for Display GPIO, PMC, Audio, and SPI
  {
    Name (_ADR, 0)
    Name (_HID, "INT3452")
    Name (_CID, "INT3452")
    Name (_DDN, "General Purpose Input/Output (GPIO) Controller - Northwest" )
    Name (_UID, 2)

    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, BAR0)
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {14}
    })

    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
      CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
      Store(GP1A, B0BA)
      Store(GP1L, B0LN)
      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized)
    {
      If (LLess(OSYS,2012)) {
        // Don't report this GPIO for WIN7
        Return (0)
      }
      Return(0xf)
    }

  }   //Device (GPO1)


  Device (GPO2) // West Community for LPSS/ISH I2C, ISH GPIO, iCLK, and PMU
  {
    Name (_ADR, 0)
    Name (_HID, "INT3452")
    Name (_CID, "INT3452")
    Name (_DDN, "General Purpose Input/Output (GPIO) Controller - West" )
    Name (_UID, 3)

    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, BAR0)
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {14}
    })

    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
      CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
      Store(GP2A, B0BA)
      Store(GP2L, B0LN)
      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized)
    {
      If (LLess(OSYS,2012)) {
        // Don't report this GPIO for WIN7
        Return (0)
      }
      Return(0xf)
    }
  }   //Device (GPO2)

  Device (GPO3) // Southwest Community for EMMC, SDIO, SDCARD, SMBUS, and LPC
  {
    Name (_ADR, 0)
    Name (_HID, "INT3452")
    Name (_CID, "INT3452")
    Name (_DDN, "General Purpose Input/Output (GPIO) Controller - Southwest" )
    Name (_UID, 4)

    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, BAR0)
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {14}
    })

    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
      CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
      Store(GP3A, B0BA)
      Store(GP3L, B0LN)
      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized)
    {
      If(LLess(OSYS,2012)) {
        // Don't report this GPIO for WIN7
        Return (0)
      }
      Return(0xf)
    }

    // Track status of GPIO OpRegion availability for this controller
    Name(AVBL, 0)
    Method(_REG,2) {
      If (Lequal(Arg0, 8)) {
        Store(Arg1, ^AVBL)
      }
    }

    OperationRegion(GPOP, SystemMemory, GP3A, GP3L)
    Field(\_SB.GPO3.GPOP, ByteAcc, NoLock, Preserve) {
      Offset(0x5F0), //PIN 30: 30 * 8 + 0x500 // GPIO_183 SD_CARD_PWR_EN_N
      SDPC, 1
    }
  }   //Device (GPO3)

  Scope (\_SB) {
    Device(IPC1)
    {
      Name (_ADR, 0)
      Name (_HID, "INT34D2")
      Name (_CID, "INT34D2")

      Name (_DDN, "Intel(R) IPCI controller ")
      Name (_UID, 1)

      Name (RBUF, ResourceTemplate ()
      {
        Memory32Fixed (ReadWrite, 0x00000000, 0x00002000, BAR0)    // IPC1 Bar, 8KB
        Memory32Fixed (ReadWrite, 0x00000000, 0x00000004, MDAT)    // PUnit mailbox Data
        Memory32Fixed (ReadWrite, 0x00000000, 0x00000004, MINF)    // PUnit mailbox Interface
        Memory32Fixed (ReadWrite, 0x00000000, 0x00002000, BAR1)    // SSRAM
        IO (Decode16, 0x400, 0x480, 0x4, 0x80)  //ACPI IO Base address
        Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, , , ) {40}  // IPC1 IRQ
      })
  

      Method (_CRS, 0x0, NotSerialized)
      {
        CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
        CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
        Store(DD1A, B0BA) // D13A is the  BAR high address for B0/D13/F1
        Store(DD1L, B0LN) // D13L is the BAR length for B0/D13/F1

        CreateDwordField(^RBUF, ^MDAT._BAS, BM01)
        CreateDwordField(^RBUF, ^MDAT._LEN, BML1)
        CreateDwordField(^RBUF, ^MINF._BAS, BM02)
        CreateDwordField(^RBUF, ^MINF._LEN, BML2)
        Store(BMDA, BM01)                        // BMDA is the mail box data
        Store(4, BML1)                           // Length for BMDA is 4 bytes
        Store(BMIA, BM02)                        // BMDA is the mail box interface
        Store(4, BML2)                           // Length for BMIA is 4 bytes

        CreateDwordField(^RBUF, ^BAR1._BAS, B1BA)
        CreateDwordField(^RBUF, ^BAR1._LEN, B1LN)
        Store(DD3A, B1BA) // D13A is the  BAR high address for B0/D13/F3
        Store(DD3L, B1LN) // D13L is the BAR length for B0/D13/F3

        Return (RBUF)
      }

      Method (_STA, 0x0, NotSerialized) {
        If (LEqual (OSYS, 2015)) {
          Return (0x0)
        } else {
          Return (0xF)
        }
      }
    }
  }//end scope

  //
  // Define a Control Method Power Button.
  //
  Device(PWRB)
  {
    Name(_HID,EISAID("PNP0C0C"))

    // GPE16 = Waketime SCI.  
    Name(_PRW, Package() {16,4})
  }

  Device(SLPB)
  {
    Name(_HID, EISAID("PNP0C0E"))
  }
} // end Scope(\_SB)

Scope (\)
{
  //
  // Global Name, returns current Interrupt controller mode;
  // updated from _PIC control method
  //
  Name(PICM, 0)

  //
  // Procedure: GPRW
  //
  // Description: Generic Wake up Control Method ("Big brother")
  //              to detect the Max Sleep State available in ASL Name scope
  //              and Return the Package compatible with _PRW format.
  // Input: Arg0 =  bit offset within GPE register space device event will be triggered to.
  //        Arg1 =  Max Sleep state, device can resume the System from.
  //                If Arg1 = 0, Update Arg1 with Max _Sx state enabled in the System.
  // Output:  _PRW package
  //
  Name(PRWP, Package(){Zero, Zero})   // _PRW Package

  Method(GPRW, 2)
  {
    Store(Arg0, Index(PRWP, 0))             // copy GPE#
    //
    // SS1-SS4 - enabled in BIOS Setup Sleep states
    //
    Store(ShiftLeft(SS1,1),Local0)          // S1 ?
    Or(Local0,ShiftLeft(SS2,2),Local0)      // S2 ?
    Or(Local0,ShiftLeft(SS3,3),Local0)      // S3 ?
    Or(Local0,ShiftLeft(SS4,4),Local0)      // S4 ?
    //
    // Local0 has a bit mask of enabled Sx(1 based)
    // bit mask of enabled in BIOS Setup Sleep states(1 based)
    //
    If (And(ShiftLeft(1, Arg1), Local0)) {
      //
      // Requested wake up value (Arg1) is present in Sx list of available Sleep states
      //
      Store(Arg1, Index(PRWP, 1))           // copy Sx#
    } Else {
      //
      // Not available -> match Wake up value to the higher Sx state
      //
      ShiftRight(Local0, 1, Local0)
      // If(LOr(LEqual(OSFL, 1), LEqual(OSFL, 2))) {  // ??? Win9x
      // FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Max Sx
      // } Else {           // ??? Win2k / XP
     FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Min Sx
      // }
    }

    Return(PRWP)
  }
}

Scope (\_SB)
{
  Name(OSCI, 0)  // \_SB._OSC DWORD2 input
  Name(OSCO, 0)  // \_SB._OSC DWORD2 output
  Name(OSCP, 0)  // \_SB._OSC CAPABILITIES
  // _OSC (Operating System Capabilities)
  //    _OSC under \_SB scope is used to convey platform wide OSPM capabilities.
  //    For a complete description of _OSC ACPI Control Method, refer to ACPI 5.0
  //    specification, section 6.2.10.
  // Arguments: (4)
  //    Arg0 - A Buffer containing the UUID "0811B06E-4A27-44F9-8D60-3CBBC22E7B48"
  //    Arg1 - An Integer containing the Revision ID of the buffer format
  //    Arg2 - An Integer containing a count of entries in Arg3
  //    Arg3 - A Buffer containing a list of DWORD capabilities
  // Return Value:
  //    A Buffer containing the list of capabilities
  //
  Method(_OSC,4,Serialized)
  {
    //
    // Point to Status DWORD in the Arg3 buffer (STATUS)
    //
    CreateDWordField(Arg3, 0, STS0)
    //
    // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg3, 4, CAP0)

    //
    // Check UUID
    //
    If (LEqual(Arg0,ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48"))) {
      //
      // Check Revision
      //
      If (LEqual(Arg1,One)) {
        Store(CAP0, OSCP)
        If (And(CAP0,0x04)) // Check _PR3 Support(BIT2)
        {
          Store(0x04, OSCO)
          If (LEqual(RTD3,0)) // Is RTD3 support disabled in Bios Setup?
          {
            // RTD3 is disabled via BIOS Setup.
            And(CAP0, 0x3B, CAP0) // Clear _PR3 capability
            Or(STS0, 0x10, STS0) // Indicate capability bit is cleared
          }
        }
      } Else{
        And(STS0,0xFFFFFF00,STS0)
        Or(STS0,0xA, STS0) // Unrecognised Revision and report OSC failure
      }
    } Else {
      And(STS0,0xFFFFFF00,STS0)
      Or (STS0,0x6, STS0) // Unrecognised UUID and report OSC failure
    }

    Return(Arg3)
  } // End _OSC
}

