/** @file
  Implement the connection to the EFI socket library

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Socket.h"


/**
  Tag GUID - IPv4 in use by an application using EfiSocketLib
**/
CONST EFI_GUID mEslIp4ServiceGuid = {
  0x9c756011, 0x5d44, 0x4ee0, { 0xbc, 0xe7, 0xc3, 0x82, 0x18, 0xfe, 0x39, 0x8d }
};


/**
  Tag GUID - TCPv4 in use by an application using EfiSocketLib
**/
CONST EFI_GUID mEslTcp4ServiceGuid = {
  0xffc659c2, 0x4ef2, 0x4532, { 0xb8, 0x75, 0xcd, 0x9a, 0xa4, 0x27, 0x4c, 0xde }
};


/**
  Tag GUID - UDPv4 in use by an application using EfiSocketLib
**/
CONST EFI_GUID mEslUdp4ServiceGuid = {
  0x44e03a55, 0x8d97, 0x4511, { 0xbf, 0xef, 0xa, 0x8b, 0xc6, 0x2c, 0x25, 0xae }
};


/**
  Connect to the EFI socket library

  This routine is called from the socket routine in BsdSocketLib
  to create the data structure for a socket.  This specific
  implementation is used when an application links directly to
  the EslSocketLib.

  @param [in] ppSocketProtocol  Address to receive the socket protocol address

  @return       Value for ::errno, zero (0) indicates success.

 **/
int
EslServiceGetProtocol (
  IN EFI_SOCKET_PROTOCOL ** ppSocketProtocol
  )
{
  EFI_HANDLE ChildHandle;
  ESL_SOCKET * pSocket;
  int RetVal;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  RetVal = 0;

  //
  //  Locate the socket protocol
  //
  ChildHandle = NULL;
  Status = EslSocketAllocate ( &ChildHandle,
                               DEBUG_SOCKET,
                               &pSocket );
  if ( !EFI_ERROR ( Status )) {
    *ppSocketProtocol = &pSocket->SocketProtocol;
  }
  else {
    //
    //  No resources
    //
    RetVal = ENOMEM;
  }

  //
  //  Return the operation status
  //
  DBG_EXIT_DEC ( RetVal );
  return RetVal;
}


/**
  Connect to the network layer

  Constructor for the EfiSocketLib when the library is linked
  directly to an application.  This routine locates the network
  devices and makes them available to EfiSocketLib.

  @retval EFI_SUCCESS   Successfully connected to the network layer

 **/
EFI_STATUS
EslServiceNetworkConnect (
  VOID
  )
{
  UINTN HandleCount;
  UINTN Index;
  CONST ESL_SOCKET_BINDING * pEnd;
  EFI_HANDLE * pHandles;
  CONST ESL_SOCKET_BINDING * pSocketBinding;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Initialize the socket layer
  //
  Status = EFI_SUCCESS;
  EslServiceLoad ( gImageHandle );

  //
  //  Connect the network devices
  //
  pSocketBinding = &cEslSocketBinding[0];
  pEnd = &pSocketBinding[ cEslSocketBindingEntries ];
  while ( pEnd > pSocketBinding ) {
    //
    //  Attempt to locate the network adapters
    //
    HandleCount = 0;
    pHandles = NULL;
    Status = gBS->LocateHandleBuffer ( ByProtocol,
                                       pSocketBinding->pNetworkBinding,
                                       NULL,
                                       &HandleCount,
                                       &pHandles );
    if ( EFI_ERROR ( Status )) {
      break;
    }
    if ( NULL != pHandles ) {
      //
      //  Attempt to connect to this network adapter
      //
      for ( Index = 0; HandleCount > Index; Index++ ) {
        Status = EslServiceConnect ( gImageHandle,
                                     pHandles[ Index ]);
        if ( EFI_ERROR ( Status )) {
          break;
        }
      }

      //
      //  Done with the handles
      //
      gBS->FreePool ( pHandles );
    }

    //
    //  Set the next network protocol
    //
    pSocketBinding += 1;
  }

  //
  //  Return the network connection status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Disconnect from the network layer

  Destructor for the EfiSocketLib when the library is linked
  directly to an application.  This routine breaks any connections
  to the network devices and removes them from use by EfiSocketLib.

  @retval EFI_SUCCESS   Successfully disconnected from the network layer

 **/
EFI_STATUS
EslServiceNetworkDisconnect (
  VOID
  )
{
  UINTN HandleCount;
  UINTN Index;
  CONST ESL_SOCKET_BINDING * pEnd;
  EFI_HANDLE * pHandles;
  CONST ESL_SOCKET_BINDING * pSocketBinding;
  EFI_STATUS Status;

  DBG_ENTER ( );

  //
  //  Assume success
  //
  Status = EFI_SUCCESS;

  //
  //  Disconnect the network devices
  //
  pSocketBinding = &cEslSocketBinding[0];
  pEnd = &pSocketBinding[ cEslSocketBindingEntries ];
  while ( pEnd > pSocketBinding ) {
    //
    //  Attempt to locate the network adapters
    //
    HandleCount = 0;
    pHandles = NULL;
    Status = gBS->LocateHandleBuffer ( ByProtocol,
                                       pSocketBinding->pNetworkBinding,
                                       NULL,
                                       &HandleCount,
                                       &pHandles );
    if ( EFI_ERROR ( Status )) {
      break;
    }
    if ( NULL != pHandles ) {
      //
      //  Attempt to disconnect from this network adapter
      //
      for ( Index = 0; HandleCount > Index; Index++ ) {
        Status = EslServiceDisconnect ( gImageHandle,
                                        pHandles[ Index ]);
        if ( EFI_ERROR ( Status )) {
          break;
        }
      }

      //
      //  Done with the handles
      //
      gBS->FreePool ( pHandles );
    }

    //
    //  Set the next network protocol
    //
    pSocketBinding += 1;
  }

  //
  //  Finish the disconnect operation
  //
  if ( !EFI_ERROR ( Status )) {
    EslServiceUnload ( );
  }

  //
  //  Return the network connection status
  //
  DBG_EXIT_STATUS ( Status );
  return Status;
}


/**
  Socket layer's service binding protocol delcaration.
**/
CONST EFI_SERVICE_BINDING_PROTOCOL mEfiServiceBinding = {
  NULL,
  NULL
};


PFN_ESL_xSTRUCTOR mpfnEslConstructor = EslServiceNetworkConnect;    ///<  Constructor for EfiSocketLib
PFN_ESL_xSTRUCTOR mpfnEslDestructor = EslServiceNetworkDisconnect;  ///<  Destructor for EfiSocketLib
