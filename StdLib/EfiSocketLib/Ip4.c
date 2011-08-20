/** @file
  Implement the IP4 driver support for the socket layer.

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Socket.h"

CONST DT_PROTOCOL_API cEslIp4Api = {
  IPPROTO_IP,
  NULL,   //  Accept
  EslIpBind4,
  EslIpConnect4,
  NULL,   //  ConnectPoll
  EslIpGetLocalAddress4,
  EslIpGetRemoteAddress4,
  EslIpSocketIsConfigured4,
  NULL,   //  Listen
  EslIpOptionGet4,
  EslIpOptionSet4,
  EslIpReceive4,
  EslIpRxCancel4,
  EslIpTxBuffer4
};


/**
  Allocate and initialize a DT_PORT structure.

  @param [in] pSocket     Address of the socket structure.
  @param [in] pService    Address of the DT_SERVICE structure.
  @param [in] ChildHandle Ip4 child handle
  @param [in] pIpAddress  Buffer containing IP4 network address of the local host
  @param [in] DebugFlags  Flags for debug messages
  @param [out] ppPort     Buffer to receive new DT_PORT structure address

  @retval EFI_SUCCESS - Socket successfully created

 **/
EFI_STATUS
EslIpPortAllocate4 (
  IN DT_SOCKET * pSocket,
  IN DT_SERVICE * pService,
  IN EFI_HANDLE ChildHandle,
  IN CONST UINT8 * pIpAddress,
  IN UINTN DebugFlags,
  OUT DT_PORT ** ppPort
  )
{
  UINTN LengthInBytes;
  EFI_IP4_CONFIG_DATA * pConfig;
  DT_LAYER * pLayer;
  DT_PORT * pPort;
  DT_IP4_CONTEXT * pIp4;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Use for/break instead of goto
  for ( ; ; ) {
    //
    //  Allocate a port structure
    //
    pLayer = &mEslLayer;
    LengthInBytes = sizeof ( *pPort );
    Status = gBS->AllocatePool ( EfiRuntimeServicesData,
                                 LengthInBytes,
                                 (VOID **)&pPort );
    if ( EFI_ERROR ( Status )) {
      DEBUG (( DEBUG_ERROR | DebugFlags | DEBUG_POOL | DEBUG_INIT,
                "ERROR - Failed to allocate the port structure, Status: %r\r\n",
                Status ));
      pSocket->errno = ENOMEM;
      pPort = NULL;
      break;
    }
    DEBUG (( DebugFlags | DEBUG_POOL | DEBUG_INIT,
              "0x%08x: Allocate pPort, %d bytes\r\n",
              pPort,
              LengthInBytes ));

    //
    //  Initialize the port
    //
    ZeroMem ( pPort, LengthInBytes );
    pPort->Signature = PORT_SIGNATURE;
    pPort->pService = pService;
    pPort->pSocket = pSocket;
    pPort->pfnCloseStart = EslIpPortCloseStart4;
    pPort->DebugFlags = DebugFlags;

    //
    //  Allocate the receive event
    //
    pIp4 = &pPort->Context.Ip4;
    Status = gBS->CreateEvent (  EVT_NOTIFY_SIGNAL,
                                 TPL_SOCKETS,
                                 (EFI_EVENT_NOTIFY)EslIpRxComplete4,
                                 pPort,
                                 &pIp4->RxToken.Event);
    if ( EFI_ERROR ( Status )) {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to create the receive event, Status: %r\r\n",
                Status ));
      pSocket->errno = ENOMEM;
      break;
    }
    DEBUG (( DEBUG_RX | DEBUG_POOL,
              "0x%08x: Created receive event\r\n",
              pIp4->RxToken.Event ));

    //
    //  Allocate the transmit event
    //
    Status = gBS->CreateEvent (  EVT_NOTIFY_SIGNAL,
                                 TPL_SOCKETS,
                                 (EFI_EVENT_NOTIFY)EslIpTxComplete4,
                                 pPort,
                                 &pIp4->TxToken.Event);
    if ( EFI_ERROR ( Status )) {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to create the transmit event, Status: %r\r\n",
                Status ));
      pSocket->errno = ENOMEM;
      break;
    }
    DEBUG (( DEBUG_CLOSE | DEBUG_POOL,
              "0x%08x: Created transmit event\r\n",
              pIp4->TxToken.Event ));

    //
    //  Open the port protocol
    //
    Status = gBS->OpenProtocol (
                    ChildHandle,
                    &gEfiIp4ProtocolGuid,
                    (VOID **) &pIp4->pProtocol,
                    pLayer->ImageHandle,
                    NULL,
                    EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL );
    if ( EFI_ERROR ( Status )) {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to open gEfiIp4ProtocolGuid on controller 0x%08x\r\n",
                pIp4->Handle ));
      pSocket->errno = EEXIST;
      break;
    }
    DEBUG (( DebugFlags,
              "0x%08x: gEfiIp4ProtocolGuid opened on controller 0x%08x\r\n",
              pIp4->pProtocol,
              ChildHandle ));

    //
    //  Set the port address
    //
    pIp4->Handle = ChildHandle;
    pConfig = &pPort->Context.Ip4.ModeData.ConfigData;
    pConfig->DefaultProtocol = (UINT8)pSocket->Protocol;
    if (( 0 == pIpAddress[0])
      && ( 0 == pIpAddress[1])
      && ( 0 == pIpAddress[2])
      && ( 0 == pIpAddress[3])) {
      pConfig->UseDefaultAddress = TRUE;
      DEBUG (( DebugFlags,
                "0x%08x: Port using default IP address\r\n",
                pPort ));
    }
    else {
      pConfig->StationAddress.Addr[0] = pIpAddress[0];
      pConfig->StationAddress.Addr[1] = pIpAddress[1];
      pConfig->StationAddress.Addr[2] = pIpAddress[2];
      pConfig->StationAddress.Addr[3] = pIpAddress[3];
      pConfig->SubnetMask.Addr[0] = 0xff;
      pConfig->SubnetMask.Addr[1] = 0xff;
      pConfig->SubnetMask.Addr[2] = 0xff;
      pConfig->SubnetMask.Addr[3] = 0xff;
      DEBUG (( DebugFlags,
                "0x%08x: Port using IP address: %d.%d.%d.%d\r\n",
                pPort,
                pConfig->StationAddress.Addr[0],
                pConfig->StationAddress.Addr[1],
                pConfig->StationAddress.Addr[2],
                pConfig->StationAddress.Addr[3]));
    }
    pConfig->AcceptAnyProtocol = (BOOLEAN)( 0 == pConfig->UseDefaultAddress);
    pConfig->AcceptIcmpErrors = FALSE;
    pConfig->AcceptBroadcast = FALSE;
    pConfig->AcceptPromiscuous = FALSE;
    pConfig->TypeOfService = 0;
    pConfig->TimeToLive = 255;
    pConfig->DoNotFragment = FALSE;
    pConfig->RawData = FALSE;
    pConfig->ReceiveTimeout = 0;
    pConfig->TransmitTimeout = 0;

    //
    //  Verify the socket layer synchronization
    //
    VERIFY_TPL ( TPL_SOCKETS );

    //
    //  Add this port to the socket
    //
    pPort->pLinkSocket = pSocket->pPortList;
    pSocket->pPortList = pPort;
    DEBUG (( DebugFlags,
              "0x%08x: Socket adding port: 0x%08x\r\n",
              pSocket,
              pPort ));

    //
    //  Add this port to the service
    //
    pPort->pLinkService = pService->pPortList;
    pService->pPortList = pPort;

    //
    //  Return the port
    //
    *ppPort = pPort;
    break;
  }

  //
  //  Clean up after the error if necessary
  //
  if (( EFI_ERROR ( Status )) && ( NULL != pPort )) {
    //
    //  Close the port
    //
    EslIpPortClose4 ( pPort );
  }
  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Bind a name to a socket.

  The ::IpBind4 routine connects a name to a IP4 stack on the local machine.

  The configure call to the IP4 driver occurs on the first poll, recv, recvfrom,
  send or sentto call.  Until then, all changes are made in the local IP context
  structure.
  
  @param [in] pSocket   Address of the socket structure.

  @param [in] pSockAddr Address of a sockaddr structure that contains the
                        connection point on the local machine.  An IPv4 address
                        of INADDR_ANY specifies that the connection is made to
                        all of the network stacks on the platform.  Specifying a
                        specific IPv4 address restricts the connection to the
                        network stack supporting that address.  Specifying zero
                        for the port causes the network layer to assign a port
                        number from the dynamic range.  Specifying a specific
                        port number causes the network layer to use that port.

  @param [in] SockAddrLen   Specifies the length in bytes of the sockaddr structure.

  @retval EFI_SUCCESS - Socket successfully created

 **/
