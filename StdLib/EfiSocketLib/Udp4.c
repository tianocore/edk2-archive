/** @file
  Implement the UDP4 driver support for the socket layer.

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


  \section Udp4ReceiveEngine UDPv4 Receive Engine

  The receive engine is started by calling ::EslUdp4RxStart when the
  ::ESL_PORT structure is configured and stopped when ::EslSocketPortCloseTxDone
  calls the UDPv4 configure operation to reset the port.  The receive engine
  consists of a single receive buffer that is posted to the UDPv4 driver.

  Upon receive completion, ::EslUdp4RxComplete posts the UDPv4 buffer to the
  ESL_SOCKET::pRxPacketListTail.  To minimize the number of buffer copies,
  the ::EslUdp4RxComplete routine queues the UDP4 driver's buffer to a list
  of datagrams waiting to be received.  The socket driver holds on to the
  buffers from the UDPv4 driver until the application layer requests
  the data or the socket is closed.

  When the application wants to receive data it indirectly calls
  ::EslUdp4Receive to remove data from the data queue.  This routine
  removes the next available datagram from ESL_SOCKET::pRxPacketListHead
  and copies the data from the UDPv4 driver's buffer into the
  application's buffer.  The UDPv4 driver's buffer is then returned.

  During socket layer shutdown, ::EslUdp4RxCancel is called by ::EslSocketShutdown
  to cancel the pending receive operations.

  Receive flow control is applied when the socket is created, since no receive
  operation is pending to the UDPv4 driver.  The flow control gets released
  when the port is configured.  Flow control remains in the released state,
  ::EslUdp4RxComplete calls ::EslUdp4RxStart until the maximum buffer space
  is consumed.  By not calling EslUdp4RxStart, EslUdp4RxComplete applies flow
  control.  Flow control is eventually released when the buffer space drops
  below the maximum amount and EslUdp4Receive calls EslUdp4RxStart.

**/

#include "Socket.h"

/**
  Interface between the socket layer and the network specific
  code that supports SOCK_DGRAM sockets over UDPv4.
**/
CONST ESL_PROTOCOL_API cEslUdp4Api = {
  IPPROTO_UDP,
  OFFSET_OF ( ESL_LAYER, pUdp4List ),
  OFFSET_OF ( struct sockaddr_in, sin_zero ),
  sizeof ( struct sockaddr_in ),
  AF_INET,
  NULL,   //  Accept
  NULL,   //  ConnectPoll
  NULL,   //  ConnectStart
  EslUdp4SocketIsConfigured,
  EslUdp4LocalAddressGet,
  EslUdp4LocalAddressSet,
  NULL,   //  Listen
  NULL,   //  OptionGet
  NULL,   //  OptionSet
  EslUdp4PortAllocate,
  EslUdp4PortClose,
  EslUdp4PortClosePacketFree,
  EslUdp4PortCloseRxStop,
  TRUE,
  EslUdp4Receive,
  EslUdp4RemoteAddressGet,
  EslUdp4RemoteAddressSet,
  EslUdp4RxCancel,
  EslUdp4TxBuffer,
  EslUdp4TxComplete,
  NULL    //  TxOobComplete
};


/**
  Get the local socket address

  This routine returns the IPv4 address and UDP port number associated
  with the local socket.

  This routine is called by ::EslSocketGetLocalAddress to determine the
  network address for the SOCK_DGRAM socket.

  @param [in] pPort       Address of an ::ESL_PORT structure.

  @param [out] pAddress   Network address to receive the local system address

**/
VOID
EslUdp4LocalAddressGet (
  IN ESL_PORT * pPort,
  OUT struct sockaddr * pAddress
  )
{
  struct sockaddr_in * pLocalAddress;
  ESL_UDP4_CONTEXT * pUdp4;

  DBG_ENTER ( );

  //
  //  Return the local address
  //
  pUdp4 = &pPort->Context.Udp4;
  pLocalAddress = (struct sockaddr_in *)pAddress;
  pLocalAddress->sin_family = AF_INET;
  pLocalAddress->sin_port = SwapBytes16 ( pUdp4->ConfigData.StationPort );
  CopyMem ( &pLocalAddress->sin_addr,
            &pUdp4->ConfigData.StationAddress.Addr[0],
            sizeof ( pLocalAddress->sin_addr ));

  DBG_EXIT ( );
}


/**
  Set the local port address.

  This routine sets the local port address.

  This support routine is called by ::EslSocketPortAllocate.

  @param [in] ppPort      Address of an ESL_PORT structure
  @param [in] pSockAddr   Address of a sockaddr structure that contains the
                          connection point on the local machine.  An IPv4 address
                          of INADDR_ANY specifies that the connection is made to
                          all of the network stacks on the platform.  Specifying a
                          specific IPv4 address restricts the connection to the
                          network stack supporting that address.  Specifying zero
                          for the port causes the network layer to assign a port
                          number from the dynamic range.  Specifying a specific
                          port number causes the network layer to use that port.

 **/
VOID
EslUdp4LocalAddressSet (
  IN ESL_PORT * pPort,
  IN CONST struct sockaddr * pSockAddr
  )
{
  EFI_UDP4_CONFIG_DATA * pConfig;
  CONST struct sockaddr_in * pIpAddress;
  CONST UINT8 * pIpv4Address;

  DBG_ENTER ( );

  //
  //  Set the local address
  //
  pIpAddress = (struct sockaddr_in *)pSockAddr;
  pIpv4Address = (UINT8 *)&pIpAddress->sin_addr.s_addr;
  pConfig = &pPort->Context.Udp4.ConfigData;
  pConfig->StationAddress.Addr[0] = pIpv4Address[0];
  pConfig->StationAddress.Addr[1] = pIpv4Address[1];
  pConfig->StationAddress.Addr[2] = pIpv4Address[2];
  pConfig->StationAddress.Addr[3] = pIpv4Address[3];

  //
  //  Determine if the default address is used
  //
  pConfig->UseDefaultAddress = (BOOLEAN)( 0 == pIpAddress->sin_addr.s_addr );

  //
  //  Set the port number
  //
  pConfig->StationPort = SwapBytes16 ( pIpAddress->sin_port );

  //
  //  Set the subnet mask
  //
  if ( pConfig->UseDefaultAddress ) {
    pConfig->SubnetMask.Addr[0] = 0;
    pConfig->SubnetMask.Addr[1] = 0;
    pConfig->SubnetMask.Addr[2] = 0;
    pConfig->SubnetMask.Addr[3] = 0;
  }
  else {
    pConfig->SubnetMask.Addr[0] = 0xff;
    pConfig->SubnetMask.Addr[1] = 0xff;
    pConfig->SubnetMask.Addr[2] = 0xff;
    pConfig->SubnetMask.Addr[3] = 0xff;
  }

  DBG_EXIT ( );
}


