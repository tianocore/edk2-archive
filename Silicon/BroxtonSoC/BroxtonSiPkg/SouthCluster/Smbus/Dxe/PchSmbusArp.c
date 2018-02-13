/** @file
  PCH SMBus Driver, ARP functions.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "PchSmbus.h"

///
/// These addresses are reserved by the SMBus 2.0 specification
///
static UINT8  mReservedAddress[SMBUS_NUM_RESERVED] = {
  0x00,
  0x02,
  0x04,
  0x06,
  0x08,
  0x0A,
  0x0C,
  0x0E,
  0x10,
  0x18,
  0x50,
  0x6E,
  0xC2,
  0xF0,
  0xF2,
  0xF4,
  0xF6,
  0xF8,
  0xFA,
  0xFC,
  0xFE,
  0x12,
  0x14,
  0x16,
  0x58,
  0x5A,
  0x80,
  0x82,
  0x84,
  0x86,
  0x88,
  0x90,
  0x92,
  0x94,
  0x96,
  0x1A,
  0x1C,
  0x1E
};

/**
  Set Slave address for an SMBus device with a known UDID or perform a general
  ARP of all devices.

  @param[in] This                 Pointer to the instance of the EFI_SMBUS_HC_PROTOCOL.
  @param[in] ArpAll               If TRUE, do a full ARP. Otherwise, just ARP the specified UDID.
  @param[in] SmbusUdid            When doing a directed ARP, ARP the device with this UDID.
  @@param[in, out] SlaveAddress   Buffer to store new Slave Address during directed ARP. On output,If
                                  ArpAlll == TRUE, this will contain the newly assigned Slave address.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   ArpAll == FALSE but SmbusUdid or SlaveAddress are NULL.
  @retval Others                  Return value from SmbusFullArp() or SmbusDirectedArp().
**/
EFI_STATUS
EFIAPI
SmbusArpDevice (
  IN      CONST EFI_SMBUS_HC_PROTOCOL   *This,
  IN      BOOLEAN                       ArpAll,
  IN      EFI_SMBUS_UDID                * SmbusUdid, OPTIONAL
  IN OUT  EFI_SMBUS_DEVICE_ADDRESS      * SlaveAddress OPTIONAL
  )
{
  DEBUG ((DEBUG_INFO, "SmbusArpDevice() Start\n"));

  InitializeSmbusRegisters ();

  DEBUG ((DEBUG_INFO, "SmbusArpDevice() End\n"));

  if (ArpAll) {
    return SmbusFullArp (mSmbusContext);
  } else {
    if ((SmbusUdid == NULL) || (SlaveAddress == NULL)) {
      return EFI_INVALID_PARAMETER;
    }

    return SmbusDirectedArp (mSmbusContext, SmbusUdid, SlaveAddress);
  }
}

/**
  Get a pointer to the assigned mappings of UDID's to Slave Addresses.

  @param[in] This                 Pointer to the instance of the EFI_SMBUS_HC_PROTOCOL.
  @param[in, out] Length          Buffer to contain the lenght of the Device Map, it will be updated to
                                  contain the number of pairs of UDID's mapped to Slave Addresses.
  @param[in, out] SmbusDeviceMap  Buffer to contian a pointer to the Device Map, it will be updated to
                                  point to the first pair in the Device Map

  @retval EFI_SUCCESS             Function completed successfully.
**/
EFI_STATUS
EFIAPI
SmbusGetArpMap (
  IN      CONST EFI_SMBUS_HC_PROTOCOL   *This,
  IN OUT  UINTN                         *Length,
  IN OUT  EFI_SMBUS_DEVICE_MAP          **SmbusDeviceMap
  )
{
  *Length         = mSmbusContext->DeviceMapEntries * sizeof (EFI_SMBUS_DEVICE_MAP);
  *SmbusDeviceMap = mSmbusContext->DeviceMap;
  return EFI_SUCCESS;
}