EFI_STATUS
EslIpBind4 (
  IN DT_SOCKET * pSocket,
  IN const struct sockaddr * pSockAddr,
  IN socklen_t SockAddrLength
  )
{
  EFI_HANDLE ChildHandle;
  CONST struct sockaddr_in * pIp4Address;
  EFI_SERVICE_BINDING_PROTOCOL * pIp4Service;
  DT_LAYER * pLayer;
  DT_PORT * pPort;
  DT_SERVICE * pService;
  EFI_STATUS Status;
  EFI_STATUS TempStatus;
  
  DBG_ENTER ( );
  
  //
  //  Verify the socket layer synchronization
  //
  VERIFY_TPL ( TPL_SOCKETS );
  
  //
  //  Assume success
  //
  pSocket->errno = 0;
  Status = EFI_SUCCESS;
  
  //
  //  Validate the address length
  //
  pIp4Address = (CONST struct sockaddr_in *) pSockAddr;
  if ( SockAddrLength >= ( sizeof ( *pIp4Address )
                           - sizeof ( pIp4Address->sin_zero ))) {

    //
    //  Walk the list of services
    //
    pLayer = &mEslLayer;
    pService = pLayer->pIp4List;
    while ( NULL != pService ) {

      //
      //  Create the IP port
      //
      pIp4Service = pService->pInterface;
      ChildHandle = NULL;
      Status = pIp4Service->CreateChild ( pIp4Service,
                                          &ChildHandle );
      if ( !EFI_ERROR ( Status )) {
        DEBUG (( DEBUG_BIND | DEBUG_POOL,
                  "0x%08x: Ip4 port handle created\r\n",
                  ChildHandle ));
  
        //
        //  Open the port
        //
        Status = EslIpPortAllocate4 ( pSocket,
                                      pService,
                                      ChildHandle,
                                      (UINT8 *) &pIp4Address->sin_addr.s_addr,
                                      DEBUG_BIND,
                                      &pPort );
      }
      else {
        DEBUG (( DEBUG_BIND | DEBUG_POOL,
                  "ERROR - Failed to open Ip4 port handle, Status: %r\r\n",
                  Status ));
        ChildHandle = NULL;
      }
  
      //
      //  Close the port if necessary
      //
      if (( EFI_ERROR ( Status )) && ( NULL != ChildHandle )) {
        TempStatus = pIp4Service->DestroyChild ( pIp4Service,
                                                 ChildHandle );
        if ( !EFI_ERROR ( TempStatus )) {
          DEBUG (( DEBUG_BIND | DEBUG_POOL,
                    "0x%08x: Ip4 port handle destroyed\r\n",
                    ChildHandle ));
        }
        else {
          DEBUG (( DEBUG_ERROR | DEBUG_BIND | DEBUG_POOL,
                    "ERROR - Failed to destroy the Ip4 port handle 0x%08x, Status: %r\r\n",
                    ChildHandle,
                    TempStatus ));
          ASSERT ( EFI_SUCCESS == TempStatus );
        }
      }
  
      //
      //  Set the next service
      //
      pService = pService->pNext;
    }
  
    //
    //  Verify that at least one network connection was found
    //
    if ( NULL == pSocket->pPortList ) {
      DEBUG (( DEBUG_BIND | DEBUG_POOL | DEBUG_INIT,
                "Socket address %d.%d.%d.%d (0x%08x) is not available!\r\n",
                ( pIp4Address->sin_addr.s_addr >> 24 ) & 0xff,
                ( pIp4Address->sin_addr.s_addr >> 16 ) & 0xff,
                ( pIp4Address->sin_addr.s_addr >> 8 ) & 0xff,
                pIp4Address->sin_addr.s_addr & 0xff,
                pIp4Address->sin_addr.s_addr ));
      pSocket->errno = EADDRNOTAVAIL;
      Status = EFI_INVALID_PARAMETER;
    }
  }
  else {
    DEBUG (( DEBUG_BIND,
              "ERROR - Invalid Ip4 address length: %d\r\n",
              SockAddrLength ));
    Status = EFI_INVALID_PARAMETER;
    pSocket->errno = EINVAL;
  }
  
  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Connect to a remote system via the network.

  The ::IpConnectStart4= routine sets the remote address for the connection.

  @param [in] pSocket         Address of the socket structure.

  @param [in] pSockAddr       Network address of the remote system.
    
  @param [in] SockAddrLength  Length in bytes of the network address.
  
  @retval EFI_SUCCESS   The connection was successfully established.
  @retval EFI_NOT_READY The connection is in progress, call this routine again.
  @retval Others        The connection attempt failed.

 **/
EFI_STATUS
EslIpConnect4 (
  IN DT_SOCKET * pSocket,
  IN const struct sockaddr * pSockAddr,
  IN socklen_t SockAddrLength
  )
{
  struct sockaddr_in LocalAddress;
  DT_PORT * pPort;
  struct sockaddr_in * pRemoteAddress;
  DT_IP4_CONTEXT * pIp4;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume failure
  //
  Status = EFI_NETWORK_UNREACHABLE;
  pSocket->errno = ENETUNREACH;

  //
  //  Get the address
  //
  pRemoteAddress = (struct sockaddr_in *)pSockAddr;

  //
  //  Validate the address length
  //
  if ( SockAddrLength >= ( sizeof ( *pRemoteAddress )
                           - sizeof ( pRemoteAddress->sin_zero ))) {
    //
    //  Determine if BIND was already called
    //
    if ( NULL == pSocket->pPortList ) {
      //
      //  Allow any local port
      //
      ZeroMem ( &LocalAddress, sizeof ( LocalAddress ));
      LocalAddress.sin_len = sizeof ( LocalAddress );
      LocalAddress.sin_family = AF_INET;
      Status = EslSocketBind ( &pSocket->SocketProtocol,
                               (struct sockaddr *)&LocalAddress,
                               LocalAddress.sin_len,
                               &pSocket->errno );
    }

    //
    //  Walk the list of ports
    //
    pPort = pSocket->pPortList;
    while ( NULL != pPort ) {
      //
      //  Set the remote address
      //
      pIp4 = &pPort->Context.Ip4;
      pIp4->DestinationAddress.Addr[0] = (UINT8)( pRemoteAddress->sin_addr.s_addr );
      pIp4->DestinationAddress.Addr[1] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 8 );
      pIp4->DestinationAddress.Addr[2] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 16 );
      pIp4->DestinationAddress.Addr[3] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 24 );

      //
      //  At least one path exists
      //
      Status = EFI_SUCCESS;
      pSocket->errno = 0;

      //
      //  Set the next port
      //
      pPort = pPort->pLinkSocket;
    }
  }
  else {
    DEBUG (( DEBUG_CONNECT,
              "ERROR - Invalid IP4 address length: %d\r\n",
              SockAddrLength ));
    Status = EFI_INVALID_PARAMETER;
    pSocket->errno = EINVAL;
  }

  //
  //  Return the connect status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Get the local socket address

  @param [in] pSocket             Address of the socket structure.

  @param [out] pAddress           Network address to receive the local system address

  @param [in,out] pAddressLength  Length of the local network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