/**
  Initialize the network specific portions of an ::ESL_PORT structure.

  This routine initializes the network specific portions of an
  ::ESL_PORT structure for use by the socket.

  This support routine is called by ::EslSocketPortAllocate
  to connect the socket with the underlying network adapter
  running the UDPv4 protocol.

  @param [in] ppPort      Address of an ESL_PORT structure
  @param [in] DebugFlags  Flags for debug messages

  @retval EFI_SUCCESS - Socket successfully created

 **/
EFI_STATUS
EslUdp4PortAllocate (
  IN ESL_PORT * pPort,
  IN UINTN DebugFlags
  )
{
  EFI_UDP4_CONFIG_DATA * pConfig;
  ESL_UDP4_CONTEXT * pUdp4;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Use for/break instead of goto
  for ( ; ; ) {
    //
    //  Allocate the receive event
    //
    pSocket = pPort->pSocket;
    pUdp4 = &pPort->Context.Udp4;
    Status = gBS->CreateEvent (  EVT_NOTIFY_SIGNAL,
                                 TPL_SOCKETS,
                                 (EFI_EVENT_NOTIFY)EslUdp4RxComplete,
                                 pPort,
                                 &pUdp4->RxToken.Event);
    if ( EFI_ERROR ( Status )) {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to create the receive event, Status: %r\r\n",
                Status ));
      pSocket->errno = ENOMEM;
      break;
    }
    DEBUG (( DEBUG_RX | DEBUG_POOL,
              "0x%08x: Created receive event\r\n",
              pUdp4->RxToken.Event ));

    //
    //  Initialize the port
    //
    pSocket->TxPacketOffset = OFFSET_OF ( ESL_PACKET, Op.Udp4Tx.TxData );
    pSocket->TxTokenEventOffset = OFFSET_OF ( ESL_IO_MGMT, Token.Udp4Tx.Event );
    pSocket->TxTokenOffset = OFFSET_OF ( EFI_UDP4_COMPLETION_TOKEN, Packet.TxData );

    //
    //  Save the transmit address
    //
    pPort->pfnTxStart = (PFN_NET_TX_START)pPort->pProtocol.UDPv4->Transmit;

    //
    //  Set the configuration flags
    //
    pConfig = &pPort->Context.Udp4.ConfigData;
    pConfig->TimeToLive = 255;
    pConfig->AcceptAnyPort = FALSE;
    pConfig->AcceptBroadcast = FALSE;
    pConfig->AcceptPromiscuous = FALSE;
    pConfig->AllowDuplicatePort = TRUE;
    pConfig->DoNotFragment = TRUE;
    break;
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Close a UDP4 port.

  This routine releases the resources allocated by
  ::EslUdp4PortAllocate.

  This routine is called by ::EslSocketPortClose.
  See the \ref PortCloseStateMachine section.

  @param [in] pPort       Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS     The port is closed
  @retval other           Port close error

**/
EFI_STATUS
EslUdp4PortClose (
  IN ESL_PORT * pPort
  )
{
  UINTN DebugFlags;
  ESL_UDP4_CONTEXT * pUdp4;
  EFI_STATUS Status;
  
  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;
  DebugFlags = pPort->DebugFlags;
  pUdp4 = &pPort->Context.Udp4;

  //
  //  Done with the receive event
  //
  if ( NULL != pUdp4->RxToken.Event ) {
    Status = gBS->CloseEvent ( pUdp4->RxToken.Event );
    if ( !EFI_ERROR ( Status )) {
      DEBUG (( DebugFlags | DEBUG_POOL,
                "0x%08x: Closed receive event\r\n",
                pUdp4->RxToken.Event ));
    }
    else {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to close the receive event, Status: %r\r\n",
                Status ));
      ASSERT ( EFI_SUCCESS == Status );
    }
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Free a receive packet

  This routine performs the network specific operations necessary
  to free a receive packet.

  This routine is called by ::EslSocketPortCloseTxDone to free a
  receive packet.

  @param [in] pPacket         Address of an ::ESL_PACKET structure.
  @param [in, out] pRxBytes   Address of the count of RX bytes

**/
VOID
EslUdp4PortClosePacketFree (
  IN ESL_PACKET * pPacket,
  IN OUT size_t * pRxBytes
  )
{
  DBG_ENTER ( );

  //
  //  Account for the receive bytes
  //
  *pRxBytes -= pPacket->Op.Udp4Rx.pRxData->DataLength;

  //
  //  Return the buffer to the UDP4 driver
  //
  gBS->SignalEvent ( pPacket->Op.Udp4Rx.pRxData->RecycleSignal );
  DBG_EXIT ( );
}


