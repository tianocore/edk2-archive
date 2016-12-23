/** @file
  Boot service DXE ASL update library implementation.
  These functions in this file can be called during DXE and cannot be called during runtime
  or in SMM which should use a RT or SMM library.
  This library uses the ACPI Support protocol.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ScAslUpdateLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

//
// Function implementations
//
static EFI_ACPI_SUPPORT_PROTOCOL  *mAcpiSupport = NULL;
static EFI_ACPI_TABLE_PROTOCOL    *mAcpiTable   = NULL;


/**
  Initialize the ASL update library state.
  This must be called prior to invoking other library functions.

  @param[in] None

  @retval    EFI_SUCCESS                   The function completed successfully.

**/
EFI_STATUS
InitializeScAslUpdateLib (
  VOID
  )
{
  EFI_STATUS  Status;

  //
  // Locate ACPI tables
  //
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **) &mAcpiSupport);
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &mAcpiTable);
  return Status;
}


/**
  This procedure will update immediate value assigned to a Name

  @param[in] AslSignature                   The signature of Operation Region that we want to update.
  @param[in] Buffer                         source of data to be written over original aml
  @param[in] Length                         length of data to be overwritten

  @retval    EFI_SUCCESS                    The function completed successfully.

**/
EFI_STATUS
UpdateNameAslCode (
  IN     UINT32                        AslSignature,
  IN     VOID                          *Buffer,
  IN     UINTN                         Length
  )
{
  EFI_STATUS                  Status;
  EFI_ACPI_DESCRIPTION_HEADER *Table;
  EFI_ACPI_TABLE_VERSION      Version;
  UINT8                       *CurrPtr;
  UINT32                      *Signature;
  UINT8                       *DsdtPointer;
  UINT8                       Index;
  UINTN                       Handle;
  UINT8                       DataSize;

  //
  // Locate table with matching ID
  //
  Index = 0;

  do {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index, (VOID **) &Table, &Version, &Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    ASSERT_EFI_ERROR (Status);
    Index++;
  } while (Table->Signature != EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE);

  //
  // Point to the beginning of the DSDT table
  //
  Index = 0;
  CurrPtr = (UINT8 *) Table;

  //
  // Loop through the ASL looking for values that we must fix up.
  //
  for (DsdtPointer = CurrPtr; DsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length); DsdtPointer++) {
    //
    // Get a pointer to compare for signature
    //
    Signature = (UINT32 *) DsdtPointer;
    //
    // Check if this is the Device Object signature we are looking for
    //
    if ((*Signature) == AslSignature) {
      //
      // Look for Name Encoding
      //
      if (*(DsdtPointer - 1) == AML_NAME_OP) {
        //
        // Check if size of new and old data is the same
        //
        DataSize = *(DsdtPointer + 4);
        if ((Length == 1 && DataSize == 0xA) ||
            (Length == 2 && DataSize == 0xB) ||
            (Length == 4 && DataSize == 0xC) ) {
          CopyMem (DsdtPointer + 5, Buffer, Length);
        }  else if (Length == 1 && ((*(UINT8 *)Buffer) == 0 || (*(UINT8 *)Buffer) == 1) && (DataSize == 0 || DataSize == 1)) {
          CopyMem (DsdtPointer + 4, Buffer, Length);
        } else {
          FreePool (Table);
          return EFI_BAD_BUFFER_SIZE;
        }
        Status = mAcpiTable->UninstallAcpiTable (
                               mAcpiTable,
                               Handle
                               );
        Handle = 0;
        Status = mAcpiTable->InstallAcpiTable (
                               mAcpiTable,
                               Table,
                               Table->Length,
                               &Handle
                               );
        FreePool (Table);
        return Status;
      }
    }
  }
  return EFI_NOT_FOUND;
}