/**
  Register a callback in the event of a Host Notify command being sent by a
  specified Slave Device.

  @param[in] This                 Pointer to the instance of the EFI_SMBUS_HC_PROTOCOL.
  @param[in] SlaveAddress         Address of the device whose Host Notify command we want to
                                  trap.
  @param[in] Data                 Data of the Host Notify command we want to trap.
  @param[in] NotifyFunction       Function to be called in the event the desired Host Notify
                                  command occurs.

  @retval EFI_UNSUPPORTED         Unable to create the event needed for notifications.
  @retval EFI_INVALID_PARAMETER   NotifyFunction was NULL.
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate space to register the notification.
  @retval EFI_SUCCESS             Function completed successfully
**/
EFI_STATUS
EFIAPI
SmbusNotify (
  IN      CONST EFI_SMBUS_HC_PROTOCOL   *This,
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN      UINTN                         Data,
  IN      EFI_SMBUS_NOTIFY_FUNCTION     NotifyFunction
  )
{
  EFI_STATUS                      Status;
  SMBUS_NOTIFY_FUNCTION_LIST_NODE *NewNode;

  DEBUG ((DEBUG_INFO, "SmbusNotify() Start\n"));

  if (NotifyFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NewNode = (SMBUS_NOTIFY_FUNCTION_LIST_NODE *) AllocatePool (sizeof (SMBUS_NOTIFY_FUNCTION_LIST_NODE));
  if (NewNode == NULL) {
    DEBUG ((DEBUG_ERROR, "Failed to allocate memory for NewNode! \n"));
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // If this is the first notification request, start an event to periodically
  // check for a Notify master command.
  //
  if (!mSmbusContext->NotificationEvent) {
    Status = InitializePeriodicEvent ();
    if (EFI_ERROR (Status)) {
      FreePool (NewNode);
      return EFI_UNSUPPORTED;
    }
  }

  NewNode->Signature                        = PCH_SMBUS_PRIVATE_DATA_SIGNATURE;
  NewNode->SlaveAddress.SmbusDeviceAddress  = SlaveAddress.SmbusDeviceAddress;
  NewNode->Data = Data;
  NewNode->NotifyFunction = NotifyFunction;

  InsertTailList (&mSmbusContext->NotifyFunctionList, &NewNode->Link);

  DEBUG ((DEBUG_INFO, "SmbusNotify() End\n"));
  return EFI_SUCCESS;
}

/**
  Set up a periodic event so that we can check if any Slave Device has sent a
  Notify ARP Master command.

  @retval EFI_SUCCESS             Periodic event successfully set up.
  @retval Others                  Failed to set up Periodic event.
                                  Error value from CreateEvent().
                                  Error value from SetTimer().
**/
EFI_STATUS
InitializePeriodicEvent (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = gBS->CreateEvent (
                  (EVT_TIMER | EVT_NOTIFY_SIGNAL),
                  TPL_CALLBACK,
                  PollSmbusNotify,
                  NULL,
                  &mSmbusContext->NotificationEvent
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->SetTimer (
                  mSmbusContext->NotificationEvent,
                  TimerPeriodic,
                  1000 * MILLISECOND
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Function to be called every time periodic event happens. This will check if
  the SMBus Host Controller has received a Host Notify command. If so, it will
  see if a notification has been reqested on that event and make any callbacks
  that may be necessary.

  @param[in] Event                The periodic event that occured and got us into this callback.
  @param[in] Context              Event context. Will be NULL in this case, since we already have our
                                  private data in a module global variable.
**/
VOID
EFIAPI
PollSmbusNotify (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *Context
  )
{
  LIST_ENTRY                      *Link;
  EFI_SMBUS_DEVICE_ADDRESS        SlaveAddress;
  SMBUS_NOTIFY_FUNCTION_LIST_NODE *Node;
  UINT8                           SstsReg;
  UINTN                           Data;

  DEBUG ((DEBUG_INFO, "PollSmbusNotify() Start\n"));

  InitializeSmbusRegisters ();

  SstsReg = SmbusIoRead (R_SMBUS_SSTS);
  if (!(SstsReg & B_SMBUS_HOST_NOTIFY_STS)) {
    //
    // Host Notify has not been received
    //
    return;
  }
  
  //
  // There was a Host Notify, see if any one wants to know about it
  //
  SlaveAddress.SmbusDeviceAddress = (SmbusIoRead (R_SMBUS_NDA)) >> 1;

  Link = GetFirstNode (&mSmbusContext->NotifyFunctionList);

  while (!IsNull (&mSmbusContext->NotifyFunctionList, Link)) {
    Node = SMBUS_NOTIFY_FUNCTION_LIST_NODE_FROM_LINK (Link);

    if (Node->SlaveAddress.SmbusDeviceAddress == SlaveAddress.SmbusDeviceAddress) {
      Data = (SmbusIoRead (R_SMBUS_NDHB) << 8) + (SmbusIoRead (R_SMBUS_NDLB));
      if ((UINT16) Node->Data == (UINT16) Data) {
        //
        // We have a match, notify the requested function
        //
        Node->NotifyFunction (SlaveAddress, Data);
      }
    }

    Link = GetNextNode (&mSmbusContext->NotifyFunctionList, &Node->Link);
  }
  
  //
  // Clear the Notify Status bit and exit.
  //
  SmbusIoWrite (R_SMBUS_SSTS, B_SMBUS_HOST_NOTIFY_STS);

  DEBUG ((DEBUG_INFO, "PollSmbusNotify() End\n"));

  return;
}

/**
  Issue a prepare ARP command to informs all devices that the ARP Master is starting the ARP process

  @param[in] Private              Pointer to the SMBUS_INSTANCE

  @retval EFI_SUCCESS             The SMBUS operation is successful
  @retval Others                  Something error occurred
**/
EFI_STATUS
SmbusPrepareToArp (
  IN  SMBUS_INSTANCE        *Private
  )
{
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  EFI_STATUS                Status;
  UINTN                     Length;
  UINT8                     Buffer;

  DEBUG ((DEBUG_INFO, "SmbusPrepareToArp() Start\n"));

  SlaveAddress.SmbusDeviceAddress = SMBUS_ADDRESS_ARP;
  Length                          = 1;
  Buffer                          = SMBUS_DATA_PREPARE_TO_ARP;

  Status = SmbusExec (
            SlaveAddress,
            0,
            EfiSmbusSendByte,
            TRUE,
            &Length,
            &Buffer
            );

  DEBUG ((DEBUG_INFO, "SmbusPrepareToArp() End\n"));

  return Status;
}

/**
  Issue a Get UDID (general) command to requests ARP-capable and/or Discoverable devices to
  return their slave address along with their UDID.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in, out] DeviceMap       Pointer to SMBUS device map table that slave device return

  @retval EFI_SUCCESS             The SMBUS operation is successful
  @retval Others                  Something error occurred
**/
EFI_STATUS
SmbusGetUdidGeneral (
  IN      SMBUS_INSTANCE        *Private,
  IN OUT  EFI_SMBUS_DEVICE_MAP  *DeviceMap
  )
{
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  EFI_STATUS                Status;
  UINTN                     Length;
  UINT8                     Buffer[SMBUS_GET_UDID_LENGTH];

  DEBUG ((DEBUG_INFO, "SmbusGetUdidGeneral() Start\n"));

  SlaveAddress.SmbusDeviceAddress = SMBUS_ADDRESS_ARP;
  Length                          = SMBUS_GET_UDID_LENGTH;

  Status = SmbusExec (
            SlaveAddress,
            SMBUS_DATA_GET_UDID_GENERAL,
            EfiSmbusReadBlock,
            TRUE,
            &Length,
            Buffer
            );

  if (!EFI_ERROR (Status)) {
    if (Length == SMBUS_GET_UDID_LENGTH) {
      DeviceMap->SmbusDeviceUdid.DeviceCapabilities = Buffer[0];
      DeviceMap->SmbusDeviceUdid.VendorRevision     = Buffer[1];
      DeviceMap->SmbusDeviceUdid.VendorId           = (UINT16) ((Buffer[2] << 8) + Buffer[3]);
      DeviceMap->SmbusDeviceUdid.DeviceId           = (UINT16) ((Buffer[4] << 8) + Buffer[5]);
      DeviceMap->SmbusDeviceUdid.Interface          = (UINT16) ((Buffer[6] << 8) + Buffer[7]);
      DeviceMap->SmbusDeviceUdid.SubsystemVendorId  = (UINT16) ((Buffer[8] << 8) + Buffer[9]);
      DeviceMap->SmbusDeviceUdid.SubsystemDeviceId  = (UINT16) ((Buffer[10] << 8) + Buffer[11]);
      DeviceMap->SmbusDeviceUdid.VendorSpecificId = (UINT32) ((Buffer[12] << 24) + (Buffer[13] << 16) + (Buffer[14] << 8) + Buffer[15]);
      DeviceMap->SmbusDeviceAddress.SmbusDeviceAddress = (UINT8) (Buffer[16] >> 1);
    } else {
      Status = EFI_DEVICE_ERROR;
    }
  }

  DEBUG ((DEBUG_INFO, "SmbusGetUdidGeneral() End\n"));

  return Status;
}

/**
  Issue a Assign address command to assigns an address to a specific slave device.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in, out] DeviceMap       Pointer to SMBus device map table that send to slave device

  @retval EFI_SUCCESS             The SMBus operation is successful
  @retval Others                  Something error occurred
**/
EFI_STATUS
SmbusAssignAddress (
  IN      SMBUS_INSTANCE        *Private,
  IN OUT  EFI_SMBUS_DEVICE_MAP  *DeviceMap
  )
{
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  EFI_STATUS                Status;
  UINTN                     Length;
  UINT8                     Buffer[SMBUS_GET_UDID_LENGTH];

  DEBUG ((DEBUG_INFO, "SmbusAssignAddress() Start\n"));

  Buffer[0] = DeviceMap->SmbusDeviceUdid.DeviceCapabilities;
  Buffer[1] = DeviceMap->SmbusDeviceUdid.VendorRevision;
  Buffer[2] = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorId >> 8);
  Buffer[3] = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorId);
  Buffer[4] = (UINT8) (DeviceMap->SmbusDeviceUdid.DeviceId >> 8);
  Buffer[5] = (UINT8) (DeviceMap->SmbusDeviceUdid.DeviceId);
  Buffer[6] = (UINT8) (DeviceMap->SmbusDeviceUdid.Interface >> 8);
  Buffer[7] = (UINT8) (DeviceMap->SmbusDeviceUdid.Interface);
  Buffer[8] = (UINT8) (DeviceMap->SmbusDeviceUdid.SubsystemVendorId >> 8);
  Buffer[9] = (UINT8) (DeviceMap->SmbusDeviceUdid.SubsystemVendorId);
  Buffer[10] = (UINT8) (DeviceMap->SmbusDeviceUdid.SubsystemDeviceId >> 8);
  Buffer[11] = (UINT8) (DeviceMap->SmbusDeviceUdid.SubsystemDeviceId);
  Buffer[12] = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorSpecificId >> 24);
  Buffer[13] = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorSpecificId >> 16);
  Buffer[14] = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorSpecificId >> 8);
  Buffer[15] = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorSpecificId);
  Buffer[16] = (UINT8) (DeviceMap->SmbusDeviceAddress.SmbusDeviceAddress << 1);

  SlaveAddress.SmbusDeviceAddress = SMBUS_ADDRESS_ARP;
  Length                          = SMBUS_GET_UDID_LENGTH;

  Status = SmbusExec (
            SlaveAddress,
            SMBUS_DATA_ASSIGN_ADDRESS,
            EfiSmbusWriteBlock,
            TRUE,
            &Length,
            Buffer
            );

  DEBUG ((DEBUG_INFO, "SmbusAssignAddress() End\n"));

  return Status;
}