/**
  Perform the network specific close operation on the port.

  This routine performs a cancel operations on the UDPv4 port to
  shutdown the receive operations on the port.

  This routine is called by the ::EslSocketPortCloseTxDone
  routine after the port completes all of the transmission.

  @param [in] pPort           Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS         The port is closed, not normally returned
  @retval EFI_NOT_READY       The port is still closing
  @retval EFI_ALREADY_STARTED Error, the port is in the wrong state,
                              most likely the routine was called already.

**/
EFI_STATUS
EslUdp4PortCloseRxStop (
  IN ESL_PORT * pPort
  )
{
  ESL_UDP4_CONTEXT * pUdp4;
  EFI_UDP4_PROTOCOL * pUdp4Protocol;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Reset the port, cancel the outstanding receive
  //
  pUdp4 = &pPort->Context.Udp4;
  pUdp4Protocol = pPort->pProtocol.UDPv4;
  Status = pUdp4Protocol->Cancel ( pUdp4Protocol,
                                   &pPort->Context.Udp4.RxToken );
  if ( !EFI_ERROR ( Status )) {
    DEBUG (( pPort->DebugFlags | DEBUG_CLOSE | DEBUG_INFO,
              "0x%08x: Packet receive aborted on port: 0x%08x\r\n",
              pPort->pReceivePending,
              pPort ));
  }
  else {
    DEBUG (( pPort->DebugFlags | DEBUG_CLOSE | DEBUG_INFO,
             "0x%08x: Packet receive pending on Port 0x%08x\r\n",
             pPort->pReceivePending,
             pPort ));
    Status = EFI_SUCCESS;
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Receive data from a network connection.

  This routine attempts to return buffered data to the caller.  The
  data is only removed from the normal queue, the message flag
  MSG_OOB is ignored.  See the \ref Udp4ReceiveEngine section.

  This routine is called by ::EslSocketReceive to handle the network
  specific receive operation to support SOCK_DGRAM sockets.

  @param [in] pSocket         Address of an ::ESL_SOCKET structure

  @param [in] Flags           Message control flags

  @param [in] BufferLength    Length of the the buffer

  @param [in] pBuffer         Address of a buffer to receive the data.

  @param [in] pDataLength     Number of received data bytes in the buffer.

  @param [out] pAddress       Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @retval EFI_SUCCESS - Socket data successfully received

**/
EFI_STATUS
EslUdp4Receive (
  IN ESL_SOCKET * pSocket,
  IN INT32 Flags,
  IN size_t BufferLength,
  IN UINT8 * pBuffer,
  OUT size_t * pDataLength,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  )
{
  socklen_t AddressLength;
  size_t BytesToCopy;
  size_t DataBytes;
  UINT32 Fragment;
  in_addr_t IpAddress;
  size_t LengthInBytes;
  UINT8 * pData;
  ESL_PACKET * pPacket;
  ESL_PORT * pPort;
  struct sockaddr_in * pRemoteAddress;
  EFI_UDP4_RECEIVE_DATA * pRxData;
  ESL_UDP4_CONTEXT * pUdp4;
  struct sockaddr_in RemoteAddress;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume failure
  //
  Status = EFI_UNSUPPORTED;
  pSocket->errno = ENOTCONN;

  //
  //  Verify that the socket is connected
  //
  if ( SOCKET_STATE_CONNECTED == pSocket->State ) {
    //
    //  Locate the port
    //
    pPort = pSocket->pPortList;
    if ( NULL != pPort ) {
      //
      //  Determine if there is any data on the queue
      //
      pUdp4 = &pPort->Context.Udp4;
      pPacket = pSocket->pRxPacketListHead;
      if ( NULL != pPacket ) {
        //
        //  Validate the return address parameters
        //
        pRxData = pPacket->Op.Udp4Rx.pRxData;
        if (( NULL == pAddress ) || ( NULL != pAddressLength )) {
          //
          //  Return the remote system address if requested
          //
          if ( NULL != pAddress ) {
            //
            //  Build the remote address
            //
            DEBUG (( DEBUG_RX,
                      "Getting packet source address: %d.%d.%d.%d:%d\r\n",
                      pRxData->UdpSession.SourceAddress.Addr[0],
                      pRxData->UdpSession.SourceAddress.Addr[1],
                      pRxData->UdpSession.SourceAddress.Addr[2],
                      pRxData->UdpSession.SourceAddress.Addr[3],
                      pRxData->UdpSession.SourcePort ));
            ZeroMem ( &RemoteAddress, sizeof ( RemoteAddress ));
            RemoteAddress.sin_len = sizeof ( RemoteAddress );
            RemoteAddress.sin_family = AF_INET;
            IpAddress = pRxData->UdpSession.SourceAddress.Addr[3];
            IpAddress <<= 8;
            IpAddress |= pRxData->UdpSession.SourceAddress.Addr[2];
            IpAddress <<= 8;
            IpAddress |= pRxData->UdpSession.SourceAddress.Addr[1];
            IpAddress <<= 8;
            IpAddress |= pRxData->UdpSession.SourceAddress.Addr[0];
            RemoteAddress.sin_addr.s_addr = IpAddress;
            RemoteAddress.sin_port = SwapBytes16 ( pRxData->UdpSession.SourcePort );

            //
            //  Copy the address
            //
            pRemoteAddress = (struct sockaddr_in *)pAddress;
            AddressLength = sizeof ( *pRemoteAddress );
            if ( AddressLength > *pAddressLength ) {
              AddressLength = *pAddressLength;
            }
            CopyMem ( pRemoteAddress,
                      &RemoteAddress,
                      AddressLength );

            //
            //  Update the address length
            //
            *pAddressLength = AddressLength;
          }

          //
          //  Reduce the buffer length if necessary
          //
          DataBytes = pRxData->DataLength;
          if ( DataBytes < BufferLength ) {
            BufferLength = DataBytes;
          }

          //
          //  Copy the received data
          //
          LengthInBytes = 0;
          Fragment = 0;
          while ( BufferLength > LengthInBytes ) {
            //
            //  Determine the amount of received data
            //
            pData = pRxData->FragmentTable[Fragment].FragmentBuffer;
            BytesToCopy = pRxData->FragmentTable[Fragment].FragmentLength;
            if (( BufferLength - LengthInBytes ) < BytesToCopy ) {
              BytesToCopy = BufferLength - LengthInBytes;
            }
            LengthInBytes += BytesToCopy;

            //
            //  Move the data into the buffer
            //
            DEBUG (( DEBUG_RX,
                      "0x%08x: Port copy packet 0x%08x data into 0x%08x, 0x%08x bytes\r\n",
                      pPort,
                      pPacket,
                      pBuffer,
                      BytesToCopy ));
            CopyMem ( pBuffer, pData, BytesToCopy );
            pBuffer += BytesToCopy;
          }

          //
          //  Determine if the data is being read
          //
          if ( 0 == ( Flags & MSG_PEEK )) {
            //
            //  Display for the bytes consumed
            //
            DEBUG (( DEBUG_RX,
                      "0x%08x: Port account for 0x%08x bytes\r\n",
                      pPort,
                      BufferLength ));

            //
            //  All done with this packet
            //  Account for any discarded data
            //
            pSocket->RxBytes -= DataBytes;
            if ( 0 != ( DataBytes - BufferLength )) {
              DEBUG (( DEBUG_RX,
                        "0x%08x: Port, packet read, skipping over 0x%08x bytes\r\n",
                        pPort,
                        DataBytes - BufferLength ));
            }

            //
            //  Remove this packet from the queue
            //
            pSocket->pRxPacketListHead = pPacket->pNext;
            if ( NULL == pSocket->pRxPacketListHead ) {
              pSocket->pRxPacketListTail = NULL;
            }

            //
            //  Return this packet to the UDP4 driver
            //
            gBS->SignalEvent ( pRxData->RecycleSignal );

            //
            //  Move the packet to the free queue
            //
            pPacket->pNext = pSocket->pRxFree;
            pSocket->pRxFree = pPacket;
            DEBUG (( DEBUG_RX,
                      "0x%08x: Port freeing packet 0x%08x\r\n",
                      pPort,
                      pPacket ));

            //
            //  Restart this receive operation if necessary
            //
            if (( NULL == pPort->pReceivePending )
              && ( MAX_RX_DATA > pSocket->RxBytes )) {
                EslUdp4RxStart ( pPort );
            }
          }

          //
          //  Return the data length
          //
          *pDataLength = LengthInBytes;

          //
          //  Successful operation
          //
          Status = EFI_SUCCESS;
          pSocket->errno = 0;
        }
        else {
          //
          //  Bad return address pointer and length
          //
          Status = EFI_INVALID_PARAMETER;
          pSocket->errno = EINVAL;
        }
      }
      else {
        //
        //  The queue is empty
        //  Determine if it is time to return the receive error
        //
        if ( EFI_ERROR ( pSocket->RxError )) {
          Status = pSocket->RxError;
          switch ( Status ) {
          default:
            pSocket->errno = EIO;
            break;

          case EFI_HOST_UNREACHABLE:
            pSocket->errno = EHOSTUNREACH;
            break;

          case EFI_NETWORK_UNREACHABLE:
            pSocket->errno = ENETUNREACH;
            break;

          case EFI_PORT_UNREACHABLE:
            pSocket->errno = EPROTONOSUPPORT;
            break;

          case EFI_PROTOCOL_UNREACHABLE:
            pSocket->errno = ENOPROTOOPT;
            break;
          }
          pSocket->RxError = EFI_SUCCESS;
        }
        else {
          Status = EFI_NOT_READY;
          pSocket->errno = EAGAIN;
        }
      }
    }
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Get the remote socket address

  This routine returns the address of the remote connection point
  associated with the SOCK_DGRAM socket.

  This routine is called by ::EslSocketGetPeerAddress to detemine
  the UDPv4 address and port number associated with the network adapter.

  @param [in] pPort       Address of an ::ESL_PORT structure.

  @param [out] pAddress   Network address to receive the remote system address

**/
VOID
EslUdp4RemoteAddressGet (
  IN ESL_PORT * pPort,
  OUT struct sockaddr * pAddress
  )
{
  struct sockaddr_in * pRemoteAddress;
  ESL_UDP4_CONTEXT * pUdp4;

  DBG_ENTER ( );

  //
  //  Return the remote address
  //
  pUdp4 = &pPort->Context.Udp4;
  pRemoteAddress = (struct sockaddr_in *)pAddress;
  pRemoteAddress->sin_family = AF_INET;
  pRemoteAddress->sin_port = SwapBytes16 ( pUdp4->ConfigData.RemotePort );
  CopyMem ( &pRemoteAddress->sin_addr,
            &pUdp4->ConfigData.RemoteAddress.Addr[0],
            sizeof ( pRemoteAddress->sin_addr ));

  DBG_EXIT ( );
}


/**
  Set the remote address

  This routine sets the remote address in the port.

  This routine is called by ::EslSocketConnect to specify the
  remote network address.

  @param [in] pPort           Address of an ::ESL_PORT structure.

  @param [in] pSockAddr       Network address of the remote system.

  @param [in] SockAddrLength  Length in bytes of the network address.

 **/
VOID
EslUdp4RemoteAddressSet (
  IN ESL_PORT * pPort,
  IN CONST struct sockaddr * pSockAddr,
  IN socklen_t SockAddrLength
  )
{
  CONST struct sockaddr_in * pRemoteAddress;
  ESL_UDP4_CONTEXT * pUdp4;

  DBG_ENTER ( );

  //
  //  Set the remote address
  //
  pUdp4 = &pPort->Context.Udp4;
  pRemoteAddress = (struct sockaddr_in *)pSockAddr;
  pUdp4->ConfigData.RemoteAddress.Addr[0] = (UINT8)( pRemoteAddress->sin_addr.s_addr );
  pUdp4->ConfigData.RemoteAddress.Addr[1] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 8 );
  pUdp4->ConfigData.RemoteAddress.Addr[2] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 16 );
  pUdp4->ConfigData.RemoteAddress.Addr[3] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 24 );
  pUdp4->ConfigData.RemotePort = SwapBytes16 ( pRemoteAddress->sin_port );

  DBG_EXIT ( );
}


/**
  Cancel the receive operations

  This routine cancels the pending receive operations.
  See the \ref Udp4ReceiveEngine section.

  This routine is called by ::EslSocketShutdown when the socket
  layer is being shutdown.

  @param [in] pSocket   Address of an ::ESL_SOCKET structure
  
  @retval EFI_SUCCESS - The cancel was successful

 **/
EFI_STATUS
EslUdp4RxCancel (
  IN ESL_SOCKET * pSocket
  )
{
  ESL_PACKET * pPacket;
  ESL_PORT * pPort;
  ESL_UDP4_CONTEXT * pUdp4;
  EFI_UDP4_PROTOCOL * pUdp4Protocol;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume failure
  //
  Status = EFI_NOT_FOUND;

  //
  //  Locate the port
  //
  pPort = pSocket->pPortList;
  if ( NULL != pPort ) {
    //
    //  Determine if a receive is pending
    //
    pUdp4 = &pPort->Context.Udp4;
    pPacket = pPort->pReceivePending;
    if ( NULL != pPacket ) {
      //
      //  Attempt to cancel the receive operation
      //
      pUdp4Protocol = pPort->pProtocol.UDPv4;
      Status = pUdp4Protocol->Cancel ( pUdp4Protocol,
                                       &pUdp4->RxToken );
      if ( EFI_NOT_FOUND == Status ) {
        //
        //  The receive is complete
        //
        Status = EFI_SUCCESS;
      }
    }
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Process the receive completion

  This routine keeps the UDPv4 driver's buffer and queues it in
  in FIFO order to the data queue.  The UDP4 driver's buffer will
  be returned by either ::EslUdp4Receive or ::EslSocketPortCloseTxDone.
  See the \ref Tcp4ReceiveEngine section.

  This routine is called by the UDPv4 driver when data is
  received.

  @param [in] Event     The receive completion event

  @param [in] pPort     Address of an ::ESL_PORT structure

**/
VOID
EslUdp4RxComplete (
  IN EFI_EVENT Event,
  IN ESL_PORT * pPort
  )
{
  size_t LengthInBytes;
  ESL_PACKET * pPacket;
  ESL_PACKET * pPrevious;
  EFI_UDP4_RECEIVE_DATA * pRxData;
  ESL_SOCKET * pSocket;
  ESL_UDP4_CONTEXT * pUdp4;
  EFI_STATUS Status;
  
  DBG_ENTER ( );
  
  //
  //  Mark this receive complete
  //
  pUdp4 = &pPort->Context.Udp4;
  pPacket = pPort->pReceivePending;
  pPort->pReceivePending = NULL;
  
  //
  //  Determine if this receive was successful
  //
  pSocket = pPort->pSocket;
  Status = pUdp4->RxToken.Status;
  if (( !EFI_ERROR ( Status )) && ( !pSocket->bRxDisable )) {
    pRxData = pUdp4->RxToken.Packet.RxData;
    if ( PORT_STATE_CLOSE_STARTED >= pPort->State ) {
      //
      //  Save the data in the packet
      //
      pPacket->Op.Udp4Rx.pRxData = pRxData;

      //
      //  Queue this packet
      //
      pPrevious = pSocket->pRxPacketListTail;
      if ( NULL == pPrevious ) {
        pSocket->pRxPacketListHead = pPacket;
      }
      else {
        pPrevious->pNext = pPacket;
      }
      pSocket->pRxPacketListTail = pPacket;

      //
      //  Account for the data
      //
      LengthInBytes = pRxData->DataLength;
      pSocket->RxBytes += LengthInBytes;

      //
      //  Log the received data
      //
      DEBUG (( DEBUG_RX | DEBUG_INFO,
                "Received packet from: %d.%d.%d.%d:%d\r\n",
                pRxData->UdpSession.SourceAddress.Addr[0],
                pRxData->UdpSession.SourceAddress.Addr[1],
                pRxData->UdpSession.SourceAddress.Addr[2],
                pRxData->UdpSession.SourceAddress.Addr[3],
                pRxData->UdpSession.SourcePort ));
      DEBUG (( DEBUG_RX | DEBUG_INFO,
                "Received packet sent to: %d.%d.%d.%d:%d\r\n",
                pRxData->UdpSession.DestinationAddress.Addr[0],
                pRxData->UdpSession.DestinationAddress.Addr[1],
                pRxData->UdpSession.DestinationAddress.Addr[2],
                pRxData->UdpSession.DestinationAddress.Addr[3],
                pRxData->UdpSession.DestinationPort ));
      DEBUG (( DEBUG_RX | DEBUG_INFO,
                "0x%08x: Packet queued on port 0x%08x with 0x%08x bytes of data\r\n",
                pPacket,
                pPort,
                LengthInBytes ));

      //
      //  Attempt to restart this receive operation
      //
      if ( pSocket->MaxRxBuf > pSocket->RxBytes ) {
        EslUdp4RxStart ( pPort );
      }
      else {
        DEBUG (( DEBUG_RX,
                  "0x%08x: Port RX suspended, 0x%08x bytes queued\r\n",
                  pPort,
                  pSocket->RxBytes ));
      }
    }
    else {
      //
      //  The port is being closed
      //  Return the buffer to the UDP4 driver
      //
      gBS->SignalEvent ( pRxData->RecycleSignal );

      //
      //  Free the packet
      //
      EslSocketPacketFree ( pPacket, DEBUG_RX );
    }
  }
  else {
    DEBUG (( DEBUG_RX | DEBUG_INFO,
              "ERROR - Receiving packet 0x%08x, on port 0x%08x, Status:%r\r\n",
              pPacket,
              pPort,
              Status ));
  
    //
    //  Receive error, free the packet save the error
    //
    EslSocketPacketFree ( pPacket, DEBUG_RX );
    if ( !EFI_ERROR ( pSocket->RxError )) {
      pSocket->RxError = Status;
    }
  
    //
    //  Update the port state
    //
    if ( PORT_STATE_CLOSE_STARTED <= pPort->State ) {
      EslSocketPortCloseRxDone ( pPort );
    }
    else {
      if ( EFI_ERROR ( Status )) {
        pPort->State = PORT_STATE_RX_ERROR;
      }
    }
  }
  
  DBG_EXIT ( );
}


/**
  Start a receive operation

  This routine posts a receive buffer to the UDPv4 driver.
  See the \ref Udp4ReceiveEngine section.

  This support routine is called by:
  <ul>
    <li>::EslUdp4SocketIsConfigured to start the recevie engine for the new socket.</li>
    <li>::EslUdp4Receive to restart the receive engine to release flow control.</li>
    <li>::EslUdp4RxComplete to continue the operation of the receive engine if flow control is not being applied.</li>
  </ul>

  @param [in] pPort       Address of an ::ESL_PORT structure.

 **/
VOID
EslUdp4RxStart (
  IN ESL_PORT * pPort
  )
{
  ESL_PACKET * pPacket;
  ESL_SOCKET * pSocket;
  ESL_UDP4_CONTEXT * pUdp4;
  EFI_UDP4_PROTOCOL * pUdp4Protocol;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Determine if a receive is already pending
  //
  Status = EFI_SUCCESS;
  pPacket = NULL;
  pSocket = pPort->pSocket;
  pUdp4 = &pPort->Context.Udp4;
  if ( !EFI_ERROR ( pPort->pSocket->RxError )) {
    if (( NULL == pPort->pReceivePending )
      && ( PORT_STATE_CLOSE_STARTED > pPort->State )) {
      //
      //  Determine if there are any free packets
      //
      pPacket = pSocket->pRxFree;
      if ( NULL != pPacket ) {
        //
        //  Remove this packet from the free list
        //
        pSocket->pRxFree = pPacket->pNext;
        DEBUG (( DEBUG_RX,
                  "0x%08x: Port removed packet 0x%08x from free list\r\n",
                  pPort,
                  pPacket ));
      }
      else {
        //
        //  Allocate a packet structure
        //
        Status = EslSocketPacketAllocate ( &pPacket,
                                           sizeof ( pPacket->Op.Udp4Rx ),
                                           DEBUG_RX );
        if ( EFI_ERROR ( Status )) {
          pPacket = NULL;
          DEBUG (( DEBUG_ERROR | DEBUG_RX,
                    "0x%08x: Port failed to allocate RX packet, Status: %r\r\n",
                    pPort,
                    Status ));
        }
      }

      //
      //  Determine if a packet is available
      //
      if ( NULL != pPacket ) {
        //
        //  Initialize the buffer for receive
        //
        pPacket->pNext = NULL;
        pPacket->Op.Udp4Rx.pRxData = NULL;
        pUdp4->RxToken.Packet.RxData = NULL;
        pPort->pReceivePending = pPacket;

        //
        //  Start the receive on the packet
        //
        pUdp4Protocol = pPort->pProtocol.UDPv4;
        Status = pUdp4Protocol->Receive ( pUdp4Protocol,
                                          &pUdp4->RxToken );
        if ( !EFI_ERROR ( Status )) {
          DEBUG (( DEBUG_RX | DEBUG_INFO,
                    "0x%08x: Packet receive pending on port 0x%08x\r\n",
                    pPacket,
                    pPort ));
        }
        else {
          DEBUG (( DEBUG_RX | DEBUG_INFO,
                    "ERROR - Failed to post a receive on port 0x%08x, Status: %r\r\n",
                    pPort,
                    Status ));
          if ( !EFI_ERROR ( pSocket->RxError )) {
            //
            //  Save the error status
            //
            pSocket->RxError = Status;
          }

          //
          //  Free the packet
          //
          pPort->pReceivePending = NULL;
          pPacket->pNext = pSocket->pRxFree;
          pSocket->pRxFree = pPacket;
        }
      }
    }
  }

  DBG_EXIT ( );
}


/**
  Determine if the socket is configured.

  This routine uses the flag ESL_SOCKET::bConfigured to determine
  if the network layer's configuration routine has been called.
  This routine calls the bind and configuration routines if they
  were not already called.  After the port is configured, the
  \ref Udp4ReceiveEngine is started.

  This routine is called by EslSocketIsConfigured to verify
  that the socket is configured.

  @param [in] pSocket         Address of an ::ESL_SOCKET structure

  @retval EFI_SUCCESS - The port is connected
  @retval EFI_NOT_STARTED - The port is not connected

 **/
 EFI_STATUS
 EslUdp4SocketIsConfigured (
  IN ESL_SOCKET * pSocket
  )
{
  EFI_UDP4_CONFIG_DATA * pConfigData;
  ESL_PORT * pPort;
  ESL_PORT * pNextPort;
  ESL_UDP4_CONTEXT * pUdp4;
  EFI_UDP4_PROTOCOL * pUdp4Protocol;
  EFI_STATUS Status;
  struct sockaddr_in LocalAddress;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Configure the port if necessary
  //
  if ( !pSocket->bConfigured ) {
    //
    //  Fill in the port list if necessary
    //
    if ( NULL == pSocket->pPortList ) {
      LocalAddress.sin_len = sizeof ( LocalAddress );
      LocalAddress.sin_family = AF_INET;
      LocalAddress.sin_addr.s_addr = 0;
      LocalAddress.sin_port = 0;
      Status = EslSocketBind ( &pSocket->SocketProtocol,
                               (struct sockaddr *)&LocalAddress,
                               LocalAddress.sin_len,
                               &pSocket->errno );
    }

    //
    //  Walk the port list
    //
    pPort = pSocket->pPortList;
    while ( NULL != pPort ) {
      //
      //  Attempt to configure the port
      //
      pNextPort = pPort->pLinkSocket;
      pUdp4 = &pPort->Context.Udp4;
      pUdp4Protocol = pPort->pProtocol.UDPv4;
      pConfigData = &pUdp4->ConfigData;
      DEBUG (( DEBUG_TX,
                "0x%08x: pPort Configuring for %d.%d.%d.%d:%d --> %d.%d.%d.%d:%d\r\n",
                pPort,
                pConfigData->StationAddress.Addr[0],
                pConfigData->StationAddress.Addr[1],
                pConfigData->StationAddress.Addr[2],
                pConfigData->StationAddress.Addr[3],
                pConfigData->StationPort,
                pConfigData->RemoteAddress.Addr[0],
                pConfigData->RemoteAddress.Addr[1],
                pConfigData->RemoteAddress.Addr[2],
                pConfigData->RemoteAddress.Addr[3],
                pConfigData->RemotePort ));
      Status = pUdp4Protocol->Configure ( pUdp4Protocol,
                                          pConfigData );
      if ( !EFI_ERROR ( Status )) {
        //
        //  Update the configuration data
        //
        Status = pUdp4Protocol->GetModeData ( pUdp4Protocol,
                                              pConfigData,
                                              NULL,
                                              NULL,
                                              NULL );
      }
      if ( EFI_ERROR ( Status )) {
        DEBUG (( DEBUG_LISTEN,
                  "ERROR - Failed to configure the Udp4 port, Status: %r\r\n",
                  Status ));
        switch ( Status ) {
        case EFI_ACCESS_DENIED:
          pSocket->errno = EACCES;
          break;

        default:
        case EFI_DEVICE_ERROR:
          pSocket->errno = EIO;
          break;

        case EFI_INVALID_PARAMETER:
          pSocket->errno = EADDRNOTAVAIL;
          break;

        case EFI_NO_MAPPING:
          pSocket->errno = EAFNOSUPPORT;
          break;

        case EFI_OUT_OF_RESOURCES:
          pSocket->errno = ENOBUFS;
          break;

        case EFI_UNSUPPORTED:
          pSocket->errno = EOPNOTSUPP;
          break;
        }
      }
      else {
        DEBUG (( DEBUG_TX,
                  "0x%08x: pPort Configured for %d.%d.%d.%d:%d --> %d.%d.%d.%d:%d\r\n",
                  pPort,
                  pConfigData->StationAddress.Addr[0],
                  pConfigData->StationAddress.Addr[1],
                  pConfigData->StationAddress.Addr[2],
                  pConfigData->StationAddress.Addr[3],
                  pConfigData->StationPort,
                  pConfigData->RemoteAddress.Addr[0],
                  pConfigData->RemoteAddress.Addr[1],
                  pConfigData->RemoteAddress.Addr[2],
                  pConfigData->RemoteAddress.Addr[3],
                  pConfigData->RemotePort ));
        pPort->bConfigured = TRUE;

        //
        //  Start the first read on the port
        //
        EslUdp4RxStart ( pPort );

        //
        //  The socket is connected
        //
        pSocket->State = SOCKET_STATE_CONNECTED;
      }

      //
      //  Set the next port
      //
      pPort = pNextPort;
    }

    //
    //  Determine the configuration status
    //
    if ( NULL != pSocket->pPortList ) {
      pSocket->bConfigured = TRUE;
    }
  }

  //
  //  Determine the socket configuration status
  //
  if ( !EFI_ERROR ( Status )) {
    Status = pSocket->bConfigured ? EFI_SUCCESS : EFI_NOT_STARTED;
  }
  
  //
  //  Return the port connected state.
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Buffer data for transmission over a network connection.

  This routine buffers data for the transmit engine in the normal
  data queue.  When the \ref TransmitEngine has resources, this
  routine will start the transmission of the next buffer on the
  network connection.

  This routine is called by ::EslSocketTransmit to buffer
  data for transmission.  The data is copied into a local buffer
  freeing the application buffer for reuse upon return.  When
  necessary, this routine starts the transmit engine that
  performs the data transmission on the network connection.  The
  transmit engine transmits the data a packet at a time over the
  network connection.

  Transmission errors are returned during the next transmission or
  during the close operation.  Only buffering errors are returned
  during the current transmission attempt.

  @param [in] pSocket         Address of an ::ESL_SOCKET structure

  @param [in] Flags           Message control flags

  @param [in] BufferLength    Length of the the buffer

  @param [in] pBuffer         Address of a buffer to receive the data.

  @param [in] pDataLength     Number of received data bytes in the buffer.

  @param [in] pAddress        Network address of the remote system address

  @param [in] AddressLength   Length of the remote network address structure

  @retval EFI_SUCCESS - Socket data successfully buffered

**/
EFI_STATUS
EslUdp4TxBuffer (
  IN ESL_SOCKET * pSocket,
  IN int Flags,
  IN size_t BufferLength,
  IN CONST UINT8 * pBuffer,
  OUT size_t * pDataLength,
  IN const struct sockaddr * pAddress,
  IN socklen_t AddressLength
  )
{
  ESL_PACKET * pPacket;
  ESL_PACKET * pPreviousPacket;
  ESL_PORT * pPort;
  const struct sockaddr_in * pRemoteAddress;
  ESL_UDP4_CONTEXT * pUdp4;
  size_t * pTxBytes;
  ESL_UDP4_TX_DATA * pTxData;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Assume failure
  //
  Status = EFI_UNSUPPORTED;
  pSocket->errno = ENOTCONN;
  *pDataLength = 0;

  //
  //  Verify that the socket is connected
  //
  if ( SOCKET_STATE_CONNECTED == pSocket->State ) {
    //
    //  Locate the port
    //
    pPort = pSocket->pPortList;
    if ( NULL != pPort ) {
      //
      //  Determine the queue head
      //
      pUdp4 = &pPort->Context.Udp4;
      pTxBytes = &pSocket->TxBytes;

      //
      //  Verify that there is enough room to buffer another
      //  transmit operation
      //
      if ( pSocket->MaxTxBuf > *pTxBytes ) {
        //
        //  Attempt to allocate the packet
        //
        Status = EslSocketPacketAllocate ( &pPacket,
                                           sizeof ( pPacket->Op.Udp4Tx )
                                           - sizeof ( pPacket->Op.Udp4Tx.Buffer )
                                           + BufferLength,
                                           DEBUG_TX );
        if ( !EFI_ERROR ( Status )) {
          //
          //  Initialize the transmit operation
          //
          pTxData = &pPacket->Op.Udp4Tx;
          pTxData->TxData.GatewayAddress = NULL;
          pTxData->TxData.UdpSessionData = NULL;
          pTxData->TxData.DataLength = (UINT32) BufferLength;
          pTxData->TxData.FragmentCount = 1;
          pTxData->TxData.FragmentTable[0].FragmentLength = (UINT32) BufferLength;
          pTxData->TxData.FragmentTable[0].FragmentBuffer = &pPacket->Op.Udp4Tx.Buffer[0];
          pTxData->RetransmitCount = 0;

          //
          //  Set the remote system address if necessary
          //
          pTxData->TxData.UdpSessionData = NULL;
          if ( NULL != pAddress ) {
            pRemoteAddress = (const struct sockaddr_in *)pAddress;
            pTxData->Session.SourceAddress.Addr[0] = pUdp4->ConfigData.StationAddress.Addr[0];
            pTxData->Session.SourceAddress.Addr[1] = pUdp4->ConfigData.StationAddress.Addr[1];
            pTxData->Session.SourceAddress.Addr[2] = pUdp4->ConfigData.StationAddress.Addr[2];
            pTxData->Session.SourceAddress.Addr[3] = pUdp4->ConfigData.StationAddress.Addr[3];
            pTxData->Session.SourcePort = 0;
            pTxData->Session.DestinationAddress.Addr[0] = (UINT8)pRemoteAddress->sin_addr.s_addr;
            pTxData->Session.DestinationAddress.Addr[1] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 8 );
            pTxData->Session.DestinationAddress.Addr[2] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 16 );
            pTxData->Session.DestinationAddress.Addr[3] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 24 );
            pTxData->Session.DestinationPort = SwapBytes16 ( pRemoteAddress->sin_port );

            //
            //  Use the remote system address when sending this packet
            //
            pTxData->TxData.UdpSessionData = &pTxData->Session;
          }

          //
          //  Copy the data into the buffer
          //
          CopyMem ( &pPacket->Op.Udp4Tx.Buffer[0],
                    pBuffer,
                    BufferLength );

          //
          //  Synchronize with the socket layer
          //
          RAISE_TPL ( TplPrevious, TPL_SOCKETS );

          //
          //  Stop transmission after an error
          //
          if ( !EFI_ERROR ( pSocket->TxError )) {
            //
            //  Display the request
            //
            DEBUG (( DEBUG_TX,
                      "Send %d %s bytes from 0x%08x\r\n",
                      BufferLength,
                      pBuffer ));

            //
            //  Queue the data for transmission
            //
            pPacket->pNext = NULL;
            pPreviousPacket = pSocket->pTxPacketListTail;
            if ( NULL == pPreviousPacket ) {
              pSocket->pTxPacketListHead = pPacket;
            }
            else {
              pPreviousPacket->pNext = pPacket;
            }
            pSocket->pTxPacketListTail = pPacket;
            DEBUG (( DEBUG_TX,
                      "0x%08x: Packet on transmit list\r\n",
                      pPacket ));

            //
            //  Account for the buffered data
            //
            *pTxBytes += BufferLength;
            *pDataLength = BufferLength;

            //
            //  Start the transmit engine if it is idle
            //
            if ( NULL != pPort->pTxFree ) {
              EslSocketTxStart ( pPort,
                                 &pSocket->pTxPacketListHead,
                                 &pSocket->pTxPacketListTail,
                                 &pPort->pTxActive,
                                 &pPort->pTxFree );
            }
          }
          else {
            //
            //  Previous transmit error
            //  Stop transmission
            //
            Status = pSocket->TxError;
            pSocket->errno = EIO;

            //
            //  Free the packet
            //
            EslSocketPacketFree ( pPacket, DEBUG_TX );
          }

          //
          //  Release the socket layer synchronization
          //
          RESTORE_TPL ( TplPrevious );
        }
        else {
          //
          //  Packet allocation failed
          //
          pSocket->errno = ENOMEM;
        }
      }
      else {
        //
        //  Not enough buffer space available
        //
        pSocket->errno = EAGAIN;
        Status = EFI_NOT_READY;
      }
    }
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Process the transmit completion

  This routine use ::EslSocketTxComplete to perform the transmit
  completion processing for data packets.

  This routine is called by the UDPv4 network layer when a data
  transmit request completes.

  @param [in] Event     The normal transmit completion event

  @param [in] pIo       Address of an ::ESL_IO_MGMT structure

**/
VOID
EslUdp4TxComplete (
  IN EFI_EVENT Event,
  IN ESL_IO_MGMT * pIo
  )
{
  UINT32 LengthInBytes;
  ESL_PORT * pPort;
  ESL_PACKET * pPacket;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  
  DBG_ENTER ( );
  
  //
  //  Locate the active transmit packet
  //
  pPacket = pIo->pPacket;
  pPort = pIo->pPort;
  pSocket = pPort->pSocket;

  //
  //  Get the transmit length and status
  //
  LengthInBytes = pPacket->Op.Udp4Tx.TxData.DataLength;
  pSocket->TxBytes -= LengthInBytes;
  Status = pIo->Token.Udp4Tx.Status;

  //
  //  Complete the transmit operation
  //
  EslSocketTxComplete ( pIo,
                        LengthInBytes,
                        Status,
                        "UDP ",
                        &pSocket->pTxPacketListHead,
                        &pSocket->pTxPacketListTail,
                        &pPort->pTxActive,
                        &pPort->pTxFree );
  DBG_EXIT ( );
}