/**
  This procedure will update a Resource Descriptor Macro in
  Resrouce Template buffer list.

  @param[in] AslSignature        The signature of Operation Region that we want to update.
  @param[in] BufferName          signature of the Buffer inside OpRegion that we want to update
  @param[in] MacroAmlEncoding    type of entry inside Buffer.
  @param[in] MacroEntryNumber    number of entry of the above type
  @param[in] Offset              offset (in bytes) inside entry where update will be performed
  @param[in] Buffer              source of data to be written over original aml
  @param[in] Length              length of data to be overwritten

  @retval    EFI_SUCCESS         The function completed successfully.

**/
EFI_STATUS
UpdateResourceTemplateAslCode (
  IN     UINT32                        AslSignature,
  IN     UINT32                        BufferName,
  IN     UINT8                         MacroAmlEncoding,
  IN     UINT8                         MacroEntryNumber,
  IN     UINT8                         Offset,
  IN     VOID                          *Buffer,
  IN     UINTN                         Length
  )
{
  EFI_STATUS                           Status;
  EFI_ACPI_DESCRIPTION_HEADER          *Table;
  EFI_ACPI_TABLE_VERSION               Version;
  UINT8                                *CurrPtr;
  UINT8                                *Operation;
  UINT32                               *Signature;
  UINT8                                *DsdtPointer;
  UINT8                                Index;
  UINT8                                *BufferLength;
  UINTN                                Handle;
  UINT16                               AslLength;
  BOOLEAN                              EntryFound;

  //
  // Locate table with matching ID
  //
  Index     = 0;
  AslLength = 0;
  EntryFound = FALSE;

  do {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index,  (VOID **) &Table, &Version, &Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }
    ASSERT_EFI_ERROR (Status);
    Index ++;
  } while (Table->Signature != EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE);

  //
  // Point to the beginning of the DSDT table
  //
  Index = 0;
  CurrPtr = (UINT8 *) Table;

  //
  // Loop through the ASL looking for values that we must fix up.
  //
  for (DsdtPointer = CurrPtr; DsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length); DsdtPointer++) {
    //
    // Get a pointer to compare for signature
    //
    Signature = (UINT32 *) DsdtPointer;

    //
    // Check if this is the Device Object signature we are looking for
    //
    if ((*Signature) == AslSignature) {
      //
      // Read the Device Object block length
      //
      if (*(DsdtPointer - 2) == AML_DEVICE_OP) {
        AslLength = *(DsdtPointer - 1);
      } else if (*(DsdtPointer - 3) == AML_DEVICE_OP) {
        AslLength = *(UINT16 *) (DsdtPointer - 2);
        AslLength = (AslLength & 0x0F) + ((AslLength & 0x0FF00) >> 4);
      }

      //
      // Conditional match.  Search AML Encoding in Device.
      //
      for (Operation = DsdtPointer; Operation <= DsdtPointer + AslLength; Operation++) {
        //
        // Look for Name Encoding
        //
        while (Operation <= DsdtPointer + AslLength) {
          if (*Operation == AML_NAME_OP) {
            //
            // Found Name AML Encoding
            //
             Operation ++;
             if (*(UINT32 *) (Operation) == BufferName) {
               //
               // Found RBUF Resource Template object name
               //
               break;
             }
           }
           Operation++;
        }

        if (Operation > DsdtPointer + AslLength ) {
           FreePool (Table);
           return EFI_NOT_FOUND;
        }

        //
        // Now look for the Resource Template Object buffer opcode
        //
        while ((*Operation) != AML_BUFFER_OP) {
          Operation++;
          if (Operation > DsdtPointer + AslLength) {
            FreePool (Table);
            return EFI_NOT_FOUND;
          }
        }

        //
        // In a Resource Template Object the length of buffer
        // list is 3 bytes after the buffer opcode
        //
        Operation += 3;
        BufferLength = (*Operation) + Operation + 1;

        //
        // Now look for the Macro to be updated
        //
        while (Operation <= BufferLength) {
          if ((*Operation == MacroAmlEncoding)) {
            //
            // We found a matching encoding however, the buffer list may have "n" number
            // of same encoding entries. Let's narrow down to the "n"th entry.
            //
            Index++;
            if (Index == MacroEntryNumber) {
              //
              // Get to the starting offset & end offset
              //
              Operation += Offset;

              //
              // Fixup the value at the offset
              //
              CopyMem ((VOID *) Operation, (VOID *) (Buffer), Length);

              //
              // Update the modified ACPI table
              //
              Status = mAcpiTable->UninstallAcpiTable (
                                     mAcpiTable,
                                     Handle
                                     );
              Handle = 0;
              Status = mAcpiTable->InstallAcpiTable (
                                     mAcpiTable,
                                     Table,
                                     Table->Length,
                                     &Handle
                                     );
              FreePool (Table);
              return Status;
            }
          }
          Operation++;
        }

        if (Operation > DsdtPointer + AslLength) {
          FreePool (Table);
          return EFI_NOT_FOUND;
        }
      }
    }
  }

  return EFI_NOT_FOUND;
}


