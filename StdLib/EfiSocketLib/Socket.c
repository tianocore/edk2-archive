/** @file
  Implement the socket support for the socket layer.

  Socket States:
  * Bound - pSocket->PortList is not NULL
  * Listen - AcceptWait event is not NULL

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


  \section TransmitEngine Transmit Engine
  The transmit engine uses the ESL_IO_MGMT structures to manage
  multiple transmit buffers.  The network specific PortAllocate
  routine allocate the ::ESL_IO_MGMT structures and place them
  on the free list by calling ::EslSocketIoInit.  During their
  lifetime, the ESL_IO_MGMT structures will move from the free
  list to the active list and back again.  The active list contains
  the packets that are actively being processed by the network
  stack.  Eventually the ESL_IO_MGMT structures will be removed
  from the free list and be deallocated by the network specific
  PortClose routines.

  The network specific code calls the ::EslSocketTxStart routine
  to hand a packet to the network stack.  EslSocketTxStart connects
  the transmit packet (::ESL_PACKET) to an ::ESL_IO_MGMT structure
  and then queues the result to one of the active lists:
  ESL_PORT::pTxActive or ESL_PORT::pTxOobActive.  The routine then
  hands the packet to the network stack.

  Upon completion, the network specific TxComplete routine calls
  ::EslSocketTxComplete to disconnect the transmit packet from the
  ESL_IO_MGMT structure.  The routine also releases the structure
  to to the appropriate free queue: ESL_PORT::pTxFree or
  ESL_PORT::pTxOobFree.  The network specific PortClose routine
  calls ::EslSocketIoFree to deallocate the ESL_IO_MGMT structures.

**/

#include "Socket.h"


/**
  Socket driver connection points

  List the network stack connection points for the socket driver.
**/
CONST ESL_SOCKET_BINDING cEslSocketBinding[] = {
  { L"Ip4",
    &gEfiIp4ServiceBindingProtocolGuid,
    &mEslIp4ServiceGuid,
    EslIp4Initialize,
    EslIp4Shutdown,
    4,
    0 },
  { L"Tcp4",
    &gEfiTcp4ServiceBindingProtocolGuid,
    &mEslTcp4ServiceGuid,
    EslTcp4Initialize,
    EslTcp4Shutdown,
    4,
    4 },
  { L"Udp4",
    &gEfiUdp4ServiceBindingProtocolGuid,
    &mEslUdp4ServiceGuid,
    EslUdp4Initialize,
    EslUdp4Shutdown,
    4,
    0 }
};

CONST UINTN cEslSocketBindingEntries = DIM ( cEslSocketBinding );

/**
  APIs to support the various socket types for the v4 network stack.
**/
CONST ESL_PROTOCOL_API * cEslAfInetApi[] = {
  NULL,             //  0
  &cEslTcp4Api,     //  SOCK_STREAM
  &cEslUdp4Api,     //  SOCK_DGRAM
  &cEslIp4Api,      //  SOCK_RAW
  NULL,             //  SOCK_RDM
  &cEslTcp4Api      //  SOCK_SEQPACKET
};

/**
  Number of entries in the v4 API array ::cEslAfInetApi.
**/
CONST int cEslAfInetApiSize = DIM ( cEslAfInetApi );


/**
  APIs to support the various socket types for the v6 network stack.
**/
CONST ESL_PROTOCOL_API * cEslAfInet6Api[] = {
  NULL,             //  0
  NULL,             //  SOCK_STREAM
  NULL,             //  SOCK_DGRAM
  NULL,             //  SOCK_RAW
  NULL,             //  SOCK_RDM
  NULL              //  SOCK_SEQPACKET
};

/**
  Number of entries in the v6 API array ::cEslAfInet6Api.
**/
CONST int cEslAfInet6ApiSize = DIM ( cEslAfInet6Api );


/**
  Global management structure for the socket layer.
**/
ESL_LAYER mEslLayer;


/**
  Initialize an endpoint for network communication.

  This routine initializes the communication endpoint.

  The ::socket routine calls this routine indirectly to create
  the communication endpoint.

  @param [in] pSocketProtocol Address of the socket protocol structure.
  @param [in] domain    Select the family of protocols for the client or server
                        application.  See the ::socket documentation for values.
  @param [in] type      Specifies how to make the network connection.
                        See the ::socket documentation for values.
  @param [in] protocol  Specifies the lower layer protocol to use.
                        See the ::socket documentation for values.
  @param [out] pErrno   Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Socket successfully created
  @retval EFI_INVALID_PARAMETER - Invalid domain value, errno = EAFNOSUPPORT
  @retval EFI_INVALID_PARAMETER - Invalid type value, errno = EINVAL
  @retval EFI_INVALID_PARAMETER - Invalid protocol value, errno = EINVAL

 **/
