/** @file
  This file describes WSMT table.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

typedef struct {
  UINT32  Signature;                   //0
  UINT32  Length;                      //4
  UINT8   Revision;                    //8
  UINT8   Checksum;                    //9
  UINT8   OEMID[6];                    //10
  UINT8   OEMTableID[8];               //16
  UINT32  OEMRevision;                 //24
  UINT32  CreatorID;                   //28
  UINT32  CreatorRevision;             //32
  UINT32  ProtectionFlags;             //36
} ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_STRUCTURE;

#define ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_STRUCTURE_SIGNATURE  SIGNATURE_32('W', 'S', 'M', 'T')

//
//note: ProtectionFlags  , BIT0=FIXED_COMM_BUFFERS , BIT1=COMM_BUFFER_NESTED_PTR_PROTECTION , BIT2=SYSTEM_RESOURCE_PROTECTION
//

/*
FIXED_COMM_BUFFERS - Firmware setting this bit should refer to the SMM Communication ACPI Table defined in the UEFI 2.6 specification.
                     Firmware should also consider all other possible data exchanges between SMM and non-SMM,
                     including but not limited to EFI_SMM_COMMUNICATION_PROTOCOL, ACPINVS in ASL code,
                     general purpose registers as buffer pointers, etc.

COMM_BUFFER_NESTED_PTR_PROTECTION - Firmware setting this bit must also set the FIXED_COMM_BUFFERS bit.

SYSTEM_RESOURCE_PROTECTION - After ExitBootServices(), firmware setting this bit shall not allow any software to make changes
                             to the locations of IOMMU's, interrupt controllers, PCI Configuration Space,
                             the Firmware ACPI Control Structure (FACS), or any registers reported through ACPI fixed tables
                             (e.g. PMx Control registers, reset register, etc.).
                             This also includes disallowing changes to RAM layout and ensuring that decodes to RAM
                             and any system resources as described above take priority over software configurable registers.
                             For example, if software configures a PCI Express BAR to overlay RAM,
                             accesses by the CPU to the affected system physical addresses must decode to RAM.
*/

#define ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_PROTECTION_FLAGS              0x00000000
#define FIXED_COMM_BUFFERS                                                  BIT0
#define COMM_BUFFER_NESTED_PTR_PROTECTION                                   BIT1
#define SYSTEM_RESOURCE_PROTECTION                                          BIT2