/**
  This function uses the ACPI support protocol to locate an ACPI table.
  It is really only useful for finding tables that only have a single instance,
  e.g. FADT, FACS, MADT, etc.  It is not good for locating SSDT, etc.

  @param[in]      Signature          Pointer to an ASCII string containing the OEM Table ID from the ACPI table header
  @param[in, out] Table              Updated with a pointer to the table
  @param[in, out] Handle             AcpiSupport protocol table handle for the table found
  @param[in, out] Version            The version of the table desired

  @retval         EFI_SUCCESS        The function completed successfully.

**/
EFI_STATUS
LocateAcpiTableBySignature (
  IN      UINT32                        Signature,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle,
  IN OUT  EFI_ACPI_TABLE_VERSION        *Version
  )
{
  EFI_STATUS              Status;
  INTN                    Index;
  EFI_ACPI_TABLE_VERSION  DesiredVersion;

  DesiredVersion = *Version;

  //
  // Locate table with matching ID
  //
  Index = 0;
  do {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index,  (VOID **) Table, Version, Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    ASSERT_EFI_ERROR (Status);
    Index++;
  } while ((*Table)->Signature != Signature || !(*Version & DesiredVersion));

  //
  // If we found the table, there will be no error.
  //
  return Status;
}


/**
  This function uses the ACPI support protocol to locate an ACPI SSDT table.

  @param[in]      TableId             Pointer to an ASCII string containing the OEM Table ID from the ACPI table header
  @param[in]      TableIdSize         Length of the TableId to match.  Table ID are 8 bytes long, this function
                                      will consider it a match if the first TableIdSize bytes match
  @param[in, out] Table               Updated with a pointer to the table
  @param[in, out] Handle              AcpiSupport protocol table handle for the table found
  @param[in, out] Version             See AcpiSupport protocol, GetAcpiTable function for use

  @retval         EFI_SUCCESS         The function completed successfully.

**/
EFI_STATUS
LocateAcpiTableByOemTableId (
  IN      UINT8                         *TableId,
  IN      UINT8                         TableIdSize,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle,
  IN OUT  EFI_ACPI_TABLE_VERSION        *Version
  )
{
  EFI_STATUS  Status;
  INTN        Index;

  //
  // Locate table with matching ID
  //
  Index = 0;
  do {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index, (VOID **) Table, Version, Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    ASSERT_EFI_ERROR (Status);
    Index++;
  } while (CompareMem (&(*Table)->OemTableId, TableId, TableIdSize));

  //
  // If we found the table, there will be no error.
  //
  return Status;
}


/**
  This function calculates and updates an UINT8 checksum.

  @param[in] Buffer                  Pointer to buffer to checksum
  @param[in] Size                    Number of bytes to checksum
  @param[in] ChecksumOffset          Offset to place the checksum result in

  @retval    EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
AcpiChecksum (
  IN VOID       *Buffer,
  IN UINTN      Size,
  IN UINTN      ChecksumOffset
  )
{
  UINT8    Sum;
  UINT8    *Ptr;

  Sum = 0;
  //
  // Initialize pointer
  //
  Ptr = Buffer;

  //
  // set checksum to 0 first
  //
  Ptr[ChecksumOffset] = 0;

  //
  // add all content of buffer
  //
  while (Size--) {
    Sum = (UINT8) (Sum + (*Ptr++));
  }
  //
  // set checksum
  //
  Ptr                 = Buffer;
  Ptr[ChecksumOffset] = (UINT8) (0xff - Sum + 1);

  return EFI_SUCCESS;
}