/**
  Do a fully (general) Arp procress to assign the slave address of all ARP-capable device.
  This function will issue issue the "Prepare to ARP", "Get UDID" and "Assign Address" commands.

  @param[in] Private              Pointer to the SMBUS_INSTANCE

  @retval EFI_OUT_OF_RESOURCES    No available address to assign
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusFullArp (
  IN      SMBUS_INSTANCE  *Private
  )
{
  EFI_STATUS            Status;
  EFI_SMBUS_DEVICE_MAP  *CurrentDeviceMap;

  DEBUG ((DEBUG_INFO, "SmbusFullArp() Start\n"));

  Status = SmbusPrepareToArp (Private);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_DEVICE_ERROR) {
      //
      //  ARP is complete
      //
      return EFI_SUCCESS;
    } else {
      return Status;
    }
  }
  //
  //  Main loop to ARP all ARP-capable devices
  //
  do {
    CurrentDeviceMap  = &Private->DeviceMap[Private->DeviceMapEntries];
    Status            = SmbusGetUdidGeneral (Private, CurrentDeviceMap);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (CurrentDeviceMap->SmbusDeviceAddress.SmbusDeviceAddress == (0xFF >> 1)) {
      //
      // If address is unassigned, assign it
      //
      Status = GetNextAvailableAddress (
                Private,
                &CurrentDeviceMap->SmbusDeviceAddress
                );
      if (EFI_ERROR (Status)) {
        return EFI_OUT_OF_RESOURCES;
      }
    } else if (((CurrentDeviceMap->SmbusDeviceUdid.DeviceCapabilities) & 0xC0) != 0) {
      //
      // if address is not fixed, check if the current address is available
      //
      if (!IsAddressAvailable (
            Private,
            CurrentDeviceMap->SmbusDeviceAddress
            )) {
        //
        // if currently assigned address is already used, get a new one
        //
        Status = GetNextAvailableAddress (
                  Private,
                  &CurrentDeviceMap->SmbusDeviceAddress
                  );
        if (EFI_ERROR (Status)) {
          return EFI_OUT_OF_RESOURCES;
        }
      }
    }

    Status = SmbusAssignAddress (Private, CurrentDeviceMap);
    if (EFI_ERROR (Status)) {
      //
      // If there was a device error, just continue on and try again.
      // Other errors should be reported.
      //
      if (Status != EFI_DEVICE_ERROR) {
        return Status;
      }
    } else {
      //
      // If there was no error, the address was assigned and we must update our
      // records.
      //
      Private->DeviceMapEntries++;
    }

  } while (Private->DeviceMapEntries < MAX_SMBUS_DEVICES);

  DEBUG ((DEBUG_INFO, "SmbusFullArp() End\n"));

  return EFI_SUCCESS;
}

/**
  Do a directed Arp procress to assign the slave address of a single ARP-capable device.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in] SmbusUdid            When doing a directed ARP, ARP the device with this UDID.
  @param[in, out] SlaveAddress    Buffer to store new Slave Address during directed ARP.

  @retval EFI_OUT_OF_RESOURCES    DeviceMapEntries is more than Max number of SMBus devices
                                  Or there is no available address to assign
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusDirectedArp (
  IN      SMBUS_INSTANCE            *Private,
  IN      EFI_SMBUS_UDID            *SmbusUdid,
  IN OUT  EFI_SMBUS_DEVICE_ADDRESS  *SlaveAddress
  )
{
  EFI_STATUS            Status;
  EFI_SMBUS_DEVICE_MAP  *CurrentDeviceMap;

  DEBUG ((DEBUG_INFO, "SmbusDirectedArp() Start\n"));

  if (Private->DeviceMapEntries >= MAX_SMBUS_DEVICES) {
    return EFI_OUT_OF_RESOURCES;
  }

  CurrentDeviceMap = &Private->DeviceMap[Private->DeviceMapEntries];

  //
  // Find an available address to assign
  //
  Status = GetNextAvailableAddress (
            Private,
            &CurrentDeviceMap->SmbusDeviceAddress
            );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  CurrentDeviceMap->SmbusDeviceUdid.DeviceCapabilities  = SmbusUdid->DeviceCapabilities;
  CurrentDeviceMap->SmbusDeviceUdid.DeviceId            = SmbusUdid->DeviceId;
  CurrentDeviceMap->SmbusDeviceUdid.Interface           = SmbusUdid->Interface;
  CurrentDeviceMap->SmbusDeviceUdid.SubsystemDeviceId   = SmbusUdid->SubsystemDeviceId;
  CurrentDeviceMap->SmbusDeviceUdid.SubsystemVendorId   = SmbusUdid->SubsystemVendorId;
  CurrentDeviceMap->SmbusDeviceUdid.VendorId            = SmbusUdid->VendorId;
  CurrentDeviceMap->SmbusDeviceUdid.VendorRevision      = SmbusUdid->VendorRevision;
  CurrentDeviceMap->SmbusDeviceUdid.VendorSpecificId    = SmbusUdid->VendorSpecificId;

  Status = SmbusAssignAddress (Private, CurrentDeviceMap);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Private->DeviceMapEntries++;
  SlaveAddress->SmbusDeviceAddress = CurrentDeviceMap->SmbusDeviceAddress.SmbusDeviceAddress;

  DEBUG ((DEBUG_INFO, "SmbusDirectedArp() End\n"));

  return EFI_SUCCESS;
}

/**
  Find an available address to assign

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in, out] SlaveAddress    Buffer to store new Slave Address during directed ARP.

  @retval EFI_OUT_OF_RESOURCES    There is no available address to assign
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
GetNextAvailableAddress (
  IN     SMBUS_INSTANCE            *Private,
  IN OUT EFI_SMBUS_DEVICE_ADDRESS  *SlaveAddress
  )
{
  for (SlaveAddress->SmbusDeviceAddress = 0x03;
       SlaveAddress->SmbusDeviceAddress < 0x7F;
       SlaveAddress->SmbusDeviceAddress++
      ) {
    if (IsAddressAvailable (Private, *SlaveAddress)) {
      return EFI_SUCCESS;
    }
  }

  return EFI_OUT_OF_RESOURCES;
}

/**
  Check whether the address is assignable.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in] SlaveAddress         The Slave Address for checking

  @retval TRUE                    The address is assignable
  @retval FALSE                   The address is not assignable
**/
BOOLEAN
IsAddressAvailable (
  IN SMBUS_INSTANCE              *Private,
  IN EFI_SMBUS_DEVICE_ADDRESS    SlaveAddress
  )
{
  UINT8 Index;

  //
  // See if we have already assigned this address to a device
  //
  for (Index = 0; Index < Private->DeviceMapEntries; Index++) {
    if (SlaveAddress.SmbusDeviceAddress == Private->DeviceMap[Index].SmbusDeviceAddress.SmbusDeviceAddress) {
      return FALSE;
    }
  }
  //
  // See if this address is claimed by a platform non-ARP-capable device
  //
  for (Index = 0; Index < Private->PlatformNumRsvd; Index++) {
    if ((SlaveAddress.SmbusDeviceAddress << 1) == Private->PlatformRsvdAddr[Index]) {
      return FALSE;
    }
  }
  //
  // See if this is a reserved address
  //
  for (Index = 0; Index < SMBUS_NUM_RESERVED; Index++) {
    if ((SlaveAddress.SmbusDeviceAddress << 1) == (UINTN) mReservedAddress[Index]) {
      return FALSE;
    }
  }

  return TRUE;
}