EFI_STATUS
EslSocket (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN int domain,
  IN int type,
  IN int protocol,
  IN int * pErrno
  )
{
  CONST ESL_PROTOCOL_API * pApi;
  CONST ESL_PROTOCOL_API ** ppApiArray;
  int ApiArraySize;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  int errno;

  DBG_ENTER ( );

  //
  //  Locate the socket
  //
  pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

  //
  //  Set the default domain if necessary
  //
  if ( AF_UNSPEC == domain ) {
    domain = AF_INET;
  }

  //
  //  Assume success
  //
  errno = 0;
  Status = EFI_SUCCESS;

  //
  //  Use break instead of goto
  //
  for ( ; ; ) {
    //
    //  Validate the domain value
    //
    if (( AF_INET != domain )
      && ( AF_LOCAL != domain )) {
      DEBUG (( DEBUG_ERROR | DEBUG_SOCKET,
                "ERROR - Invalid domain value" ));
      Status = EFI_INVALID_PARAMETER;
      errno = EAFNOSUPPORT;
      break;
    }

    //
    //  Determine the protocol APIs
    //
    ppApiArray = NULL;
    ApiArraySize = 0;
    if (( AF_INET == domain )
      || ( AF_LOCAL == domain )) {
      ppApiArray = &cEslAfInetApi[0];
      ApiArraySize = cEslAfInetApiSize;
    }
    else {
      ppApiArray = &cEslAfInet6Api[0];
      ApiArraySize = cEslAfInet6ApiSize;
    }

    //
    //  Set the default type if necessary
    //
    if ( 0 == type ) {
      type = SOCK_STREAM;
    }

    //
    //  Validate the type value
    //
    if (( type >= ApiArraySize )
      || ( NULL == ppApiArray )
      || ( NULL == ppApiArray[ type ])) {
      DEBUG (( DEBUG_ERROR | DEBUG_SOCKET,
                "ERROR - Invalid type value\r\n" ));
      Status = EFI_INVALID_PARAMETER;
      errno = EINVAL;
      break;
    }

    //
    //  Set the default protocol if necessary
    //
    pApi = ppApiArray[ type ];
    if ( 0 == protocol ) {
      protocol = pApi->DefaultProtocol;
    }

    //
    //  Validate the protocol value
    //
    if (( pApi->DefaultProtocol != protocol )
      && ( SOCK_RAW != type )) {
      DEBUG (( DEBUG_ERROR | DEBUG_SOCKET,
                "ERROR - Invalid protocol value" ));
      Status = EFI_INVALID_PARAMETER;
      errno = EINVAL;
      break;
    }

    //
    //  Save the socket attributes
    //
    pSocket->pApi = pApi;
    pSocket->Domain = domain;
    pSocket->Type = type;
    pSocket->Protocol = protocol;

    //
    //  Done
    //
    break;
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    *pErrno = errno;
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Accept a network connection.

  This routine calls the network specific layer to remove the next
  connection from the FIFO.

  The ::accept calls this routine to poll for a network
  connection to the socket.  When a connection is available
  this routine returns the ::EFI_SOCKET_PROTOCOL structure address
  associated with the new socket and the remote network address
  if requested.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.

  @param [in] pSockAddr       Address of a buffer to receive the remote
                              network address.

  @param [in, out] pSockAddrLength  Length in bytes of the address buffer.
                                    On output specifies the length of the
                                    remote network address.

  @param [out] ppSocketProtocol Address of a buffer to receive the
                                ::EFI_SOCKET_PROTOCOL instance
                                associated with the new socket.

  @param [out] pErrno   Address to receive the errno value upon completion.

  @retval EFI_SUCCESS   New connection successfully created
  @retval EFI_NOT_READY No connection is available

 **/
EFI_STATUS
EslSocketAccept (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN struct sockaddr * pSockAddr,
  IN OUT socklen_t * pSockAddrLength,
  IN EFI_SOCKET_PROTOCOL ** ppSocketProtocol,
  IN int * pErrno
  )
{
  ESL_SOCKET * pNewSocket;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Validate the socket
  //
  pSocket = NULL;
  pNewSocket = NULL;
  if ( NULL != pSocketProtocol ) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

    //
    //  Verify the API
    //
    if ( NULL == pSocket->pApi->pfnAccept ) {
      Status = EFI_UNSUPPORTED;
      pSocket->errno = ENOTSUP;
    }
    else {
      //
      //  Validate the sockaddr
      //
      if (( NULL != pSockAddr )
        && ( NULL == pSockAddrLength )) {
        DEBUG (( DEBUG_ACCEPT,
                  "ERROR - pSockAddr is NULL!\r\n" ));
        Status = EFI_INVALID_PARAMETER;
        pSocket->errno = EFAULT;
      }
      else {
        //
        //  Synchronize with the socket layer
        //
        RAISE_TPL ( TplPrevious, TPL_SOCKETS );

        //
        //  Verify that the socket is in the listen state
        //
        if ( SOCKET_STATE_LISTENING != pSocket->State ) {
          DEBUG (( DEBUG_ACCEPT,
                    "ERROR - Socket is not listening!\r\n" ));
          Status = EFI_NOT_STARTED;
          pSocket->errno = EOPNOTSUPP;
        }
        else {
          //
          //  Determine if a socket is available
          //
          if ( 0 == pSocket->FifoDepth ) {
            //
            //  No connections available
            //  Determine if any ports are available
            //
            if ( NULL == pSocket->pPortList ) {
              //
              //  No ports available
              //
              Status = EFI_DEVICE_ERROR;
              pSocket->errno = EINVAL;

              //
              //  Update the socket state
              //
              pSocket->State = SOCKET_STATE_NO_PORTS;
            }
            else {
              //
              //  Ports are available
              //  No connection requests at this time
              //
              Status = EFI_NOT_READY;
              pSocket->errno = EAGAIN;
            }
          }
          else {

            //
            //  Attempt to accept the connection and
            //  get the remote network address
            //
            pNewSocket = pSocket->pFifoHead;
            ASSERT ( NULL != pNewSocket );
            Status = pSocket->pApi->pfnAccept ( pNewSocket,
                                                pSockAddr,
                                                pSockAddrLength );
            if ( !EFI_ERROR ( Status )) {
              //
              //  Remove the new socket from the list
              //
              pSocket->pFifoHead = pNewSocket->pNextConnection;
              if ( NULL == pSocket->pFifoHead ) {
                pSocket->pFifoTail = NULL;
              }

              //
              //  Account for this socket
              //
              pSocket->FifoDepth -= 1;

              //
              //  Update the new socket's state
              //
              pNewSocket->State = SOCKET_STATE_CONNECTED;
              pNewSocket->bConfigured = TRUE;
              DEBUG (( DEBUG_ACCEPT,
                        "0x%08x: Socket connected\r\n",
                        pNewSocket ));
            }
          }
        }

        //
        //  Release the socket layer synchronization
        //
        RESTORE_TPL ( TplPrevious );
      }
    }
  }

  //
  //  Return the new socket
  //
  if (( NULL != ppSocketProtocol )
    && ( NULL != pNewSocket )) {
    *ppSocketProtocol = &pNewSocket->SocketProtocol;
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    if ( NULL != pSocket ) {
      *pErrno = pSocket->errno;
    }
    else {
      Status = EFI_INVALID_PARAMETER;
      *pErrno = EBADF;
    }
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Allocate and initialize a ESL_SOCKET structure.
  
  This support function allocates an ::ESL_SOCKET structure
  and installs a protocol on ChildHandle.  If pChildHandle is a
  pointer to NULL, then a new handle is created and returned in
  pChildHandle.  If pChildHandle is not a pointer to NULL, then
  the protocol installs on the existing pChildHandle.

  @param [in, out] pChildHandle Pointer to the handle of the child to create.
                                If it is NULL, then a new handle is created.
                                If it is a pointer to an existing UEFI handle, 
                                then the protocol is added to the existing UEFI
                                handle.
  @param [in] DebugFlags        Flags for debug messages
  @param [in, out] ppSocket     The buffer to receive an ::ESL_SOCKET structure address.

  @retval EFI_SUCCESS           The protocol was added to ChildHandle.
  @retval EFI_INVALID_PARAMETER ChildHandle is NULL.
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources available to create
                                the child
  @retval other                 The child handle was not created
  
**/
EFI_STATUS
EFIAPI
EslSocketAllocate (
  IN OUT EFI_HANDLE * pChildHandle,
  IN     UINTN DebugFlags,
  IN OUT ESL_SOCKET ** ppSocket
  )
{
  UINTN LengthInBytes;
  ESL_LAYER * pLayer;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Create a socket structure
  //
  LengthInBytes = sizeof ( *pSocket );
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  LengthInBytes,
                  (VOID **) &pSocket
                  );
  if ( !EFI_ERROR ( Status )) {
    DEBUG (( DebugFlags | DEBUG_POOL | DEBUG_INIT,
              "0x%08x: Allocate pSocket, %d bytes\r\n",
              pSocket,
              LengthInBytes ));

    //
    //  Initialize the socket protocol
    //
    ZeroMem ( pSocket, LengthInBytes );

    pSocket->Signature = SOCKET_SIGNATURE;
    pSocket->SocketProtocol.pfnAccept = EslSocketAccept;
    pSocket->SocketProtocol.pfnBind = EslSocketBind;
    pSocket->SocketProtocol.pfnClosePoll = EslSocketClosePoll;
    pSocket->SocketProtocol.pfnCloseStart = EslSocketCloseStart;
    pSocket->SocketProtocol.pfnConnect = EslSocketConnect;
    pSocket->SocketProtocol.pfnGetLocal = EslSocketGetLocalAddress;
    pSocket->SocketProtocol.pfnGetPeer = EslSocketGetPeerAddress;
    pSocket->SocketProtocol.pfnListen = EslSocketListen;
    pSocket->SocketProtocol.pfnOptionGet = EslSocketOptionGet;
    pSocket->SocketProtocol.pfnOptionSet = EslSocketOptionSet;
    pSocket->SocketProtocol.pfnPoll = EslSocketPoll;
    pSocket->SocketProtocol.pfnReceive = EslSocketReceive;
    pSocket->SocketProtocol.pfnShutdown = EslSocketShutdown;
    pSocket->SocketProtocol.pfnSocket = EslSocket;
    pSocket->SocketProtocol.pfnTransmit = EslSocketTransmit;

    pSocket->MaxRxBuf = MAX_RX_DATA;
    pSocket->MaxTxBuf = MAX_TX_DATA;

    //
    //  Install the socket protocol on the specified handle
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                    pChildHandle,
                    &gEfiSocketProtocolGuid,
                    &pSocket->SocketProtocol,
                    NULL
                    );
    if ( !EFI_ERROR ( Status )) {
      DEBUG (( DebugFlags | DEBUG_POOL | DEBUG_INIT | DEBUG_INFO,
                "Installed: gEfiSocketProtocolGuid on   0x%08x\r\n",
                *pChildHandle ));
      pSocket->SocketProtocol.SocketHandle = *pChildHandle;

      //
      //  Synchronize with the socket layer
      //
      RAISE_TPL ( TplPrevious, TPL_SOCKETS );

      //
      //  Add this socket to the list
      //
      pLayer = &mEslLayer;
      pSocket->pNext = pLayer->pSocketList;
      pLayer->pSocketList = pSocket;

      //
      //  Release the socket layer synchronization
      //
      RESTORE_TPL ( TplPrevious );

      //
      //  Return the socket structure address
      //
      *ppSocket = pSocket;
    }
    else {
      DEBUG (( DEBUG_ERROR | DebugFlags | DEBUG_POOL | DEBUG_INIT,
                "ERROR - Failed to install gEfiSocketProtocolGuid on 0x%08x, Status: %r\r\n",
                *pChildHandle,
                Status ));
    }

    //
    //  Release the socket if necessary
    //
    if ( EFI_ERROR ( Status )) {
      gBS->FreePool ( pSocket );
      DEBUG (( DebugFlags | DEBUG_POOL | DEBUG_INIT,
                "0x%08x: Free pSocket, %d bytes\r\n",
                pSocket,
                sizeof ( *pSocket )));
      pSocket = NULL;
    }
  }
  else {
    DEBUG (( DEBUG_ERROR | DebugFlags | DEBUG_POOL | DEBUG_INIT,
              "ERROR - Failed socket allocation, Status: %r\r\n",
              Status ));
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Bind a name to a socket.

  This routine calls the network specific layer to save the network
  address of the local connection point.

  The ::bind routine calls this routine to connect a name
  (network address and port) to a socket on the local machine.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.

  @param [in] pSockAddr Address of a sockaddr structure that contains the
                        connection point on the local machine.  An IPv4 address
                        of INADDR_ANY specifies that the connection is made to
                        all of the network stacks on the platform.  Specifying a
                        specific IPv4 address restricts the connection to the
                        network stack supporting that address.  Specifying zero
                        for the port causes the network layer to assign a port
                        number from the dynamic range.  Specifying a specific
                        port number causes the network layer to use that port.

  @param [in] SockAddrLength  Specifies the length in bytes of the sockaddr structure.

  @param [out] pErrno   Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Socket successfully created

 **/
EFI_STATUS
EslSocketBind (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN const struct sockaddr * pSockAddr,
  IN socklen_t SockAddrLength,
  OUT int * pErrno
  )
{
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Validate the socket
  //
  pSocket = NULL;
  if ( NULL != pSocketProtocol ) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

    //
    //  Validate the structure pointer
    //
    if ( NULL == pSockAddr ) {
      DEBUG (( DEBUG_BIND,
                "ERROR - pSockAddr is NULL!\r\n" ));
      Status = EFI_INVALID_PARAMETER;
      pSocket->errno = EFAULT;
    }
    else{
      //
      //  Validate the name length
      //
      if (( SockAddrLength < ( sizeof ( struct sockaddr ) - sizeof ( pSockAddr->sa_data )))
        || ( pSockAddr->sa_len < ( sizeof ( struct sockaddr ) - sizeof ( pSockAddr->sa_data )))) {
        DEBUG (( DEBUG_BIND,
                  "ERROR - Invalid bind name length: %d, sa_len: %d\r\n",
                  SockAddrLength,
                  pSockAddr->sa_len ));
        Status = EFI_INVALID_PARAMETER;
        pSocket->errno = EINVAL;
      }
      else {
        //
        //  Set the socket address length
        //
        if ( SockAddrLength > pSockAddr->sa_len ) {
          SockAddrLength = pSockAddr->sa_len;
        }

        //
        //  Verify the API
        //
        if ( NULL == pSocket->pApi->pfnBind ) {
          Status = EFI_UNSUPPORTED;
          pSocket->errno = ENOTSUP;
        }
        else {
          //
          //  Synchronize with the socket layer
          //
          RAISE_TPL ( TplPrevious, TPL_SOCKETS );

          //
          //  Bind the socket
          //
          Status = pSocket->pApi->pfnBind ( pSocket,
                                            pSockAddr,
                                            SockAddrLength );

          //
          //  Mark this socket as bound if successful
          //
          if ( !EFI_ERROR ( Status )) {
            pSocket->State = SOCKET_STATE_BOUND;
          }

          //
          //  Release the socket layer synchronization
          //
          RESTORE_TPL ( TplPrevious );
        }
      }
    }
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    if ( NULL != pSocket ) {
      *pErrno = pSocket->errno;
    }
    else {
      Status = EFI_INVALID_PARAMETER;
      *pErrno = EBADF;
    }
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Determine if the socket is closed

  This routine checks the state of the socket to determine if
  the network specific layer has completed the close operation.

  The ::close routine polls this routine to determine when the
  close operation is complete.  The close operation needs to
  reverse the operations of the ::EslSocketAllocate routine.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.
  @param [out] pErrno         Address to receive the errno value upon completion.

  @retval EFI_SUCCESS     Socket successfully closed
  @retval EFI_NOT_READY   Close still in progress
  @retval EFI_ALREADY     Close operation already in progress
  @retval Other           Failed to close the socket

**/
EFI_STATUS
EslSocketClosePoll (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN int * pErrno
  )
{
  int errno;
  ESL_LAYER * pLayer;
  ESL_SOCKET * pNextSocket;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  errno = 0;
  Status = EFI_SUCCESS;

  //
  //  Synchronize with the socket layer
  //
  RAISE_TPL ( TplPrevious, TPL_SOCKETS );

  //
  //  Locate the socket
  //
  pLayer = &mEslLayer;
  pNextSocket = pLayer->pSocketList;
  pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );
  while ( NULL != pNextSocket ) {
    if ( pNextSocket == pSocket ) {
      //
      //  Determine if the socket is in the closing state
      //
      if ( SOCKET_STATE_CLOSED == pSocket->State ) {
        //
        //  Walk the list of ports
        //
        if ( NULL == pSocket->pPortList ) {
          //
          //  All the ports are closed
          //  Close the WaitAccept event if necessary
          //
          if ( NULL != pSocket->WaitAccept ) {
            Status = gBS->CloseEvent ( pSocket->WaitAccept );
            if ( !EFI_ERROR ( Status )) {
              DEBUG (( DEBUG_SOCKET | DEBUG_CLOSE | DEBUG_POOL,
                        "0x%08x: Closed WaitAccept event\r\n",
                        pSocket->WaitAccept ));
              //
              //  Return the transmit status
              //
              Status = pSocket->TxError;
              if ( EFI_ERROR ( Status )) {
                pSocket->errno = EIO;
              }
            }
            else {
              DEBUG (( DEBUG_ERROR | DEBUG_SOCKET | DEBUG_CLOSE | DEBUG_POOL,
                        "ERROR - Failed to close the WaitAccept event, Status: %r\r\n",
                        Status ));
              ASSERT ( EFI_SUCCESS == Status );
            }
          }
        }
        else {
          //
          //  At least one port is still open
          //
          Status = EFI_NOT_READY;
          errno = EAGAIN;
        }
      }
      else {
        //
        //  SocketCloseStart was not called
        //
        Status = EFI_NOT_STARTED;
        errno = EPERM;
      }
      break;
    }

    //
    //  Set the next socket
    //
    pNextSocket = pNextSocket->pNext;
  }

  //
  //  Handle the error case where the socket was already closed
  //
  if ( NULL == pSocket ) {
    //
    //  Socket not found
    //
    Status = EFI_NOT_FOUND;
    errno = ENOTSOCK;
  }

  //
  //  Release the socket layer synchronization
  //
  RESTORE_TPL ( TplPrevious );

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    *pErrno = errno;
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Start the close operation on the socket

  This routine calls the network specific layer to initiate the
  close state machine.  This routine then calls the network
  specific layer to determine if the close state machine has gone
  to completion.  The result from this poll is returned to the
  caller.

  The ::close routine calls this routine to start the close
  operation which reverses the operations of the
  ::EslSocketAllocate routine.  The close routine then polls
  the ::EslSocketClosePoll routine to determine when the
  socket is closed.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.
  @param [in] bCloseNow       Boolean to control close behavior
  @param [out] pErrno         Address to receive the errno value upon completion.

  @retval EFI_SUCCESS     Socket successfully closed
  @retval EFI_NOT_READY   Close still in progress
  @retval EFI_ALREADY     Close operation already in progress
  @retval Other           Failed to close the socket

**/
EFI_STATUS
EslSocketCloseStart (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN BOOLEAN bCloseNow,
  IN int * pErrno
  )
{
  int errno;
  ESL_PORT * pNextPort;
  ESL_PORT * pPort;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;
  errno = 0;

  //
  //  Synchronize with the socket layer
  //
  RAISE_TPL ( TplPrevious, TPL_SOCKETS );

  //
  //  Determine if the socket is already closed
  //
  pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );
  if ( SOCKET_STATE_CLOSED > pSocket->State ) {
    //
    //  Update the socket state
    //
    pSocket->State = SOCKET_STATE_CLOSED;

    //
    //  Walk the list of ports
    //
    pPort = pSocket->pPortList;
    while ( NULL != pPort ) {
      //
      //  Start closing the ports
      //
      pNextPort = pPort->pLinkSocket;
      Status = pPort->pfnCloseStart ( pPort,
                                      bCloseNow,
                                      DEBUG_CLOSE | DEBUG_LISTEN | DEBUG_CONNECTION );
      if (( EFI_SUCCESS != Status )
        && ( EFI_NOT_READY != Status )) {
        errno = EIO;
        break;
      }

      //
      //  Set the next port
      //
      pPort = pNextPort;
    }

    //
    //  Attempt to finish closing the socket
    //
    if ( NULL == pPort ) {
      Status = EslSocketClosePoll ( pSocketProtocol, &errno );
    }
  }
  else {
    Status = EFI_ALREADY_STARTED;
    errno = EALREADY;
  }

  //
  //  Release the socket layer synchronization
  //
  RESTORE_TPL ( TplPrevious );

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    *pErrno = errno;
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Connect to a remote system via the network.

  This routine calls the network specific layer to establish
  the remote system address and establish the connection to
  the remote system.

  The ::connect routine calls this routine to establish a
  connection with the specified remote system.  This routine
  is designed to be polled by the connect routine for completion
  of the network connection.
  
  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.

  @param [in] pSockAddr       Network address of the remote system.
    
  @param [in] SockAddrLength  Length in bytes of the network address.
  
  @param [out] pErrno   Address to receive the errno value upon completion.
  
  @retval EFI_SUCCESS   The connection was successfully established.
  @retval EFI_NOT_READY The connection is in progress, call this routine again.
  @retval Others        The connection attempt failed.

 **/
EFI_STATUS
EslSocketConnect (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN const struct sockaddr * pSockAddr,
  IN socklen_t SockAddrLength,
  IN int * pErrno
  )
{
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;
  
  DEBUG (( DEBUG_CONNECT, "Entering SocketConnect\r\n" ));

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Validate the socket
  //
  pSocket = NULL;
  if ( NULL != pSocketProtocol ) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

    //
    //  Validate the name length
    //
    if (( SockAddrLength < ( sizeof ( struct sockaddr ) - sizeof ( pSockAddr->sa_data )))
      || ( pSockAddr->sa_len < ( sizeof ( struct sockaddr ) - sizeof ( pSockAddr->sa_data )))) {
      DEBUG (( DEBUG_CONNECT,
                "ERROR - Invalid bind name length: %d, sa_len: %d\r\n",
                SockAddrLength,
                pSockAddr->sa_len ));
      Status = EFI_INVALID_PARAMETER;
      pSocket->errno = EINVAL;
    }
    else {
      //
      //  Assume success
      //
      pSocket->errno = 0;

      //
      //  Set the socket address length
      //
      if ( SockAddrLength > pSockAddr->sa_len ) {
        SockAddrLength = pSockAddr->sa_len;
      }

      //
      //  Synchronize with the socket layer
      //
      RAISE_TPL ( TplPrevious, TPL_SOCKETS );

      //
      //  Validate the socket state
      //
      switch ( pSocket->State ) {
      default:
        //
        //  Wrong socket state
        //
        pSocket->errno = EIO;
        Status = EFI_DEVICE_ERROR;
        break;

      case SOCKET_STATE_NOT_CONFIGURED:
      case SOCKET_STATE_BOUND:
        //
        //  Verify the API
        //
        if ( NULL == pSocket->pApi->pfnConnectStart ) {
          Status = EFI_UNSUPPORTED;
          pSocket->errno = ENOTSUP;
        }
        else {
          //
          //  Initiate the connection with the remote system
          //
          Status = pSocket->pApi->pfnConnectStart ( pSocket,
                                                    pSockAddr,
                                                    SockAddrLength );

          //
          //  Set the next state if connecting
          //
          if ( EFI_NOT_READY == Status ) {
            pSocket->State = SOCKET_STATE_CONNECTING;
          }
        }
        break;

      case SOCKET_STATE_CONNECTING:
        //
        //  Poll for connection completion
        //
        if ( NULL == pSocket->pApi->pfnConnectPoll ) {
          //
          //  Already connected
          //
          pSocket->errno = EISCONN;
          Status = EFI_ALREADY_STARTED;
        }
        else {
          Status = pSocket->pApi->pfnConnectPoll ( pSocket );

          //
          //  Set the next state if connected
          //
          if ( EFI_NOT_READY != Status ) {
            if ( !EFI_ERROR ( Status )) {
              pSocket->State = SOCKET_STATE_CONNECTED;
            }
            else {
              pSocket->State = SOCKET_STATE_BOUND;
            }
          }
        }
        break;

      case SOCKET_STATE_CONNECTED:
        //
        //  Already connected
        //
        pSocket->errno = EISCONN;
        Status = EFI_ALREADY_STARTED;
        break;
      }

      //
      //  Release the socket layer synchronization
      //
      RESTORE_TPL ( TplPrevious );
    }
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    if ( NULL != pSocket ) {
      *pErrno = pSocket->errno;
    }
    else {
      //
      //  Bad socket protocol
      //
      DEBUG (( DEBUG_ERROR | DEBUG_CONNECT,
                "ERROR - pSocketProtocol invalid!\r\n" ));
      Status = EFI_INVALID_PARAMETER;
      *pErrno = EBADF;
    }
  }

  //
  //  Return the operation status
  //
  DEBUG (( DEBUG_CONNECT, "Exiting SocketConnect, Status: %r\r\n", Status ));
  return Status;
}


/**
  Get the local address.

  This routine calls the network specific layer to get the network
  address of the local host connection point.

  The ::getsockname routine calls this routine to obtain the network
  address associated with the local host connection point.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.
  
  @param [out] pAddress       Network address to receive the local system address

  @param [in,out] pAddressLength  Length of the local network address structure

  @param [out] pErrno         Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Local address successfully returned

 **/
EFI_STATUS
EslSocketGetLocalAddress (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength,
  IN int * pErrno
  )
{
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;
  
  DBG_ENTER ( );
  
  //
  //  Assume success
  //
  Status = EFI_SUCCESS;
  
  //
  //  Validate the socket
  //
  pSocket = NULL;
  if ( NULL != pSocketProtocol ) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

    //
    //  Verify the socket state
    //
    Status = EslSocketIsConfigured ( pSocket );
    if ( !EFI_ERROR ( Status )) {
      //
      //  Verify the address buffer and length address
      //
      if (( NULL != pAddress ) && ( NULL != pAddressLength )) {
        //
        //  Verify the socket state
        //
        if ( SOCKET_STATE_CONNECTED == pSocket->State ) {
          //
          //  Verify the API
          //
          if ( NULL == pSocket->pApi->pfnGetLocalAddr ) {
            Status = EFI_UNSUPPORTED;
            pSocket->errno = ENOTSUP;
          }
          else {
            //
            //  Synchronize with the socket layer
            //
            RAISE_TPL ( TplPrevious, TPL_SOCKETS );

            //
            //  Get the local address
            //
            Status = pSocket->pApi->pfnGetLocalAddr ( pSocket,
                                                      pAddress,
                                                      pAddressLength );

            //
            //  Release the socket layer synchronization
            //
            RESTORE_TPL ( TplPrevious );
          }
        }
        else {
          pSocket->errno = ENOTCONN;
          Status = EFI_NOT_STARTED;
        }
      }
      else {
        pSocket->errno = EINVAL;
        Status = EFI_INVALID_PARAMETER;
      }
    }
  }
  
  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    if ( NULL != pSocket ) {
      *pErrno = pSocket->errno;
    }
    else {
      Status = EFI_INVALID_PARAMETER;
      *pErrno = EBADF;
    }
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Get the peer address.

  This routine calls the network specific layer to get the remote
  system connection point.

  The ::getpeername routine calls this routine to obtain the network
  address of the remote connection point.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.
  
  @param [out] pAddress       Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @param [out] pErrno         Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Remote address successfully returned

 **/
EFI_STATUS
EslSocketGetPeerAddress (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength,
  IN int * pErrno
  )
{
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;
  
  DBG_ENTER ( );
  
  //
  //  Assume success
  //
  Status = EFI_SUCCESS;
  
  //
  //  Validate the socket
  //
  pSocket = NULL;
  if ( NULL != pSocketProtocol ) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

    //
    //  Verify the socket state
    //
    Status = EslSocketIsConfigured ( pSocket );
    if ( !EFI_ERROR ( Status )) {
      //
      //  Verify the API
      //
      if ( NULL == pSocket->pApi->pfnGetRemoteAddr ) {
        Status = EFI_UNSUPPORTED;
        pSocket->errno = ENOTSUP;
      }
      else {
        //
        //  Verify the address buffer and length address
        //
        if (( NULL != pAddress ) && ( NULL != pAddressLength )) {
          //
          //  Verify the socket state
          //
          if ( SOCKET_STATE_CONNECTED == pSocket->State ) {
            //
            //  Synchronize with the socket layer
            //
            RAISE_TPL ( TplPrevious, TPL_SOCKETS );

            //
            //  Get the remote address
            //
            Status = pSocket->pApi->pfnGetRemoteAddr ( pSocket,
                                                       pAddress,
                                                       pAddressLength );

            //
            //  Release the socket layer synchronization
            //
            RESTORE_TPL ( TplPrevious );
          }
          else {
            pSocket->errno = ENOTCONN;
            Status = EFI_NOT_STARTED;
          }
        }
        else {
          pSocket->errno = EINVAL;
          Status = EFI_INVALID_PARAMETER;
        }
      }
    }
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    if ( NULL != pSocket ) {
      *pErrno = pSocket->errno;
    }
    else {
      Status = EFI_INVALID_PARAMETER;
      *pErrno = EBADF;
    }
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Free the ESL_IO_MGMT event and structure

  This support routine walks the free list to close the event in
  the ESL_IO_MGMT structure and remove the structure from the free
  list.

  See the \ref TransmitEngine section.

  @param [in] pPort         Address of an ::ESL_PORT structure
  @param [in] ppFreeQueue   Address of the free queue head
  @param [in] DebugFlags    Flags for debug messages
  @param [in] pEventName    Zero terminated string containing the event name
  @param [in] EventOffset   Offset of the event in the ::ESL_IO_MGMT structure

  @retval EFI_SUCCESS - The structures were properly initialized

**/
EFI_STATUS
EslSocketIoFree (
  IN ESL_PORT * pPort,
  IN ESL_IO_MGMT ** ppFreeQueue,
  IN UINTN DebugFlags,
  IN CHAR8 * pEventName,
  IN UINT32 EventOffset
  )
{
  UINT8 * pBuffer;
  EFI_EVENT * pEvent;
  ESL_IO_MGMT * pIo;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Walk the list of IO structures
  //
  pSocket = pPort->pSocket;
  while ( *ppFreeQueue ) {
    //
    //  Free the event for this structure
    //
    pIo = *ppFreeQueue;
    pBuffer = (UINT8 *)pIo;
    pBuffer = &pBuffer[ EventOffset ];
    pEvent = (EFI_EVENT *)pBuffer;
    Status = gBS->CloseEvent ( *pEvent );
    if ( EFI_ERROR ( Status )) {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to close the %a event, Status: %r\r\n",
                pEventName,
                Status ));
      pSocket->errno = ENOMEM;
      break;
    }
    DEBUG (( DebugFlags,
              "0x%08x: Closed %a event 0x%08x\r\n",
              pIo,
              pEventName,
              *pEvent ));

    //
    //  Remove this structure from the queue
    //
    *ppFreeQueue = pIo->pNext;
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Initialize the ESL_IO_MGMT structures

  This support routine initializes the ESL_IO_MGMT structure and
  places them on to a free list.

  This routine is called by the PortAllocate routines to prepare
  the transmit engines.  See the \ref TransmitEngine section.

  @param [in] pPort         Address of an ::ESL_PORT structure
  @param [in, out] ppIo     Address containing the first structure address.  Upon
                            return this buffer contains the next structure address.
  @param [in] TokenCount    Number of structures to initialize
  @param [in] ppFreeQueue   Address of the free queue head
  @param [in] DebugFlags    Flags for debug messages
  @param [in] pEventName    Zero terminated string containing the event name
  @param [in] EventOffset   Offset of the event in the ::ESL_IO_MGMT structure
  @param [in] pfnCompletion Completion routine address

  @retval EFI_SUCCESS - The structures were properly initialized

**/
EFI_STATUS
EslSocketIoInit (
  IN ESL_PORT * pPort,
  IN ESL_IO_MGMT ** ppIo,
  IN UINTN TokenCount,
  IN ESL_IO_MGMT ** ppFreeQueue,
  IN UINTN DebugFlags,
  IN CHAR8 * pEventName,
  IN UINTN EventOffset,
  IN EFI_EVENT_NOTIFY pfnCompletion
  )
{
  ESL_IO_MGMT * pEnd;
  EFI_EVENT * pEvent;
  ESL_IO_MGMT * pIo;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Walk the list of IO structures
  //
  pSocket = pPort->pSocket;
  pIo = *ppIo;
  pEnd = &pIo [ TokenCount ];
  while ( pEnd > pIo ) {
    //
    //  Initialize the IO structure
    //
    pIo->pPort = pPort;
    pIo->pPacket = NULL;

    //
    //  Allocate the event for this structure
    //
    pEvent = (EFI_EVENT *)&(((UINT8 *)pIo)[ EventOffset ]);
    Status = gBS->CreateEvent ( EVT_NOTIFY_SIGNAL,
                                TPL_SOCKETS,
                                (EFI_EVENT_NOTIFY)pfnCompletion,
                                pIo,
                                pEvent );
    if ( EFI_ERROR ( Status )) {
      DEBUG (( DEBUG_ERROR | DebugFlags,
                "ERROR - Failed to create the %a event, Status: %r\r\n",
                pEventName,
                Status ));
      pSocket->errno = ENOMEM;
      break;
    }
    DEBUG (( DebugFlags,
              "0x%08x: Created %a event 0x%08x\r\n",
              pIo,
              pEventName,
              *pEvent ));

    //
    //  Add this structure to the queue
    //
    pIo->pNext = *ppFreeQueue;
    *ppFreeQueue = pIo;

    //
    //  Set the next structure
    //
    pIo += 1;
  }

  //
  //  Save the next structure
  //
  *ppIo = pIo;

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Determine if the socket is configured

  This support routine is called to determine if the socket if the
  configuration call was made to the network layer.  The following
  routines call this routine to verify that they may be successful
  in their operations:
  <ul>
    <li>::EslSocketGetLocalAddress</li>
    <li>::EslSocketGetPeerAddress</li>
    <li>::EslSocketPoll</li>
    <li>::EslSocketReceive</li>
    <li>::EslSocketTransmit</li>
  </ul>

  @param [in] pSocket       Address of an ::ESL_SOCKET structure

  @retval EFI_SUCCESS - The socket is configured

**/
EFI_STATUS
EslSocketIsConfigured (
  IN ESL_SOCKET * pSocket
  )
{
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Verify the socket state
  //
  if ( !pSocket->bConfigured ) {
    //
    //  Verify the API
    //
    if ( NULL == pSocket->pApi->pfnIsConfigured ) {
      Status = EFI_UNSUPPORTED;
      pSocket->errno = ENOTSUP;
    }
    else {
      //
      //  Synchronize with the socket layer
      //
      RAISE_TPL ( TplPrevious, TPL_SOCKETS );

      //
      //  Determine if the socket is configured
      //
      Status = pSocket->pApi->pfnIsConfigured ( pSocket );

      //
      //  Release the socket layer synchronization
      //
      RESTORE_TPL ( TplPrevious );

      //
      //  Set errno if a failure occurs
      //
      if ( EFI_ERROR ( Status )) {
        pSocket->errno = EADDRNOTAVAIL;
      }
    }
  }

  //
  //  Return the configuration status
  //
  return Status;
}


/**
  Establish the known port to listen for network connections.

  This routine calls into the network protocol layer to establish
  a handler that is called upon connection completion.  The handler
  is responsible for inserting the connection into the FIFO.

  The ::listen routine indirectly calls this routine to place the
  socket into a state that enables connection attempts.  Connections
  are placed in a FIFO that is serviced by the application.  The
  application calls the ::accept (::EslSocketAccept) routine to
  remove the next connection from the FIFO and get the associated
  socket and address.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.

  @param [in] Backlog         Backlog specifies the maximum FIFO depth for
                              the connections waiting for the application
                              to call accept.  Connection attempts received
                              while the queue is full are refused.

  @param [out] pErrno         Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Socket successfully created
  @retval Other - Failed to enable the socket for listen

**/
EFI_STATUS
EslSocketListen (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN INT32 Backlog,
  OUT int * pErrno
  )
{
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_STATUS TempStatus;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Validate the socket
  //
  pSocket = NULL;
  if ( NULL != pSocketProtocol ) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

    //
    //  Verify the API
    //
    if ( NULL == pSocket->pApi->pfnListen ) {
      Status = EFI_UNSUPPORTED;
      pSocket->errno = ENOTSUP;
    }
    else {
      //
      //  Assume success
      //
      pSocket->Status = EFI_SUCCESS;
      pSocket->errno = 0;

      //
      //  Verify that the bind operation was successful
      //
      if ( SOCKET_STATE_BOUND == pSocket->State ) {
        //
        //  Synchronize with the socket layer
        //
        RAISE_TPL ( TplPrevious, TPL_SOCKETS );

        //
        //  Create the event for SocketAccept completion
        //
        Status = gBS->CreateEvent ( 0,
                                    TplPrevious,
                                    NULL,
                                    NULL,
                                    &pSocket->WaitAccept );
        if ( !EFI_ERROR ( Status )) {
          DEBUG (( DEBUG_POOL,
                    "0x%08x: Created WaitAccept event\r\n",
                    pSocket->WaitAccept ));
          //
          //  Set the maximum FIFO depth
          //
          if ( 0 >= Backlog ) {
            Backlog = MAX_PENDING_CONNECTIONS;
          }
          else {
            if ( SOMAXCONN < Backlog ) {
              Backlog = SOMAXCONN;
            }
            else {
              pSocket->MaxFifoDepth = Backlog;
            }
          }

          //
          //  Initiate the connection attempt listen
          //
          Status = pSocket->pApi->pfnListen ( pSocket );

          //
          //  Place the socket in the listen state if successful
          //
          if ( !EFI_ERROR ( Status )) {
            pSocket->State = SOCKET_STATE_LISTENING;
          }
          else {
            //
            //  Not waiting for SocketAccept to complete
            //
            TempStatus = gBS->CloseEvent ( pSocket->WaitAccept );
            if ( !EFI_ERROR ( TempStatus )) {
              DEBUG (( DEBUG_POOL,
                        "0x%08x: Closed WaitAccept event\r\n",
                        pSocket->WaitAccept ));
              pSocket->WaitAccept = NULL;
            }
            else {
              DEBUG (( DEBUG_ERROR | DEBUG_POOL,
                        "ERROR - Failed to close WaitAccept event, Status: %r\r\n",
                        TempStatus ));
              ASSERT ( EFI_SUCCESS == TempStatus );
            }
          }
        }
        else {
          DEBUG (( DEBUG_ERROR | DEBUG_LISTEN,
                    "ERROR - Failed to create the WaitAccept event, Status: %r\r\n",
                    Status ));
          pSocket->errno = ENOMEM;
        }

        //
        //  Release the socket layer synchronization
        //
        RESTORE_TPL ( TplPrevious );
      }
      else {
        DEBUG (( DEBUG_ERROR | DEBUG_LISTEN,
                  "ERROR - Bind operation must be performed first!\r\n" ));
        pSocket->errno = EDESTADDRREQ;
      }
    }
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    if ( NULL != pSocket ) {
      *pErrno = pSocket->errno;
    }
    else {
      Status = EFI_INVALID_PARAMETER;
      *pErrno = EBADF;
    }
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Get the socket options

  This routine handles the socket level options and passes the
  others to the network specific layer.

  The ::getsockopt routine calls this routine to retrieve the
  socket options one at a time by name.

  @param [in] pSocketProtocol   Address of an ::EFI_SOCKET_PROTOCOL structure.
  @param [in] level             Option protocol level
  @param [in] OptionName        Name of the option
  @param [out] pOptionValue     Buffer to receive the option value
  @param [in,out] pOptionLength Length of the buffer in bytes,
                                upon return length of the option value in bytes
  @param [out] pErrno           Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Socket data successfully received

 **/
EFI_STATUS
EslSocketOptionGet (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN int level,
  IN int OptionName,
  OUT void * __restrict pOptionValue,
  IN OUT socklen_t * __restrict pOptionLength,
  IN int * pErrno
  )
{
  int errno;
  socklen_t LengthInBytes;
  socklen_t MaxBytes;
  UINT8 * pOptionData;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume failure
  //
  errno = EINVAL;
  Status = EFI_INVALID_PARAMETER;

  //
  //  Validate the socket
  //
  pSocket = NULL;
  if (( NULL != pSocketProtocol )
    && ( NULL != pOptionValue )
    && ( NULL != pOptionLength )) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );
    LengthInBytes = 0;
    MaxBytes = *pOptionLength;
    pOptionData = NULL;
    switch ( level ) {
    default:
      //
      //  See if the protocol will handle the option
      //
      if ( NULL != pSocket->pApi->pfnOptionGet ) {
        Status = pSocket->pApi->pfnOptionGet ( pSocket,
                                               level,
                                               OptionName,
                                               &pOptionData,
                                               &LengthInBytes );
        errno = pSocket->errno;
      }
      else {
        //
        //  Protocol level not supported
        //
        DEBUG (( DEBUG_INFO | DEBUG_OPTION, "ERROR - Invalid option level\r\n" ));
        errno = ENOTSUP;
        Status = EFI_UNSUPPORTED;
      }
      break;

    case SOL_SOCKET:
      switch ( OptionName ) {
      default:
        //
        //  See if the protocol will handle the socket option
        //
        if ( NULL != pSocket->pApi->pfnOptionGet ) {
          Status = pSocket->pApi->pfnOptionGet ( pSocket,
                                                 level,
                                                 OptionName,
                                                 &pOptionData,
                                                 &LengthInBytes );
          errno = pSocket->errno;
        }
        else {
          //
          //  Socket option not supported
          //
          DEBUG (( DEBUG_INFO | DEBUG_OPTION, "ERROR - Invalid socket option\r\n" ));
          errno = ENOTSUP;
          Status = EFI_UNSUPPORTED;
        }
        break;

      case SO_OOBINLINE:
        pOptionData = (UINT8 *)&pSocket->bOobInLine;
        LengthInBytes = sizeof ( pSocket->bOobInLine );
        break;

      case SO_RCVTIMEO:
        //
        //  Return the receive timeout
        //
        pOptionData = (UINT8 *)&pSocket->RxTimeout;
        LengthInBytes = sizeof ( pSocket->RxTimeout );
        break;
        
      case SO_RCVBUF:
        //
        //  Return the maximum transmit buffer size
        //
        pOptionData = (UINT8 *)&pSocket->MaxRxBuf;
        LengthInBytes = sizeof ( pSocket->MaxRxBuf );
        break;

      case SO_SNDBUF:
        //
        //  Return the maximum transmit buffer size
        //
        pOptionData = (UINT8 *)&pSocket->MaxTxBuf;
        LengthInBytes = sizeof ( pSocket->MaxTxBuf );
        break;

      case SO_TYPE:
        //
        //  Return the socket type
        //
        pOptionData = (UINT8 *)&pSocket->Type;
        LengthInBytes = sizeof ( pSocket->Type );
        break;
      }
      break;
    }

    //
    //  Return the option length
    //
    *pOptionLength = LengthInBytes;

    //
    //  Return the option value
    //
    if ( NULL != pOptionData ) {
      //
      //  Silently truncate the value length
      //
      if ( LengthInBytes > MaxBytes ) {
        LengthInBytes = MaxBytes;
      }

      //
      //  Return the value
      //
      CopyMem ( pOptionValue, pOptionData, LengthInBytes );

      //
      //  Zero fill any remaining space
      //
      if ( LengthInBytes < MaxBytes ) {
        ZeroMem ( &((UINT8 *)pOptionValue)[LengthInBytes], MaxBytes - LengthInBytes );
      }
      errno = 0;
      Status = EFI_SUCCESS;
    }
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    *pErrno = errno;
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Set the socket options

  This routine handles the socket level options and passes the
  others to the network specific layer.

  The ::setsockopt routine calls this routine to adjust the socket
  options one at a time by name.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.
  @param [in] level           Option protocol level
  @param [in] OptionName      Name of the option
  @param [in] pOptionValue    Buffer containing the option value
  @param [in] OptionLength    Length of the buffer in bytes
  @param [out] pErrno         Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Option successfully set

 **/
EFI_STATUS
EslSocketOptionSet (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN int level,
  IN int OptionName,
  IN CONST void * pOptionValue,
  IN socklen_t OptionLength,
  IN int * pErrno
  )
{
  BOOLEAN bTrueFalse;
  int errno;
  socklen_t LengthInBytes;
  UINT8 * pOptionData;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  
  DBG_ENTER ( );
  
  //
  //  Assume failure
  //
  errno = EINVAL;
  Status = EFI_INVALID_PARAMETER;

  //
  //  Validate the socket
  //
  pSocket = NULL;
  if (( NULL != pSocketProtocol )
    && ( NULL != pOptionValue )) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );
    LengthInBytes = 0;
    pOptionData = NULL;
    switch ( level ) {
    default:
      //
      //  See if the protocol will handle the option
      //
      if ( NULL != pSocket->pApi->pfnOptionSet ) {
        Status = pSocket->pApi->pfnOptionSet ( pSocket,
                                               level,
                                               OptionName,
                                               pOptionValue,
                                               OptionLength );
        errno = pSocket->errno;
      }
      else {
        //
        //  Protocol level not supported
        //
        errno = ENOTSUP;
        Status = EFI_UNSUPPORTED;
      }
      break;
  
    case SOL_SOCKET:
      switch ( OptionName ) {
      default:
        //
        //  See if the protocol will handle the socket option
        //
        if ( NULL != pSocket->pApi->pfnOptionSet ) {
          Status = pSocket->pApi->pfnOptionSet ( pSocket,
                                                 level,
                                                 OptionName,
                                                 pOptionValue,
                                                 OptionLength );
          errno = pSocket->errno;
        }
        else {
          //
          //  Option not supported
          //
          errno = ENOTSUP;
          Status = EFI_UNSUPPORTED;
        }
        break;

      case SO_OOBINLINE:
        pOptionData = (UINT8 *)&pSocket->bOobInLine;
        LengthInBytes = sizeof ( pSocket->bOobInLine );

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
        }
        else {
          //
          //  Force an invalid option length error
          //
          OptionLength = LengthInBytes - 1;
        }
        break;

      case SO_RCVTIMEO:
        //
        //  Return the receive timeout
        //
        pOptionData = (UINT8 *)&pSocket->RxTimeout;
        LengthInBytes = sizeof ( pSocket->RxTimeout );
        break;

      case SO_RCVBUF:
        //
        //  Return the maximum transmit buffer size
        //
        pOptionData = (UINT8 *)&pSocket->MaxRxBuf;
        LengthInBytes = sizeof ( pSocket->MaxRxBuf );
        break;

      case SO_SNDBUF:
        //
        //  Send buffer size
        //
        //
        //  Return the maximum transmit buffer size
        //
        pOptionData = (UINT8 *)&pSocket->MaxTxBuf;
        LengthInBytes = sizeof ( pSocket->MaxTxBuf );
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
        errno = 0;
        Status = EFI_SUCCESS;
      }
    }
  }
  
  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    *pErrno = errno;
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Allocate a packet for a receive or transmit operation

  This support routine is called by the network specific RxStart
  and TxBuffer routines to get buffer space for the next operation.

  @param [in] ppPacket      Address to receive the ::ESL_PACKET structure
  @param [in] LengthInBytes Length of the packet structure
  @param [in] DebugFlags    Flags for debug messages

  @retval EFI_SUCCESS - The packet was allocated successfully

 **/
EFI_STATUS
EslSocketPacketAllocate (
  IN ESL_PACKET ** ppPacket,
  IN size_t LengthInBytes,
  IN UINTN DebugFlags
  )
{
  ESL_PACKET * pPacket;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Allocate a packet structure
  //
  LengthInBytes += sizeof ( *pPacket )
                - sizeof ( pPacket->Op );
  Status = gBS->AllocatePool ( EfiRuntimeServicesData,
                               LengthInBytes,
                               (VOID **)&pPacket );
  if ( !EFI_ERROR ( Status )) {
    DEBUG (( DebugFlags | DEBUG_POOL | DEBUG_INIT,
              "0x%08x: Allocate pPacket, %d bytes\r\n",
              pPacket,
              LengthInBytes ));
    pPacket->PacketSize = LengthInBytes;
  }
  else {
    DEBUG (( DebugFlags | DEBUG_POOL | DEBUG_INFO,
              "ERROR - Packet allocation failed for %d bytes, Status: %r\r\n",
              LengthInBytes,
              Status ));
    pPacket = NULL;
  }

  //
  //  Return the packet
  //
  *ppPacket = pPacket;

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Free a packet used for receive or transmit operation

  This support routine is called by the network specific Close
  and TxComplete routines and during error cases in RxComplete
  and TxBuffer.  Note that the network layers typically place
  receive packets on the ESL_SOCKET::pRxFree list for reuse.

  @param [in] pPacket     Address of an ::ESL_PACKET structure
  @param [in] DebugFlags  Flags for debug messages

  @retval EFI_SUCCESS - The packet was allocated successfully

 **/
EFI_STATUS
EslSocketPacketFree (
  IN ESL_PACKET * pPacket,
  IN UINTN DebugFlags
  )
{
  UINTN LengthInBytes;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Allocate a packet structure
  //
  LengthInBytes = pPacket->PacketSize;
  Status = gBS->FreePool ( pPacket );
  if ( !EFI_ERROR ( Status )) {
    DEBUG (( DebugFlags | DEBUG_POOL,
              "0x%08x: Free pPacket, %d bytes\r\n",
              pPacket,
              LengthInBytes ));
  }
  else {
    DEBUG (( DebugFlags | DEBUG_POOL | DEBUG_INFO,
              "ERROR - Failed to free packet 0x%08x, Status: %r\r\n",
              pPacket,
              Status ));
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Poll a socket for pending activity.

  This routine builds a detected event mask which is returned to
  the caller in the buffer provided.

  The ::poll routine calls this routine to determine if the socket
  needs to be serviced as a result of connection, error, receive or
  transmit activity.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.

  @param [in] Events    Events of interest for this socket

  @param [in] pEvents   Address to receive the detected events

  @param [out] pErrno   Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Socket successfully polled
  @retval EFI_INVALID_PARAMETER - When pEvents is NULL

 **/
EFI_STATUS
EslSocketPoll (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN short Events,
  IN short * pEvents,
  IN int * pErrno
  )
{
  short DetectedEvents;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  short ValidEvents;

  DEBUG (( DEBUG_POLL, "Entering SocketPoll\r\n" ));

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;
  DetectedEvents = 0;
  pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );
  pSocket->errno = 0;

  //
  //  Verify the socket state
  //
  Status = EslSocketIsConfigured ( pSocket );
  if ( !EFI_ERROR ( Status )) {
    //
    //  Check for invalid events
    //
    ValidEvents = POLLIN
                | POLLPRI
                | POLLOUT | POLLWRNORM
                | POLLERR
                | POLLHUP
                | POLLNVAL
                | POLLRDNORM
                | POLLRDBAND
                | POLLWRBAND ;
    if ( 0 != ( Events & ( ~ValidEvents ))) {
      DetectedEvents |= POLLNVAL;
      DEBUG (( DEBUG_INFO | DEBUG_POLL,
                "ERROR - Invalid event mask, Valid Events: 0x%04x, Invalid Events: 0x%04x\r\n",
                Events & ValidEvents,
                Events & ( ~ValidEvents )));
    }
    else {
      //
      //  Check for pending connections
      //
      if ( 0 != pSocket->FifoDepth ) {
        //
        //  A connection is waiting for an accept call
        //  See posix connect documentation at
        //  http://pubs.opengroup.org/onlinepubs/9699919799/functions/accept.htm
        //
        DetectedEvents |= POLLIN | POLLRDNORM;
      }
      if ( pSocket->bConnected ) {
        //
        //  A connection is present
        //  See posix connect documentation at
        //  http://pubs.opengroup.org/onlinepubs/9699919799/functions/listen.htm
        //
        DetectedEvents |= POLLOUT | POLLWRNORM;
      }

      //
      //  The following bits are set based upon the POSIX poll documentation at
      //  http://pubs.opengroup.org/onlinepubs/9699919799/functions/poll.html
      //

      //
      //  Check for urgent receive data
      //
      if ( 0 < pSocket->RxOobBytes ) {
        DetectedEvents |= POLLRDBAND | POLLPRI | POLLIN;
      }

      //
      //  Check for normal receive data
      //
      if (( 0 < pSocket->RxBytes )
        || ( EFI_SUCCESS != pSocket->RxError )) {
        DetectedEvents |= POLLRDNORM | POLLIN;
      }

      //
      //  Handle the receive errors
      //
      if (( EFI_SUCCESS != pSocket->RxError )
        && ( 0 == ( DetectedEvents & POLLIN ))) {
        DetectedEvents |= POLLERR | POLLIN | POLLRDNORM | POLLRDBAND;
      }

      //
      //  Check for urgent transmit data buffer space
      //
      if (( MAX_TX_DATA > pSocket->TxOobBytes )
        || ( EFI_SUCCESS != pSocket->TxError )) {
        DetectedEvents |= POLLWRBAND;
      }

      //
      //  Check for normal transmit data buffer space
      //
      if (( MAX_TX_DATA > pSocket->TxBytes )
        || ( EFI_SUCCESS != pSocket->TxError )) {
        DetectedEvents |= POLLWRNORM;
      }

      //
      //  Handle the transmit error
      //
      if ( EFI_ERROR ( pSocket->TxError )) {
        DetectedEvents |= POLLERR;
      }
    }
  }

  //
  //  Return the detected events
  //
  *pEvents = DetectedEvents & ( Events
                              | POLLERR
                              | POLLHUP
                              | POLLNVAL );

  //
  //  Return the operation status
  //
  DEBUG (( DEBUG_POLL, "Exiting SocketPoll, Status: %r\r\n", Status ));
  return Status;
}


/**
  Receive data from a network connection.

  This routine calls the network specific routine to remove the
  next portion of data from the receive queue and return it to the
  caller.

  The ::recvfrom routine calls this routine to determine if any data
  is received from the remote system.  Note that the other routines
  ::recv and ::read are layered on top of ::recvfrom.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.
  
  @param [in] Flags           Message control flags
  
  @param [in] BufferLength    Length of the the buffer
  
  @param [in] pBuffer         Address of a buffer to receive the data.
  
  @param [in] pDataLength     Number of received data bytes in the buffer.

  @param [out] pAddress       Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @param [out] pErrno         Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Socket data successfully received

 **/
EFI_STATUS
EslSocketReceive (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN INT32 Flags,
  IN size_t BufferLength,
  IN UINT8 * pBuffer,
  OUT size_t * pDataLength,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength,
  IN int * pErrno
  )
{
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Validate the socket
  //
  pSocket = NULL;
  if ( NULL != pSocketProtocol ) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

    //
    //  Return the transmit error if necessary
    //
    if ( EFI_SUCCESS != pSocket->TxError ) {
      pSocket->errno = EIO;
      Status = pSocket->TxError;
      pSocket->TxError = EFI_SUCCESS;
    }
    else {
      //
      //  Verify the socket state
      //
      Status = EslSocketIsConfigured ( pSocket );
      if ( !EFI_ERROR ( Status )) {
        //
        //  Validate the buffer length
        //
        if (( NULL == pDataLength )
          && ( 0 > pDataLength )
          && ( NULL == pBuffer )) {
          if ( NULL == pDataLength ) {
            DEBUG (( DEBUG_RX,
                      "ERROR - pDataLength is NULL!\r\n" ));
          }
          else if ( NULL == pBuffer ) {
            DEBUG (( DEBUG_RX,
                      "ERROR - pBuffer is NULL!\r\n" ));
          }
          else {
            DEBUG (( DEBUG_RX,
                      "ERROR - Data length < 0!\r\n" ));
          }
          Status = EFI_INVALID_PARAMETER;
          pSocket->errno = EFAULT;
        }
        else {
          //
          //  Verify the API
          //
          if ( NULL == pSocket->pApi->pfnReceive ) {
            Status = EFI_UNSUPPORTED;
            pSocket->errno = ENOTSUP;
          }
          else {
            //
            //  Synchronize with the socket layer
            //
            RAISE_TPL ( TplPrevious, TPL_SOCKETS );

            //
            //  Attempt to receive a packet
            //
            Status = pSocket->pApi->pfnReceive ( pSocket,
                                                 Flags,
                                                 BufferLength,
                                                 pBuffer,
                                                 pDataLength,
                                                 pAddress,
                                                 pAddressLength);

            //
            //  Release the socket layer synchronization
            //
            RESTORE_TPL ( TplPrevious );
          }
        }
      }
    }
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    if ( NULL != pSocket ) {
      *pErrno = pSocket->errno;
    }
    else {
      Status = EFI_INVALID_PARAMETER;
      *pErrno = EBADF;
    }
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Shutdown the socket receive and transmit operations

  This routine sets a flag to stop future transmissions and calls
  the network specific layer to cancel the pending receive operation.

  The ::shutdown routine calls this routine to stop receive and transmit
  operations on the socket.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.
  
  @param [in] How             Which operations to stop
  
  @param [out] pErrno         Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Socket operations successfully shutdown

 **/
EFI_STATUS
EslSocketShutdown (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN int How,
  IN int * pErrno
  )
{
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;
  
  DBG_ENTER ( );
  
  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Validate the socket
  //
  pSocket = NULL;
  if ( NULL != pSocketProtocol ) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

    //
    //  Verify that the socket is connected
    //
    if ( pSocket->bConnected ) {
      //
      //  Verify the API
      //
      if ( NULL == pSocket->pApi->pfnRxCancel ) {
        Status = EFI_UNSUPPORTED;
        pSocket->errno = ENOTSUP;
      }
      else {
        //
        //  Validate the How value
        //
        if (( SHUT_RD <= How ) && ( SHUT_RDWR >= How )) {
          //
          //  Synchronize with the socket layer
          //
          RAISE_TPL ( TplPrevious, TPL_SOCKETS );

          //
          //  Disable the receiver if requested
          //
          if (( SHUT_RD == How ) || ( SHUT_RDWR == How )) {
            pSocket->bRxDisable = TRUE;
          }

          //
          //  Disable the transmitter if requested
          //
          if (( SHUT_WR == How ) || ( SHUT_RDWR == How )) {
            pSocket->bTxDisable = TRUE;
          }

          //
          //  Cancel the pending receive operation
          //
          Status = pSocket->pApi->pfnRxCancel ( pSocket );

          //
          //  Release the socket layer synchronization
          //
          RESTORE_TPL ( TplPrevious );
        }
        else {
          //
          //  Invalid How value
          //
          pSocket->errno = EINVAL;
          Status = EFI_INVALID_PARAMETER;
        }
      }
    }
    else {
      //
      //  The socket is not connected
      //
      pSocket->errno = ENOTCONN;
      Status = EFI_NOT_STARTED;
    }
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    if ( NULL != pSocket ) {
      *pErrno = pSocket->errno;
    }
    else {
      Status = EFI_INVALID_PARAMETER;
      *pErrno = EBADF;
    }
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Send data using a network connection.

  This routine calls the network specific layer to queue the data
  for transmission.  Eventually the buffer will reach the head of
  the queue and will get transmitted over the network by the
  \ref TransmitEngine.  For datagram
  sockets (SOCK_DGRAM and SOCK_RAW) there is no guarantee that
  the data reaches the application running on the remote system.

  The ::sendto routine calls this routine to send data to the remote
  system.  Note that ::send and ::write are layered on top of ::sendto.

  @param [in] pSocketProtocol Address of an ::EFI_SOCKET_PROTOCOL structure.
  
  @param [in] Flags           Message control flags
  
  @param [in] BufferLength    Length of the the buffer
  
  @param [in] pBuffer         Address of a buffer containing the data to send
  
  @param [in] pDataLength     Address to receive the number of data bytes sent

  @param [in] pAddress        Network address of the remote system address

  @param [in] AddressLength   Length of the remote network address structure

  @param [out] pErrno         Address to receive the errno value upon completion.

  @retval EFI_SUCCESS - Socket data successfully queued for transmit

 **/
EFI_STATUS
EslSocketTransmit (
  IN EFI_SOCKET_PROTOCOL * pSocketProtocol,
  IN int Flags,
  IN size_t BufferLength,
  IN CONST UINT8 * pBuffer,
  OUT size_t * pDataLength,
  IN const struct sockaddr * pAddress,
  IN socklen_t AddressLength,
  IN int * pErrno
  )
{
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;
  EFI_TPL TplPrevious;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Validate the socket
  //
  pSocket = NULL;
  if ( NULL != pSocketProtocol ) {
    pSocket = SOCKET_FROM_PROTOCOL ( pSocketProtocol );

    //
    //  Return the transmit error if necessary
    //
    if ( EFI_SUCCESS != pSocket->TxError ) {
      pSocket->errno = EIO;
      Status = pSocket->TxError;
      pSocket->TxError = EFI_SUCCESS;
    }
    else {
      //
      //  Verify the socket state
      //
      Status = EslSocketIsConfigured ( pSocket );
      if ( !EFI_ERROR ( Status )) {
        //
        //  Verify that transmit is still allowed
        //
        if ( !pSocket->bTxDisable ) {
          //
          //  Validate the buffer length
          //
          if (( NULL == pDataLength )
            && ( 0 > pDataLength )
            && ( NULL == pBuffer )) {
            if ( NULL == pDataLength ) {
              DEBUG (( DEBUG_RX,
                        "ERROR - pDataLength is NULL!\r\n" ));
            }
            else if ( NULL == pBuffer ) {
              DEBUG (( DEBUG_RX,
                        "ERROR - pBuffer is NULL!\r\n" ));
            }
            else {
              DEBUG (( DEBUG_RX,
                        "ERROR - Data length < 0!\r\n" ));
            }
            Status = EFI_INVALID_PARAMETER;
            pSocket->errno = EFAULT;
          }
          else {
            //
            //  Validate the remote network address
            //
            if (( NULL != pAddress )
              && ( AddressLength < pAddress->sa_len )) {
              DEBUG (( DEBUG_TX,
                        "ERROR - Invalid sin_len field in address\r\n" ));
              Status = EFI_INVALID_PARAMETER;
              pSocket->errno = EFAULT;
            }
            else {
              //
              //  Verify the API
              //
              if ( NULL == pSocket->pApi->pfnTransmit ) {
                Status = EFI_UNSUPPORTED;
                pSocket->errno = ENOTSUP;
              }
              else {
                //
                //  Synchronize with the socket layer
                //
                RAISE_TPL ( TplPrevious, TPL_SOCKETS );

                //
                //  Attempt to buffer the packet for transmission
                //
                Status = pSocket->pApi->pfnTransmit ( pSocket,
                                                      Flags,
                                                      BufferLength,
                                                      pBuffer,
                                                      pDataLength,
                                                      pAddress,
                                                      AddressLength );

                //
                //  Release the socket layer synchronization
                //
                RESTORE_TPL ( TplPrevious );
              }
            }
          }
        }
        else {
          //
          //  The transmitter was shutdown
          //
          pSocket->errno = EPIPE;
          Status = EFI_NOT_STARTED;
        }
      }
    }
  }

  //
  //  Return the operation status
  //
  if ( NULL != pErrno ) {
    if ( NULL != pSocket ) {
      *pErrno = pSocket->errno;
    }
    else {
      Status = EFI_INVALID_PARAMETER;
      *pErrno = EBADF;
    }
  }
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Complete the transmit operation

  This support routine removes the ESL_IO_MGMT structure from
  the active queue and returns it to the free queue.

  The network specific code calls this routine during its transmit
  complete processing.  See the \ref TransmitEngine section.

  @param [in] pPort           Address of an ::ESL_PORT structure
  @param [in] pIo             Address of an ::ESL_IO_MGMT structure
  @param [in] ppActive        Active transmit queue address
  @param [in] ppFree          Free transmit queue address

 **/
VOID
EslSocketTxComplete (
  IN ESL_PORT * pPort,
  IN ESL_IO_MGMT * pIo,
  IN ESL_IO_MGMT ** ppActive,
  IN ESL_IO_MGMT ** ppFree
  )
{
  ESL_IO_MGMT * pIoNext;

  //
  //  No more packet
  //
  pIo->pPacket = NULL;

  //
  //  Remove the IO structure from the active list
  //
  pIoNext = *ppActive;
  while (( NULL != pIoNext ) && ( pIoNext != pIo ) && ( pIoNext->pNext != pIo ))
  {
    pIoNext = pIoNext->pNext;
  }
  ASSERT ( NULL != pIoNext );
  if ( pIoNext == pIo ) {
    *ppActive = pIo->pNext;       //  Beginning of list
  }
  else {
    pIoNext->pNext = pIo->pNext;  //  Middle of list
  }

  //
  //  Free the IO structure
  //
  pIo->pNext = *ppFree;
  *ppFree = pIo;

  //
  //  Display the results
  //
  DEBUG (( DEBUG_TX | DEBUG_INFO,
            "0x%08x: pIo Released\r\n",
            pIo ));
}


/**
  Transmit data using a network connection.

  This support routine starts a transmit operation on the
  underlying network layer.

  The network specific code calls this routine to start a
  transmit operation.  See the \ref TransmitEngine section.

  @param [in] pPort           Address of an ::ESL_PORT structure
  @param [in] ppQueueHead     Transmit queue head address
  @param [in] ppQueueTail     Transmit queue tail address
  @param [in] ppActive        Active transmit queue address
  @param [in] ppFree          Free transmit queue address

 **/
VOID
EslSocketTxStart (
  IN ESL_PORT * pPort,
  IN ESL_PACKET ** ppQueueHead,
  IN ESL_PACKET ** ppQueueTail,
  IN ESL_IO_MGMT ** ppActive,
  IN ESL_IO_MGMT ** ppFree
  )
{
  UINT8 * pBuffer;
  ESL_IO_MGMT * pIo;
  ESL_PACKET * pNextPacket;
  ESL_PACKET * pPacket;
  VOID ** ppTokenData;
  ESL_SOCKET * pSocket;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Get the packet from the queue head
  //
  pPacket = *ppQueueHead;
  pIo = *ppFree;
  if (( NULL != pPacket ) && ( NULL != pIo )) {
    //
    //  Remove the packet from the queue
    //
    pNextPacket = pPacket->pNext;
    *ppQueueHead = pNextPacket;
    if ( NULL == pNextPacket ) {
      *ppQueueTail = NULL;
    }

    //
    //  Remove the IO structure from the queue
    //
    *ppFree = pIo->pNext;

    //
    //  Mark this packet as active
    //
    pIo->pPacket = pPacket;
    pIo->pNext = *ppActive;
    *ppActive = pIo;

    //
    //  Connect the token to the transmit data
    //
    pSocket = pPort->pSocket;
    pBuffer = (UINT8 *)&pIo->Token;
    pBuffer = &pBuffer[ pSocket->TxTokenOffset ];
    ppTokenData = (VOID **)pBuffer;
    pBuffer = (UINT8 *)pPacket;
    pBuffer = &pBuffer[ pSocket->TxPacketOffset ];
    *ppTokenData = (VOID **)pBuffer;

    //
    //  Display the results
    //
    DEBUG (( DEBUG_TX | DEBUG_INFO,
              "0x%08x: pIo allocated for pPacket: 0x%08x\r\n",
              pIo,
              pPacket ));

    //
    //  Start the transmit operation
    //
    Status = pPort->pfnTxStart ( pPort->pProtocol,
                                 &pIo->Token );
    if ( EFI_ERROR ( Status )) {
      if ( EFI_SUCCESS == pSocket->TxError ) {
        pSocket->TxError = Status;
      }
    }
  }

  DBG_EXIT ( );
}