EFI_STATUS
EslIpGetLocalAddress4 (
  IN DT_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  )
{
  socklen_t LengthInBytes;
  DT_PORT * pPort;
  struct sockaddr_in * pLocalAddress;
  DT_IP4_CONTEXT * pIp4;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Verify the socket layer synchronization
  //
  VERIFY_TPL ( TPL_SOCKETS );

  //
  //  Verify that there is just a single connection
  //
  pPort = pSocket->pPortList;
  if (( NULL != pPort ) && ( NULL == pPort->pLinkSocket )) {
    //
    //  Verify the address length
    //
    LengthInBytes = sizeof ( struct sockaddr_in );
    if ( LengthInBytes <= * pAddressLength ) {
      //
      //  Return the local address
      //
      pIp4 = &pPort->Context.Ip4;
      pLocalAddress = (struct sockaddr_in *)pAddress;
      ZeroMem ( pLocalAddress, LengthInBytes );
      pLocalAddress->sin_family = AF_INET;
      pLocalAddress->sin_len = (uint8_t)LengthInBytes;
      CopyMem ( &pLocalAddress->sin_addr,
                &pIp4->ModeData.ConfigData.StationAddress.Addr[0],
                sizeof ( pLocalAddress->sin_addr ));
      pSocket->errno = 0;
      Status = EFI_SUCCESS;
    }
    else {
      pSocket->errno = EINVAL;
      Status = EFI_INVALID_PARAMETER;
    }
  }
  else {
    pSocket->errno = ENOTCONN;
    Status = EFI_NOT_STARTED;
  }
  
  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Get the remote socket address

  @param [in] pSocket             Address of the socket structure.

  @param [out] pAddress           Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
EFI_STATUS
EslIpGetRemoteAddress4 (
  IN DT_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  )
{
  socklen_t LengthInBytes;
  DT_PORT * pPort;
  struct sockaddr_in * pRemoteAddress;
  DT_IP4_CONTEXT * pIp4;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Verify the socket layer synchronization
  //
  VERIFY_TPL ( TPL_SOCKETS );

  //
  //  Verify that there is just a single connection
  //
  pPort = pSocket->pPortList;
  if (( NULL != pPort ) && ( NULL == pPort->pLinkSocket )) {
    //
    //  Verify the address length
    //
    LengthInBytes = sizeof ( struct sockaddr_in );
    if ( LengthInBytes <= * pAddressLength ) {
      //
      //  Return the local address
      //
      pIp4 = &pPort->Context.Ip4;
      pRemoteAddress = (struct sockaddr_in *)pAddress;
      ZeroMem ( pRemoteAddress, LengthInBytes );
      pRemoteAddress->sin_family = AF_INET;
      pRemoteAddress->sin_len = (uint8_t)LengthInBytes;
      CopyMem ( &pRemoteAddress->sin_addr,
                &pIp4->DestinationAddress.Addr[0],
                sizeof ( pRemoteAddress->sin_addr ));
      pSocket->errno = 0;
      Status = EFI_SUCCESS;
    }
    else {
      pSocket->errno = EINVAL;
      Status = EFI_INVALID_PARAMETER;
    }
  }
  else {
    pSocket->errno = ENOTCONN;
    Status = EFI_NOT_STARTED;
  }
  
  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Initialize the IP4 service.

  This routine initializes the IP4 service after its service binding
  protocol was located on a controller.

  @param [in] pService        DT_SERVICE structure address

  @retval EFI_SUCCESS         The service was properly initialized
  @retval other               A failure occurred during the service initialization

**/
EFI_STATUS
EFIAPI
EslIpInitialize4 (
  IN DT_SERVICE * pService
  )
{
  DT_LAYER * pLayer;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Identify the service
  //
  pService->NetworkType = NETWORK_TYPE_IP4;

  //
  //  Connect this service to the service list
  //
  pLayer = &mEslLayer;
  pService->pNext = pLayer->pIp4List;
  pLayer->pIp4List = pService;

  //
  //  Assume the list is empty
  //
  Status = EFI_SUCCESS;

  //
  //  Return the initialization status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Get the option value

  Retrieve the protocol options one at a time by name.

  @param [in] pSocket           Address of a DT_SOCKET structure
  @param [in] level             Option protocol level
  @param [in] OptionName        Name of the option
  @param [out] ppOptionData     Buffer to receive address of option value
  @param [out] pOptionLength    Buffer to receive the option length

  @retval EFI_SUCCESS - Socket data successfully received

 **/
EFI_STATUS
EslIpOptionGet4 (
  IN DT_SOCKET * pSocket,
  IN int level,
  IN int OptionName,
  OUT CONST void ** __restrict ppOptionData,
  OUT socklen_t * __restrict pOptionLength
  )
{
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  pSocket->errno = 0;
  Status = EFI_SUCCESS;

  //
  //  Attempt to get the option
  //
  switch ( level ) {
  default:
    //
    //  Protocol level not supported
    //
    pSocket->errno = ENOTSUP;
    Status = EFI_UNSUPPORTED;
    break;

  case IPPROTO_IP:
    switch ( OptionName ) {
    default:
      //
      //  Option not supported
      //
      pSocket->errno = ENOTSUP;
      Status = EFI_UNSUPPORTED;
      break;

    case IP_HDRINCL:
      *ppOptionData = (void *)pSocket->bIncludeHeader;
      *pOptionLength = sizeof ( pSocket->bIncludeHeader );
      break;
    }
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Set the option value

  Adjust the protocol options one at a time by name.

  @param [in] pSocket         Address of a DT_SOCKET structure
  @param [in] level           Option protocol level
  @param [in] OptionName      Name of the option
  @param [in] pOptionValue    Buffer containing the option value
  @param [in] OptionLength    Length of the buffer in bytes

  @retval EFI_SUCCESS - Option successfully set

 **/
EFI_STATUS
EslIpOptionSet4 (
  IN DT_SOCKET * pSocket,
  IN int level,
  IN int OptionName,
  IN CONST void * pOptionValue,
  IN socklen_t OptionLength
  )
{
  BOOLEAN bTrueFalse;
  socklen_t LengthInBytes;
  UINT8 * pOptionData;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume failure
  //
  pSocket->errno = EINVAL;
  Status = EFI_INVALID_PARAMETER;

  //
  //  Determine if the option protocol matches
  //
  LengthInBytes = 0;
  pOptionData = NULL;
  switch ( level ) {
  default:
    //
    //  Protocol level not supported
    //
    DEBUG (( DEBUG_INFO | DEBUG_OPTION, "ERROR - Invalid option level\r\n" ));
    pSocket->errno = ENOTSUP;
    Status = EFI_UNSUPPORTED;
    break;

  case IPPROTO_IP:
    switch ( OptionName ) {
    default:
      //
      //  Protocol level not supported
      //
      DEBUG (( DEBUG_INFO | DEBUG_OPTION, "ERROR - Invalid protocol option\r\n" ));
      pSocket->errno = ENOTSUP;
      Status = EFI_UNSUPPORTED;
      break;

    case IP_HDRINCL:

      //
      //  Validate the option length
      //
      if ( sizeof ( UINT32 ) == OptionLength ) {
        //
        //  Restrict the input to TRUE or FALSE
        //
        bTrueFalse = TRUE;
        if ( 0 == *(UINT32 *)pOptionValue ) {
          bTrueFalse = FALSE;
        }
        pOptionValue = &bTrueFalse;

        //
        //  Set the option value
        //
        pOptionData = (UINT8 *)&pSocket->bIncludeHeader;
        LengthInBytes = sizeof ( pSocket->bIncludeHeader );
      }
      break;
      
    }
    break;
  }

  //
  //  Validate the option length
  //
  if ( LengthInBytes <= OptionLength ) {
    //
    //  Set the option value
    //
    if ( NULL != pOptionData ) {
      CopyMem ( pOptionData, pOptionValue, LengthInBytes );
      pSocket->errno = 0;
      Status = EFI_SUCCESS;
    }
  }
  
  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}



/**
  Close a IP4 port.

  This routine releases the resources allocated by
  ::IpPortAllocate4().
  
  @param [in] pPort       Address of the port structure.

  @retval EFI_SUCCESS     The port is closed
  @retval other           Port close error

**/
EFI_STATUS
EslIpPortClose4 (
  IN DT_PORT * pPort
  )
{
  UINTN DebugFlags;
  DT_LAYER * pLayer;
  DT_PACKET * pPacket;
  DT_PORT * pPreviousPort;
  DT_SERVICE * pService;
  DT_SOCKET * pSocket;
  EFI_SERVICE_BINDING_PROTOCOL * pIp4Service;
  DT_IP4_CONTEXT * pIp4;
  EFI_STATUS Status;
  
  DBG_ENTER ( );

  //
  //  Verify the socket layer synchronization
  //
  VERIFY_TPL ( TPL_SOCKETS );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;
  pSocket = pPort->pSocket;
  pSocket->errno = 0;

  //
  //  Locate the port in the socket list
  //
  pLayer = &mEslLayer;
  DebugFlags = pPort->DebugFlags;
  pPreviousPort = pSocket->pPortList;
  if ( pPreviousPort == pPort ) {
    //
    //  Remove this port from the head of the socket list
    //
    pSocket->pPortList = pPort->pLinkSocket;
  }
  else {
    //
    //  Locate the port in the middle of the socket list
    //
    while (( NULL != pPreviousPort )
      && ( pPreviousPort->pLinkSocket != pPort )) {
      pPreviousPort = pPreviousPort->pLinkSocket;
    }
    if ( NULL != pPreviousPort ) {
      //
      //  Remove the port from the middle of the socket list
      //
      pPreviousPort->pLinkSocket = pPort->pLinkSocket;
    }
  }

  //
  //  Locate the port in the service list
  //
  pService = pPort->pService;
  pPreviousPort = pService->pPortList;
  if ( pPreviousPort == pPort ) {
    //
    //  Remove this port from the head of the service list
    //
    pService->pPortList = pPort->pLinkService;
  }
  else {
    //
    //  Locate the port in the middle of the service list
    //
    while (( NULL != pPreviousPort )
      && ( pPreviousPort->pLinkService != pPort )) {
      pPreviousPort = pPreviousPort->pLinkService;
    }
    if ( NULL != pPreviousPort ) {
      //
      //  Remove the port from the middle of the service list
      //
      pPreviousPort->pLinkService = pPort->pLinkService;
    }
  }

  //
  //  Empty the receive queue
  //
  ASSERT ( NULL == pSocket->pRxPacketListHead );
  ASSERT ( NULL == pSocket->pRxPacketListTail );
  ASSERT ( 0 == pSocket->RxBytes );

  //
  //  Empty the receive free queue
  //
  while ( NULL != pSocket->pRxFree ) {
    pPacket = pSocket->pRxFree;
    pSocket->pRxFree = pPacket->pNext;
    EslSocketPacketFree ( pPacket, DEBUG_RX );
  }

  //
  //  Done with the receive event
  //
  pIp4 = &pPort->Context.Ip4;
  if ( NULL != pIp4->RxToken.Event ) {
    Status = gBS->CloseEvent ( pIp4->RxToken.Event );
    if ( !EFI_ERROR ( Status )) {
      DEBUG (( DebugFlags | DEBUG_POOL,
                "0x%08x: Closed receive event\r\n",
                pIp4->RxToken.Event ));
    }
    else {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to close the receive event, Status: %r\r\n",
                Status ));
      ASSERT ( EFI_SUCCESS == Status );
    }
  }

  //
  //  Done with the transmit event
  //
  if ( NULL != pIp4->TxToken.Event ) {
    Status = gBS->CloseEvent ( pIp4->TxToken.Event );
    if ( !EFI_ERROR ( Status )) {
      DEBUG (( DebugFlags | DEBUG_POOL,
                "0x%08x: Closed normal transmit event\r\n",
                pIp4->TxToken.Event ));
    }
    else {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to close the normal transmit event, Status: %r\r\n",
                Status ));
      ASSERT ( EFI_SUCCESS == Status );
    }
  }

  //
  //  Done with the IP protocol
  //
  pIp4Service = pService->pInterface;
  if ( NULL != pIp4->pProtocol ) {
    Status = gBS->CloseProtocol ( pIp4->Handle,
                                  &gEfiIp4ProtocolGuid,
                                  pLayer->ImageHandle,
                                  NULL );
    if ( !EFI_ERROR ( Status )) {
      DEBUG (( DebugFlags,
                "0x%08x: gEfiIp4ProtocolGuid closed on controller 0x%08x\r\n",
                pIp4->pProtocol,
                pIp4->Handle ));
    }
    else {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to close gEfiIp4ProtocolGuid opened on controller 0x%08x, Status: %r\r\n",
                pIp4->Handle,
                Status ));
      ASSERT ( EFI_SUCCESS == Status );
    }
  }

  //
  //  Done with the IP port
  //
  if ( NULL != pIp4->Handle ) {
    Status = pIp4Service->DestroyChild ( pIp4Service,
                                          pIp4->Handle );
    if ( !EFI_ERROR ( Status )) {
      DEBUG (( DebugFlags | DEBUG_POOL,
                "0x%08x: Ip4 port handle destroyed\r\n",
                pIp4->Handle ));
    }
    else {
      DEBUG (( DEBUG_ERROR | DebugFlags | DEBUG_POOL,
                "ERROR - Failed to destroy the Ip4 port handle, Status: %r\r\n",
                Status ));
      ASSERT ( EFI_SUCCESS == Status );
    }
  }

  //
  //  Release the port structure
  //
  Status = gBS->FreePool ( pPort );
  if ( !EFI_ERROR ( Status )) {
    DEBUG (( DebugFlags | DEBUG_POOL,
              "0x%08x: Free pPort, %d bytes\r\n",
              pPort,
              sizeof ( *pPort )));
  }
  else {
    DEBUG (( DEBUG_ERROR | DebugFlags | DEBUG_POOL,
              "ERROR - Failed to free pPort: 0x%08x, Status: %r\r\n",
              pPort,
              Status ));
    ASSERT ( EFI_SUCCESS == Status );
  }

  //
  //  Mark the socket as closed if necessary
  //
  if ( NULL == pSocket->pPortList ) {
    pSocket->State = SOCKET_STATE_CLOSED;
    DEBUG (( DEBUG_CLOSE | DEBUG_INFO,
              "0x%08x: Socket State: SOCKET_STATE_CLOSED\r\n",
              pSocket ));
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Port close state 3

  Continue the close operation after the receive is complete.

  @param [in] pPort       Address of the port structure.

  @retval EFI_SUCCESS         The port is closed
  @retval EFI_NOT_READY       The port is still closing
  @retval EFI_ALREADY_STARTED Error, the port is in the wrong state,
                              most likely the routine was called already.

**/
EFI_STATUS
EslIpPortCloseRxDone4 (
  IN DT_PORT * pPort
  )
{
  PORT_STATE PortState;
  DT_SOCKET * pSocket;
  DT_IP4_CONTEXT * pIp4;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Verify the socket layer synchronization
  //
  VERIFY_TPL ( TPL_SOCKETS );

  //
  //  Verify that the port is closing
  //
  Status = EFI_ALREADY_STARTED;
  pSocket = pPort->pSocket;
  pSocket->errno = EALREADY;
  PortState = pPort->State;
  if (( PORT_STATE_CLOSE_TX_DONE == PortState )
    || ( PORT_STATE_CLOSE_DONE == PortState )) {
    //
    //  Determine if the receive operation is pending
    //
    Status = EFI_NOT_READY;
    pSocket->errno = EAGAIN;
    pIp4 = &pPort->Context.Ip4;
    if ( NULL == pIp4->pReceivePending ) {
      //
      //  The receive operation is complete
      //  Update the port state
      //
      pPort->State = PORT_STATE_CLOSE_RX_DONE;
      DEBUG (( DEBUG_CLOSE | DEBUG_INFO,
                "0x%08x: Port Close State: PORT_STATE_CLOSE_RX_DONE\r\n",
                pPort ));

      //
      //  The close operation has completed
      //  Release the port resources
      //
      Status = EslIpPortClose4 ( pPort );
    }
    else {
      DEBUG (( DEBUG_CLOSE | DEBUG_INFO,
                "0x%08x: Port Close: Receive still pending!\r\n",
                pPort ));
    }
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Start the close operation on a IP4 port, state 1.

  Closing a port goes through the following states:
  1. Port close starting - Mark the port as closing and wait for transmission to complete
  2. Port TX close done - Transmissions complete, close the port and abort the receives
  3. Port RX close done - Receive operations complete, close the port
  4. Port closed - Release the port resources
  
  @param [in] pPort       Address of the port structure.
  @param [in] bCloseNow   Set TRUE to abort active transfers
  @param [in] DebugFlags  Flags for debug messages

  @retval EFI_SUCCESS         The port is closed, not normally returned
  @retval EFI_NOT_READY       The port has started the closing process
  @retval EFI_ALREADY_STARTED Error, the port is in the wrong state,
                              most likely the routine was called already.

**/
EFI_STATUS
EslIpPortCloseStart4 (
  IN DT_PORT * pPort,
  IN BOOLEAN bCloseNow,
  IN UINTN DebugFlags
  )
{
  DT_SOCKET * pSocket;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Verify the socket layer synchronization
  //
  VERIFY_TPL ( TPL_SOCKETS );

  //
  //  Mark the port as closing
  //
  Status = EFI_ALREADY_STARTED;
  pSocket = pPort->pSocket;
  pSocket->errno = EALREADY;
  if ( PORT_STATE_CLOSE_STARTED > pPort->State ) {

    //
    //  Update the port state
    //
    pPort->State = PORT_STATE_CLOSE_STARTED;
    DEBUG (( DEBUG_CLOSE | DEBUG_INFO,
              "0x%08x: Port Close State: PORT_STATE_CLOSE_STARTED\r\n",
              pPort ));
    pPort->bCloseNow = bCloseNow;
    pPort->DebugFlags = DebugFlags;

    //
    //  Determine if transmits are complete
    //
    Status = EslIpPortCloseTxDone4 ( pPort );
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Port close state 2

  Continue the close operation after the transmission is complete.

  @param [in] pPort       Address of the port structure.

  @retval EFI_SUCCESS         The port is closed, not normally returned
  @retval EFI_NOT_READY       The port is still closing
  @retval EFI_ALREADY_STARTED Error, the port is in the wrong state,
                              most likely the routine was called already.

**/
EFI_STATUS
EslIpPortCloseTxDone4 (
  IN DT_PORT * pPort
  )
{
  DT_PACKET * pPacket;
  DT_SOCKET * pSocket;
  DT_IP4_CONTEXT * pIp4;
  EFI_IP4_PROTOCOL * pIp4Protocol;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Verify the socket layer synchronization
  //
  VERIFY_TPL ( TPL_SOCKETS );

  //
  //  All transmissions are complete or must be stopped
  //  Mark the port as TX complete
  //
  Status = EFI_ALREADY_STARTED;
  if ( PORT_STATE_CLOSE_STARTED == pPort->State ) {
    //
    //  Verify that the transmissions are complete
    //
    pSocket = pPort->pSocket;
    if ( pPort->bCloseNow
         || ( EFI_SUCCESS != pSocket->TxError )
         || ( 0 == pSocket->TxBytes )) {
      //
      //  Start the close operation on the port
      //
      pIp4 = &pPort->Context.Ip4;
      pIp4Protocol = pIp4->pProtocol;
      if ( !pIp4->bConfigured ) {
        //
        //  Skip the close operation since the port is not
        //  configured
        //
        //  Update the port state
        //
        pPort->State = PORT_STATE_CLOSE_DONE;
        DEBUG (( DEBUG_CLOSE | DEBUG_INFO,
                  "0x%08x: Port Close State: PORT_STATE_CLOSE_DONE\r\n",
                  pPort ));
        Status = EFI_SUCCESS;
      }
      else {
        //
        //  Update the port state
        //
        pPort->State = PORT_STATE_CLOSE_TX_DONE;
        DEBUG (( DEBUG_CLOSE | DEBUG_INFO,
                  "0x%08x: Port Close State: PORT_STATE_CLOSE_TX_DONE\r\n",
                  pPort ));

        //
        //  Empty the receive queue
        //
        while ( NULL != pSocket->pRxPacketListHead ) {
          pPacket = pSocket->pRxPacketListHead;
          pSocket->pRxPacketListHead = pPacket->pNext;
          pSocket->RxBytes -= pPacket->Op.Ip4Rx.pRxData->DataLength;

          //
          //  Return the buffer to the IP4 driver
          //
          gBS->SignalEvent ( pPacket->Op.Ip4Rx.pRxData->RecycleSignal );

          //
          //  Done with this packet
          //
          EslSocketPacketFree ( pPacket, DEBUG_RX );
        }
        pSocket->pRxPacketListTail = NULL;
        ASSERT ( 0 == pSocket->RxBytes );

        //
        //  Reset the port, cancel the outstanding receive
        //
        Status = pIp4Protocol->Configure ( pIp4Protocol,
                                            NULL );
        if ( !EFI_ERROR ( Status )) {
          DEBUG (( pPort->DebugFlags | DEBUG_CLOSE | DEBUG_INFO,
                    "0x%08x: Port reset\r\n",
                    pPort ));

          //
          //  Free the receive packet
          //
          Status = gBS->CheckEvent ( pIp4->RxToken.Event );
          if ( EFI_SUCCESS != Status ) {
            EslSocketPacketFree ( pIp4->pReceivePending, DEBUG_CLOSE );
            pIp4->pReceivePending = NULL;
            Status = EFI_SUCCESS;
          }
        }
        else {
          DEBUG (( DEBUG_ERROR | pPort->DebugFlags | DEBUG_CLOSE | DEBUG_INFO,
                   "ERROR - Port 0x%08x reset failed, Status: %r\r\n",
                   pPort,
                   Status ));
          ASSERT ( EFI_SUCCESS == Status );
        }
      }

      //
      //  Determine if the receive operation is pending
      //
      if ( !EFI_ERROR ( Status )) {
        Status = EslIpPortCloseRxDone4 ( pPort );
      }
    }
    else {
      //
      //  Transmissions are still active, exit
      //
      DEBUG (( DEBUG_CLOSE | DEBUG_INFO,
                "0x%08x: Port Close: Transmits are still pending!\r\n",
                pPort ));
      Status = EFI_NOT_READY;
      pSocket->errno = EAGAIN;
    }
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Receive data from a network connection.

  To minimize the number of buffer copies, the ::UdpRxComplete4
  routine queues the IP4 driver's buffer to a list of datagrams
  waiting to be received.  The socket driver holds on to the
  buffers from the IP4 driver until the application layer requests
  the data or the socket is closed.

  The application calls this routine in the socket layer to
  receive datagrams from one or more remote systems. This routine
  removes the next available datagram from the list of datagrams
  and copies the data from the IP4 driver's buffer into the
  application's buffer.  The IP4 driver's buffer is then returned.

  @param [in] pSocket         Address of a DT_SOCKET structure

  @param [in] Flags           Message control flags

  @param [in] BufferLength    Length of the the buffer

  @param [in] pBuffer         Address of a buffer to receive the data.

  @param [in] pDataLength     Number of received data bytes in the buffer.

  @param [out] pAddress       Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @retval EFI_SUCCESS - Socket data successfully received

**/
EFI_STATUS
EslIpReceive4 (
  IN DT_SOCKET * pSocket,
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
  size_t HeaderBytes;
  in_addr_t IpAddress;
  size_t LengthInBytes;
  UINT8 * pData;
  DT_PACKET * pPacket;
  DT_PORT * pPort;
  struct sockaddr_in * pRemoteAddress;
  EFI_IP4_RECEIVE_DATA * pRxData;
  DT_IP4_CONTEXT * pIp4;
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
  if (( SOCKET_STATE_CONNECTED == pSocket->State )
    || ( PORT_STATE_RX_ERROR == pSocket->State )) {
    //
    //  Locate the port
    //
    pPort = pSocket->pPortList;
    if ( NULL != pPort ) {
      //
      //  Determine if there is any data on the queue
      //
      pIp4 = &pPort->Context.Ip4;
      pPacket = pSocket->pRxPacketListHead;
      if ( NULL != pPacket ) {
        //
        //  Validate the return address parameters
        //
        pRxData = pPacket->Op.Ip4Rx.pRxData;
        if (( NULL == pAddress ) || ( NULL != pAddressLength )) {
          //
          //  Return the remote system address if requested
          //
          if ( NULL != pAddress ) {
            //
            //  Build the remote address
            //
            DEBUG (( DEBUG_RX,
                      "Getting packet source address: %d.%d.%d.%d\r\n",
                      pRxData->Header->SourceAddress.Addr[0],
                      pRxData->Header->SourceAddress.Addr[1],
                      pRxData->Header->SourceAddress.Addr[2],
                      pRxData->Header->SourceAddress.Addr[3]));
            ZeroMem ( &RemoteAddress, sizeof ( RemoteAddress ));
            RemoteAddress.sin_len = sizeof ( RemoteAddress );
            RemoteAddress.sin_family = AF_INET;
            IpAddress = pRxData->Header->SourceAddress.Addr[3];
            IpAddress <<= 8;
            IpAddress |= pRxData->Header->SourceAddress.Addr[2];
            IpAddress <<= 8;
            IpAddress |= pRxData->Header->SourceAddress.Addr[1];
            IpAddress <<= 8;
            IpAddress |= pRxData->Header->SourceAddress.Addr[0];
            RemoteAddress.sin_addr.s_addr = IpAddress;

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
          DataBytes = pRxData->HeaderLength + pRxData->DataLength;
          if ( DataBytes < BufferLength ) {
            BufferLength = DataBytes;
          }

          //
          //  Copy the IP header
          //
          LengthInBytes = 0;
          HeaderBytes = pRxData->HeaderLength;
          if ( HeaderBytes > BufferLength ) {
            HeaderBytes = BufferLength;
          }
          DEBUG (( DEBUG_RX,
                    "0x%08x: Port copy header 0x%08x data into 0x%08x, 0x%08x bytes\r\n",
                    pPort,
                    pRxData->Header,
                    pBuffer,
                    HeaderBytes ));
          CopyMem ( pBuffer, pRxData->Header, HeaderBytes );
          pBuffer += HeaderBytes;
          LengthInBytes += HeaderBytes;
          
          //
          //  Copy the received data
          //
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
            //  Return this packet to the IP4 driver
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
            if (( NULL == pIp4->pReceivePending )
              && ( MAX_RX_DATA > pSocket->RxBytes )) {
                EslIpRxStart4 ( pPort );
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
  Cancel the receive operations

  @param [in] pSocket         Address of a DT_SOCKET structure
  
  @retval EFI_SUCCESS - The cancel was successful

 **/
EFI_STATUS
EslIpRxCancel4 (
  IN DT_SOCKET * pSocket
  )
{
  DT_PACKET * pPacket;
  DT_PORT * pPort;
  DT_IP4_CONTEXT * pIp4;
  EFI_IP4_PROTOCOL * pIp4Protocol;
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
    pIp4 = &pPort->Context.Ip4;
    pPacket = pIp4->pReceivePending;
    if ( NULL != pPacket ) {
      //
      //  Attempt to cancel the receive operation
      //
      pIp4Protocol = pIp4->pProtocol;
      Status = pIp4Protocol->Cancel ( pIp4Protocol,
                                       &pIp4->RxToken );
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

  Keep the IP4 driver's buffer and append it to the list of
  datagrams for the application to receive.  The IP4 driver's
  buffer will be returned by either ::IpReceive4 or
  ::IpPortCloseTxDone4.

  @param  Event         The receive completion event

  @param  pPort         The DT_PORT structure address

**/
VOID
EslIpRxComplete4 (
  IN EFI_EVENT Event,
  IN DT_PORT * pPort
  )
{
  size_t LengthInBytes;
  DT_PACKET * pPacket;
  DT_PACKET * pPrevious;
  EFI_IP4_RECEIVE_DATA * pRxData;
  DT_SOCKET * pSocket;
  DT_IP4_CONTEXT * pIp4;
  EFI_STATUS Status;
  
  DBG_ENTER ( );
  
  //
  //  Mark this receive complete
  //
  pIp4 = &pPort->Context.Ip4;
  pPacket = pIp4->pReceivePending;
  pIp4->pReceivePending = NULL;
  
  //
  //  Determine if this receive was successful
  //
  pSocket = pPort->pSocket;
  Status = pIp4->RxToken.Status;
  if (( !EFI_ERROR ( Status )) && ( !pSocket->bRxDisable )) {
    pRxData = pIp4->RxToken.Packet.RxData;
    if ( PORT_STATE_CLOSE_STARTED >= pPort->State ) {
      //
      //  Save the data in the packet
      //
      pPacket->Op.Ip4Rx.pRxData = pRxData;

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
      LengthInBytes = pRxData->HeaderLength + pRxData->DataLength;
      pSocket->RxBytes += LengthInBytes;

      //
      //  Log the received data
      //
      DEBUG (( DEBUG_RX | DEBUG_INFO,
                "Received packet from: %d.%d.%d.%d\r\n",
                pRxData->Header->SourceAddress.Addr[0],
                pRxData->Header->SourceAddress.Addr[1],
                pRxData->Header->SourceAddress.Addr[2],
                pRxData->Header->SourceAddress.Addr[3]));
      DEBUG (( DEBUG_RX | DEBUG_INFO,
                "Received packet sent to: %d.%d.%d.%d\r\n",
                pRxData->Header->DestinationAddress.Addr[0],
                pRxData->Header->DestinationAddress.Addr[1],
                pRxData->Header->DestinationAddress.Addr[2],
                pRxData->Header->DestinationAddress.Addr[3]));
      DEBUG (( DEBUG_RX | DEBUG_INFO,
                "0x%08x: Packet queued on port 0x%08x with 0x%08x bytes of data\r\n",
                pPacket,
                pPort,
                LengthInBytes ));

      //
      //  Attempt to restart this receive operation
      //
      if ( pSocket->MaxRxBuf > pSocket->RxBytes ) {
        EslIpRxStart4 ( pPort );
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
      //  Return the buffer to the IP4 driver
      //
      gBS->SignalEvent ( pRxData->RecycleSignal );

      //
      //  Free the packet
      //
      EslSocketPacketFree ( pPacket, DEBUG_RX );
    }
  }
  else
  {
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
      EslIpPortCloseRxDone4 ( pPort );
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

  @param [in] pPort       Address of the DT_PORT structure.

 **/
VOID
EslIpRxStart4 (
  IN DT_PORT * pPort
  )
{
  DT_PACKET * pPacket;
  DT_SOCKET * pSocket;
  DT_IP4_CONTEXT * pIp4;
  EFI_IP4_PROTOCOL * pIp4Protocol;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Determine if a receive is already pending
  //
  Status = EFI_SUCCESS;
  pPacket = NULL;
  pSocket = pPort->pSocket;
  pIp4 = &pPort->Context.Ip4;
  if ( !EFI_ERROR ( pPort->pSocket->RxError )) {
    if (( NULL == pIp4->pReceivePending )
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
                                           sizeof ( pPacket->Op.Ip4Rx ),
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
        pPacket->Op.Ip4Rx.pRxData = NULL;
        pIp4->RxToken.Packet.RxData = NULL;
        pIp4->pReceivePending = pPacket;

        //
        //  Start the receive on the packet
        //
        pIp4Protocol = pIp4->pProtocol;
        Status = pIp4Protocol->Receive ( pIp4Protocol,
                                          &pIp4->RxToken );
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
          pIp4->pReceivePending = NULL;
          pPacket->pNext = pSocket->pRxFree;
          pSocket->pRxFree = pPacket;
        }
      }
    }
  }

  DBG_EXIT ( );
}


/**
  Shutdown the IP4 service.

  This routine undoes the work performed by ::IpInitialize4.

  @param [in] pService        DT_SERVICE structure address

**/
VOID
EFIAPI
EslIpShutdown4 (
  IN DT_SERVICE * pService
  )
{
  DT_LAYER * pLayer;
  DT_PORT * pPort;
  DT_SERVICE * pPreviousService;

  DBG_ENTER ( );

  //
  //  Verify the socket layer synchronization
  //
  VERIFY_TPL ( TPL_SOCKETS );

  //
  //  Walk the list of ports
  //
  do {
    pPort = pService->pPortList;
    if ( NULL != pPort ) {
      //
      //  Remove the port from the port list
      //
      pService->pPortList = pPort->pLinkService;

      //
      //  Close the port
      // TODO: Fix this
      //
//      pPort->pfnClosePort ( pPort, 0 );
    }
  } while ( NULL != pPort );

  //
  //  Remove the service from the service list
  //
  pLayer = &mEslLayer;
  pPreviousService = pLayer->pIp4List;
  if ( pService == pPreviousService ) {
    //
    //  Remove the service from the beginning of the list
    //
    pLayer->pIp4List = pService->pNext;
  }
  else {
    //
    //  Remove the service from the middle of the list
    //
    while ( NULL != pPreviousService ) {
      if ( pService == pPreviousService->pNext ) {
        pPreviousService->pNext = pService->pNext;
        break;
      }
    }
  }

  DBG_EXIT ( );
}


/**
  Determine if the sockedt is configured.


  @param [in] pSocket         Address of a DT_SOCKET structure
  
  @retval EFI_SUCCESS - The port is connected
  @retval EFI_NOT_STARTED - The port is not connected

 **/
 EFI_STATUS
 EslIpSocketIsConfigured4 (
  IN DT_SOCKET * pSocket
  )
{
  DT_PORT * pPort;
  DT_PORT * pNextPort;
  DT_IP4_CONTEXT * pIp4;
  EFI_IP4_PROTOCOL * pIp4Protocol;
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
      Status = EslIpBind4 ( pSocket,
                             (struct sockaddr *)&LocalAddress,
                             LocalAddress.sin_len );
    }

    //
    //  Walk the port list
    //
    pPort = pSocket->pPortList;
    while ( NULL != pPort ) {
      //
      //  Update the raw setting
      //
      pIp4 = &pPort->Context.Ip4;
      if ( pSocket->bIncludeHeader ) {
        //
        //  IP header will be included with the data on transmit
        //
        pIp4->ModeData.ConfigData.RawData = TRUE;
      }

      //
      //  Attempt to configure the port
      //
      pNextPort = pPort->pLinkSocket;
      pIp4Protocol = pIp4->pProtocol;
      DEBUG (( DEBUG_TX,
                "0x%08x: pPort Configuring for %d.%d.%d.%d --> %d.%d.%d.%d\r\n",
                          pPort,
                          pIp4->ModeData.ConfigData.StationAddress.Addr[0],
                          pIp4->ModeData.ConfigData.StationAddress.Addr[1],
                          pIp4->ModeData.ConfigData.StationAddress.Addr[2],
                          pIp4->ModeData.ConfigData.StationAddress.Addr[3],
                          pIp4->DestinationAddress.Addr[0],
                          pIp4->DestinationAddress.Addr[1],
                          pIp4->DestinationAddress.Addr[2],
                          pIp4->DestinationAddress.Addr[3]));
      Status = pIp4Protocol->Configure ( pIp4Protocol,
                                          &pIp4->ModeData.ConfigData );
      if ( !EFI_ERROR ( Status )) {
        //
        //  Update the configuration data
        //
        Status = pIp4Protocol->GetModeData ( pIp4Protocol,
                                             &pIp4->ModeData,
                                             NULL,
                                             NULL );
      }
      if ( EFI_ERROR ( Status )) {
        DEBUG (( DEBUG_LISTEN,
                  "ERROR - Failed to configure the Ip4 port, Status: %r\r\n",
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
        DEBUG (( DEBUG_LISTEN,
                  "0x%08x: Port configured\r\n",
                  pPort ));
        pIp4->bConfigured = TRUE;

        //
        //  Start the first read on the port
        //
        EslIpRxStart4 ( pPort );

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

  This routine is called by the socket layer API to buffer
  data for transmission.  The data is copied into a local buffer
  freeing the application buffer for reuse upon return.  When
  necessary, this routine will start the transmit engine that
  performs the data transmission on the network connection.  The
  transmit engine transmits the data a packet at a time over the
  network connection.

  Transmission errors are returned during the next transmission or
  during the close operation.  Only buffering errors are returned
  during the current transmission attempt.

  @param [in] pSocket         Address of a DT_SOCKET structure

  @param [in] Flags           Message control flags

  @param [in] BufferLength    Length of the the buffer

  @param [in] pBuffer         Address of a buffer to receive the data.

  @param [in] pDataLength     Number of received data bytes in the buffer.

  @param [in] pAddress        Network address of the remote system address

  @param [in] AddressLength   Length of the remote network address structure

  @retval EFI_SUCCESS - Socket data successfully buffered

**/
EFI_STATUS
EslIpTxBuffer4 (
  IN DT_SOCKET * pSocket,
  IN int Flags,
  IN size_t BufferLength,
  IN CONST UINT8 * pBuffer,
  OUT size_t * pDataLength,
  IN const struct sockaddr * pAddress,
  IN socklen_t AddressLength
  )
{
  DT_PACKET * pPacket;
  DT_PACKET * pPreviousPacket;
  DT_PACKET ** ppPacket;
  DT_PORT * pPort;
  const struct sockaddr_in * pRemoteAddress;
  DT_IP4_CONTEXT * pIp4;
  EFI_IP4_COMPLETION_TOKEN * pToken;
  size_t * pTxBytes;
  DT_IP4_TX_DATA * pTxData;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Assume failure
  //
  Status = EFI_UNSUPPORTED;
  pSocket->errno = ENOTCONN;
  * pDataLength = 0;

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
      pIp4 = &pPort->Context.Ip4;
      ppPacket = &pIp4->pTxPacket;
      pToken = &pIp4->TxToken;
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
                                           sizeof ( pPacket->Op.Ip4Tx )
                                           - sizeof ( pPacket->Op.Ip4Tx.Buffer )
                                           + BufferLength,
                                           DEBUG_TX );
        if ( !EFI_ERROR ( Status )) {
          //
          //  Initialize the transmit operation
          //
          pTxData = &pPacket->Op.Ip4Tx;
          pTxData->TxData.DestinationAddress.Addr[0] = pIp4->DestinationAddress.Addr[0];
          pTxData->TxData.DestinationAddress.Addr[1] = pIp4->DestinationAddress.Addr[1];
          pTxData->TxData.DestinationAddress.Addr[2] = pIp4->DestinationAddress.Addr[2];
          pTxData->TxData.DestinationAddress.Addr[3] = pIp4->DestinationAddress.Addr[3];
          pTxData->TxData.OverrideData = NULL;
          pTxData->TxData.OptionsLength = 0;
          pTxData->TxData.OptionsBuffer = NULL;
          pTxData->TxData.TotalDataLength = (UINT32) BufferLength;
          pTxData->TxData.FragmentCount = 1;
          pTxData->TxData.FragmentTable[0].FragmentLength = (UINT32) BufferLength;
          pTxData->TxData.FragmentTable[0].FragmentBuffer = &pPacket->Op.Ip4Tx.Buffer[0];

          //
          //  Set the remote system address if necessary
          //
          if ( NULL != pAddress ) {
            pRemoteAddress = (const struct sockaddr_in *)pAddress;
            pTxData->Override.SourceAddress.Addr[0] = pIp4->ModeData.ConfigData.StationAddress.Addr[0];
            pTxData->Override.SourceAddress.Addr[1] = pIp4->ModeData.ConfigData.StationAddress.Addr[1];
            pTxData->Override.SourceAddress.Addr[2] = pIp4->ModeData.ConfigData.StationAddress.Addr[2];
            pTxData->Override.SourceAddress.Addr[3] = pIp4->ModeData.ConfigData.StationAddress.Addr[3];
            pTxData->TxData.DestinationAddress.Addr[0] = (UINT8)pRemoteAddress->sin_addr.s_addr;
            pTxData->TxData.DestinationAddress.Addr[1] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 8 );
            pTxData->TxData.DestinationAddress.Addr[2] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 16 );
            pTxData->TxData.DestinationAddress.Addr[3] = (UINT8)( pRemoteAddress->sin_addr.s_addr >> 24 );
            pTxData->Override.GatewayAddress.Addr[0] = 0;
            pTxData->Override.GatewayAddress.Addr[1] = 0;
            pTxData->Override.GatewayAddress.Addr[2] = 0;
            pTxData->Override.GatewayAddress.Addr[3] = 0;
            pTxData->Override.Protocol = (UINT8)pSocket->Protocol;
            pTxData->Override.TypeOfService = 0;
            pTxData->Override.TimeToLive = 255;
            pTxData->Override.DoNotFragment = FALSE;

            //
            //  Use the remote system address when sending this packet
            //
            pTxData->TxData.OverrideData = &pTxData->Override;
          }

          //
          //  Copy the data into the buffer
          //
          CopyMem ( &pPacket->Op.Ip4Tx.Buffer[0],
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
            if ( NULL == pIp4->pTxPacket ) {
              EslIpTxStart4 ( pSocket->pPortList );
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

  @param  Event         The normal transmit completion event

  @param  pPort         The DT_PORT structure address

**/
VOID
EslIpTxComplete4 (
  IN EFI_EVENT Event,
  IN DT_PORT * pPort
  )
{
  UINT32 LengthInBytes;
  DT_PACKET * pCurrentPacket;
  DT_PACKET * pNextPacket;
  DT_PACKET * pPacket;
  DT_SOCKET * pSocket;
  DT_IP4_TX_DATA * pTxData;
  DT_IP4_CONTEXT * pIp4;
  EFI_STATUS Status;
  
  DBG_ENTER ( );
  
  //
  //  Locate the active transmit packet
  //
  pSocket = pPort->pSocket;
  pIp4 = &pPort->Context.Ip4;
  pPacket = pIp4->pTxPacket;
  pTxData = &pPacket->Op.Ip4Tx;
  Status = pIp4->TxToken.Status;
  
  //
  //  Mark this packet as complete
  //
  pIp4->pTxPacket = NULL;
  LengthInBytes = pPacket->Op.Ip4Tx.TxData.TotalDataLength;
  pSocket->TxBytes -= LengthInBytes;

  //
  //  Save any transmit error
  //
  if ( EFI_ERROR ( Status )) {
    if ( !EFI_ERROR ( pSocket->TxError )) {
      pSocket->TxError = Status;
    }
    DEBUG (( DEBUG_TX | DEBUG_INFO,
              "ERROR - Transmit failure for packet 0x%08x on pPort 0x%08x, Status: %r\r\n",
              pPacket,
              pPort,
              Status ));

    //
    //  Empty the normal transmit list
    //
    pCurrentPacket = pPacket;
    pNextPacket = pSocket->pTxPacketListHead;
    while ( NULL != pNextPacket ) {
      pPacket = pNextPacket;
      pNextPacket = pPacket->pNext;
      EslSocketPacketFree ( pPacket, DEBUG_TX );
    }
    pSocket->pTxPacketListHead = NULL;
    pSocket->pTxPacketListTail = NULL;
    pPacket = pCurrentPacket;
  }
  else
  {
    DEBUG (( DEBUG_TX | DEBUG_INFO,
              "0x%08x: Packet transmitted %d bytes successfully\r\n",
              pPacket,
              LengthInBytes ));

    //
    //  Verify the transmit engine is still running
    //
    if ( !pPort->bCloseNow ) {
      //
      //  Start the next packet transmission
      //
      EslIpTxStart4 ( pPort );
    }
  }

  //
  //  Release this packet
  //
  EslSocketPacketFree ( pPacket, DEBUG_TX );

  //
  //  Finish the close operation if necessary
  //
  if (( PORT_STATE_CLOSE_STARTED <= pPort->State )
    && ( NULL == pSocket->pTxPacketListHead )
    && ( NULL == pIp4->pTxPacket )) {
    //
    //  Indicate that the transmit is complete
    //
    EslIpPortCloseTxDone4 ( pPort );
  }
  DBG_EXIT ( );
}


/**
  Transmit data using a network connection.

  @param [in] pPort           Address of a DT_PORT structure

 **/
VOID
EslIpTxStart4 (
  IN DT_PORT * pPort
  )
{
  DT_PACKET * pNextPacket;
  DT_PACKET * pPacket;
  DT_SOCKET * pSocket;
  DT_IP4_CONTEXT * pIp4;
  EFI_IP4_PROTOCOL * pIp4Protocol;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Get the packet from the queue head
  //
  pSocket = pPort->pSocket;
  pPacket = pSocket->pTxPacketListHead;
  if ( NULL != pPacket ) {
    //
    //  Remove the packet from the queue
    //
    pNextPacket = pPacket->pNext;
    pSocket->pTxPacketListHead = pNextPacket;
    if ( NULL == pNextPacket ) {
      pSocket->pTxPacketListTail = NULL;
    }

    //
    //  Set the packet as active
    //
    pIp4 = &pPort->Context.Ip4;
    pIp4->pTxPacket = pPacket;

    //
    //  Start the transmit operation
    //
    pIp4Protocol = pIp4->pProtocol;
    pIp4->TxToken.Packet.TxData = &pPacket->Op.Ip4Tx.TxData;
    Status = pIp4Protocol->Transmit ( pIp4Protocol, &pIp4->TxToken );
    if ( EFI_ERROR ( Status )) {
      pSocket = pPort->pSocket;
      if ( EFI_SUCCESS == pSocket->TxError ) {
        pSocket->TxError = Status;
      }
    }
  }

  DBG_EXIT ( );
}
