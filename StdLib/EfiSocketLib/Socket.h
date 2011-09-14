/** @file
  Definitions for the Socket layer driver.

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <Efi/EfiSocketLib.h>

//------------------------------------------------------------------------------
//  Constants
//------------------------------------------------------------------------------

#define DEBUG_SOCKET        0x20000000  ///<  Display Socket related messages
#define DEBUG_BIND          0x10000000  ///<  Display bind related messages
#define DEBUG_LISTEN        0x08000000  ///<  Display listen related messages
#define DEBUG_CONNECTION    0x04000000  ///<  Display connection list related messages
#define DEBUG_POLL          0x02000000  ///<  Display poll messages
#define DEBUG_ACCEPT        0x01000000  ///<  Display accept related messages
#define DEBUG_RX            0x00800000  ///<  Display receive messages
#define DEBUG_TX            0x00400000  ///<  Display transmit messages
#define DEBUG_CLOSE         0x00200000  ///<  Display close messages
#define DEBUG_CONNECT       0x00100000  ///<  Display connect messages
#define DEBUG_OPTION        0x00080000  ///<  Display option messages

#define MAX_PENDING_CONNECTIONS     1   ///<  Maximum connection FIFO depth
#define MAX_RX_DATA         65536       ///<  Maximum receive data size
#define MAX_TX_DATA         ( MAX_RX_DATA * 2 ) ///<  Maximum buffered transmit data in bytes
#define RX_PACKET_DATA      16384       ///<  Maximum number of bytes in a RX packet
#define MAX_UDP_RETRANSMIT  16          ///<  UDP retransmit attempts to handle address not mapped

#define ESL_STRUCTURE_ALIGNMENT_BYTES   15  ///<  Number of bytes for structure alignment
#define ESL_STRUCTURE_ALIGNMENT_MASK    ( ~ESL_STRUCTURE_ALIGNMENT_BYTES )  ///<  Mask to align structures

#define LAYER_SIGNATURE           SIGNATURE_32 ('S','k','t','L')  ///<  ESL_LAYER memory signature
#define SERVICE_SIGNATURE         SIGNATURE_32 ('S','k','t','S')  ///<  ESL_SERVICE memory signature
#define SOCKET_SIGNATURE          SIGNATURE_32 ('S','c','k','t')  ///<  ESL_SOCKET memory signature
#define PORT_SIGNATURE            SIGNATURE_32 ('P','o','r','t')  ///<  ESL_PORT memory signature


/**
  Socket states
**/
typedef enum
{
  SOCKET_STATE_NOT_CONFIGURED = 0,  ///<  socket call was successful
  SOCKET_STATE_BOUND,               ///<  bind call was successful
  SOCKET_STATE_LISTENING,           ///<  listen call was successful
  SOCKET_STATE_NO_PORTS,            ///<  No ports available
  SOCKET_STATE_IN_FIFO,             ///<  Socket on FIFO
  SOCKET_STATE_CONNECTING,          ///<  Connecting to a remote system
  SOCKET_STATE_CONNECTED,           ///<  Accept or connect call was successful

  //
  //  Close state must be the last in the list
  //
  SOCKET_STATE_CLOSED               ///<  Close call was successful
} SOCKET_STATE;


/**
  Port states
**/
typedef enum
{
  PORT_STATE_ALLOCATED = 0, ///<  Port allocated
  PORT_STATE_OPEN,          ///<  Port opened
  PORT_STATE_RX_ERROR,      ///<  Receive error detected

  //
  //  Close state must be last in the list
  //
  PORT_STATE_CLOSE_STARTED, ///<  Close started on port
  PORT_STATE_CLOSE_TX_DONE, ///<  Transmits shutdown
  PORT_STATE_CLOSE_RX_DONE, ///<  Receives shutdown
  PORT_STATE_CLOSE_DONE     ///<  Port close operation complete
} PORT_STATE;

//------------------------------------------------------------------------------
//  Data Types
//------------------------------------------------------------------------------

typedef struct _ESL_IO_MGMT ESL_IO_MGMT;///<  Forward declaration
typedef struct _ESL_PACKET ESL_PACKET;  ///<  Forward declaration
typedef struct _ESL_PORT ESL_PORT;      ///<  Forward declaration
typedef struct _ESL_SOCKET ESL_SOCKET;  ///<  Forward declaration

/**
  Receive context for SOCK_RAW sockets using IPv4.
**/
typedef struct
{
  EFI_IP4_RECEIVE_DATA * pRxData;       ///<  Receive operation description
} ESL_IP4_RX_DATA;


/**
  Transmit context for SOCK_RAW sockets using IPv4.
**/
typedef struct
{
  EFI_IP4_OVERRIDE_DATA Override;       ///<  Override data
  EFI_IP4_TRANSMIT_DATA TxData;         ///<  Transmit operation description
  UINT8 Buffer[ 1 ];                    ///<  Data buffer
} ESL_IP4_TX_DATA;


/**
  Receive context for SOCK_STREAM and SOCK_SEQPACKET sockets using TCPv4.
**/
typedef struct
{
  EFI_TCP4_RECEIVE_DATA RxData;         ///<  Receive operation description
  size_t ValidBytes;                    ///<  Length of valid data in bytes
  UINT8 * pBuffer;                      ///<  Current data pointer
  UINT8 Buffer[ RX_PACKET_DATA ];       ///<  Data buffer
} ESL_TCP4_RX_DATA;


/**
  Transmit context for SOCK_STREAM and SOCK_SEQPACKET sockets using TCPv4.
**/
typedef struct
{
  EFI_TCP4_TRANSMIT_DATA TxData;        ///<  Transmit operation description
  UINT8 Buffer[ 1 ];                    ///<  Data buffer
} ESL_TCP4_TX_DATA;


/**
  Receive context for SOCK_DGRAM sockets using UDPv4.
**/
typedef struct
{
  EFI_UDP4_SESSION_DATA Session;        ///<  Remote network address
  EFI_UDP4_RECEIVE_DATA * pRxData;      ///<  Receive operation description
} ESL_UDP4_RX_DATA;


/**
  Transmit context for SOCK_DGRAM sockets using UDPv4.
**/
typedef struct
{
  EFI_UDP4_SESSION_DATA Session;        ///<  Remote network address
  EFI_UDP4_TRANSMIT_DATA TxData;        ///<  Transmit operation description
  UINTN RetransmitCount;                ///<  Retransmit to handle ARP negotiation
  UINT8 Buffer[ 1 ];                    ///<  Data buffer
} ESL_UDP4_TX_DATA;


/**
  Network specific context for transmit and receive packets.
**/
typedef struct _ESL_PACKET {
  ESL_PACKET * pNext;                   ///<  Next packet in the receive list
  size_t PacketSize;                    ///<  Size of this data structure
  union {
    ESL_IP4_RX_DATA Ip4Rx;              ///<  Receive operation description
    ESL_IP4_TX_DATA Ip4Tx;              ///<  Transmit operation description
    ESL_TCP4_RX_DATA Tcp4Rx;            ///<  Receive operation description
    ESL_TCP4_TX_DATA Tcp4Tx;            ///<  Transmit operation description
    ESL_UDP4_RX_DATA Udp4Rx;            ///<  Receive operation description
    ESL_UDP4_TX_DATA Udp4Tx;            ///<  Transmit operation description
  } Op;                                 ///<  Network specific context
} GCC_ESL_PACKET;

/**
  Service control structure

  The driver uses this structure to manage the network devices.
**/
typedef struct _ESL_SERVICE {
  UINTN Signature;          ///<  Structure identification

  //
  //  Links
  //
  ESL_SERVICE * pNext;      ///<  Next service in the service list

  //
  //  Service data
  //
  CONST ESL_SOCKET_BINDING * pSocketBinding;      ///<  Name and shutdown routine
  EFI_HANDLE Controller;                          ///<  Controller for the service
  EFI_SERVICE_BINDING_PROTOCOL * pServiceBinding; ///<  Network layer service binding interface

  //
  //  Network data
  //
  ESL_PORT * pPortList;     ///<  List of ports using this service
}GCC_ESL_SERVICE;

/**
  IO management structure

  This structure manages a single operation with the network.
**/
typedef struct _ESL_IO_MGMT {
  ESL_IO_MGMT * pNext;                ///<  Next TX management structure
  ESL_PORT * pPort;                   ///<  Port structure address
  ESL_PACKET * pPacket;               ///<  Packet structure address
  union {
    EFI_IP4_COMPLETION_TOKEN Ip4Tx;   ///<  IP4 transmit token
    EFI_TCP4_IO_TOKEN Tcp4Tx;         ///<  TCP4 transmit token
    EFI_UDP4_COMPLETION_TOKEN Udp4Tx; ///<  UDP4 transmit token
  } Token;                            ///<  Completion token for the network operation
};

/**
  IP4 context structure

  The driver uses this structure to manage the IP4 connections.
**/
typedef struct {
  //
  //  IP4 context
  //
  EFI_IP4_MODE_DATA ModeData;           ///<  IP4 mode data, includes configuration data
  EFI_IPv4_ADDRESS DestinationAddress;  ///<  Default destination address

  //
  //  Receive data management
  //
  EFI_IP4_COMPLETION_TOKEN RxToken;     ///<  Receive token
} ESL_IP4_CONTEXT;


/**
  TCP4 context structure

  The driver uses this structure to manage the TCP4 connections.
**/
typedef struct {
  //
  //  TCP4 context
  //
  EFI_TCP4_CONFIG_DATA ConfigData;        ///<  TCP4 configuration data
  EFI_TCP4_OPTION Option;                 ///<  TCP4 port options

  //
  //  Tokens
  //
  EFI_TCP4_LISTEN_TOKEN ListenToken;      ///<  Listen control
  EFI_TCP4_CONNECTION_TOKEN ConnectToken; ///<  Connection control
  EFI_TCP4_CLOSE_TOKEN CloseToken;        ///<  Close control

  //
  //  Receive data management
  //
  EFI_TCP4_IO_TOKEN RxToken;              ///<  Receive token
} ESL_TCP4_CONTEXT;

/**
  UDP4 context structure

  The driver uses this structure to manage the UDP4 connections.
**/
typedef struct {
  //
  //  UDP4 context
  //
  EFI_UDP4_CONFIG_DATA ConfigData;  ///<  UDP4 configuration data

  //
  //  Receive data management
  //
  EFI_UDP4_COMPLETION_TOKEN RxToken;///<  Receive token
} ESL_UDP4_CONTEXT;


/**
  Hand an I/O operation to the network layer.

  @param [in] pProtocol   Protocol structure address
  @param [in] pToken      Completion token address

  @return   Returns EFI_SUCCESS if the operation is successfully
            started.
**/
typedef
EFI_STATUS
(* PFN_NET_TX_START) (
  IN VOID * pProtocol,
  IN VOID * pToken
  );

/**
  Port control structure

  The driver uses this structure to manager the socket's connection
  with the network driver.
**/
typedef struct _ESL_PORT {
  UINTN Signature;              ///<  Structure identification

  //
  //  List links
  //
  ESL_PORT * pLinkService;      ///<  Link in service port list
  ESL_PORT * pLinkSocket;       ///<  Link in socket port list

  //
  //  Structures
  //
  ESL_SERVICE * pService;       ///<  Service for this port
  ESL_SOCKET * pSocket;         ///<  Socket for this port

  //
  //  Eliminate the pService references during port close
  //
  EFI_SERVICE_BINDING_PROTOCOL * pServiceBinding; ///<  Service binding for network layer
  CONST ESL_SOCKET_BINDING * pSocketBinding;      ///<  Socket binding for network layer

  //
  //  Port management
  //
  EFI_HANDLE Handle;            ///<  Network port handle
  PORT_STATE State;             ///<  State of the port
  UINTN DebugFlags;             ///<  Debug flags used to close the port
  BOOLEAN bCloseNow;            ///<  TRUE = Close the port immediately
  BOOLEAN bConfigured;          ///<  TRUE = Configure call made to network layer

  //
  //  Transmit data management
  //
  PFN_NET_TX_START pfnTxStart;  ///<  Start a transmit on the network
  ESL_IO_MGMT * pTxActive;      ///<  Normal data queue
  ESL_IO_MGMT * pTxFree;        ///<  Normal free queue

  ESL_IO_MGMT * pTxOobActive;   ///<  Urgent data queue
  ESL_IO_MGMT * pTxOobFree;     ///<  Urgent free queue

  //
  //  Receive data management
  //
  ESL_PACKET * pReceivePending; ///<  Receive operation in progress

  //
  //  Protocol specific management data
  //
  union {
    VOID * v;                   ///<  VOID pointer
    EFI_IP4_PROTOCOL * IPv4;    ///<  IP4 protocol pointer
    EFI_TCP4_PROTOCOL * TCPv4;  ///<  TCP4 protocol pointer
    EFI_UDP4_PROTOCOL * UDPv4;  ///<  UDP4 protocol pointer
  } pProtocol;                  ///<  Protocol structure address
  union {
    ESL_IP4_CONTEXT Ip4;        ///<  IPv4 management data
    ESL_TCP4_CONTEXT Tcp4;      ///<  TCPv4 management data
    ESL_UDP4_CONTEXT Udp4;      ///<  UDPv4 management data
  } Context;                    ///<  Network specific context
}GCC_ESL_PORT;

/**
  Accept a network connection.

  @param [in] pSocket   Address of the socket structure.

  @param [in] pSockAddr       Address of a buffer to receive the remote
                              network address.

  @param [in, out] pSockAddrLength  Length in bytes of the address buffer.
                                    On output specifies the length of the
                                    remote network address.

  @retval EFI_SUCCESS   Remote address is available
  @retval Others        Remote address not available

 **/
typedef
EFI_STATUS
(* PFN_API_ACCEPT) (
  IN ESL_SOCKET * pSocket,
  IN struct sockaddr * pSockAddr,
  IN OUT socklen_t * pSockAddrLength
  );

/**
  Connect to a remote system via the network.

  @param [in] pSocket         Address of the socket structure.

  @param [in] pSockAddr       Network address of the remote system.
    
  @param [in] SockAddrLength  Length in bytes of the network address.
  
  @retval EFI_SUCCESS   The connection was successfully established.
  @retval EFI_NOT_READY The connection is in progress, call this routine again.
  @retval Others        The connection attempt failed.

 **/
typedef
EFI_STATUS
(* PFN_API_CONNECT_START) (
  IN ESL_SOCKET * pSocket,
  IN const struct sockaddr * pSockAddr,
  IN socklen_t SockAddrLength
  );

/**
  Poll for completion of the connection attempt.

  @param [in] pSocket         Address of the socket structure.

  @retval EFI_SUCCESS   The connection was successfully established.
  @retval EFI_NOT_READY The connection is in progress, call this routine again.
  @retval Others        The connection attempt failed.

 **/
typedef
EFI_STATUS
(* PFN_API_CONNECT_POLL) (
  IN ESL_SOCKET * pSocket
  );

/**
  Get the local socket address

  @param [in] pSocket             Address of the socket structure.

  @param [out] pAddress           Network address to receive the local system address

  @param [in,out] pAddressLength  Length of the local network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
typedef
EFI_STATUS
(* PFN_API_GET_LOCAL_ADDR) (
  IN ESL_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Get the remote socket address

  @param [in] pSocket             Address of the socket structure.

  @param [out] pAddress           Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
typedef
EFI_STATUS
(* PFN_API_GET_RMT_ADDR) (
  IN ESL_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Determine if the socket is configured.


  @param [in] pSocket         Address of a ESL_SOCKET structure
  
  @retval EFI_SUCCESS - The port is connected
  @retval EFI_NOT_STARTED - The port is not connected

 **/
 typedef
 EFI_STATUS
 (* PFN_API_IS_CONFIGURED) (
  IN ESL_SOCKET * pSocket
  );

/**
  Establish the known port to listen for network connections.

  @param [in] pSocket     Address of the socket structure.

  @retval EFI_SUCCESS - Socket successfully created
  @retval Other - Failed to enable the socket for listen

**/
typedef
EFI_STATUS
(* PFN_API_LISTEN) (
  IN ESL_SOCKET * pSocket
  );

/**
  Get the option value

  Retrieve the protocol options one at a time by name.

  @param [in] pSocket           Address of a ESL_SOCKET structure
  @param [in] level             Option protocol level
  @param [in] OptionName        Name of the option
  @param [out] ppOptionData     Buffer to receive address of option value
  @param [out] pOptionLength    Buffer to receive the option length

  @retval EFI_SUCCESS - Socket data successfully received

 **/
typedef
EFI_STATUS
(* PFN_API_OPTION_GET) (
  IN ESL_SOCKET * pSocket,
  IN int level,
  IN int OptionName,
  OUT CONST void ** __restrict ppOptionData,
  OUT socklen_t * __restrict pOptionLength
  );

/**
  Set the option value

  Adjust the protocol options one at a time by name.

  @param [in] pSocket         Address of a ESL_SOCKET structure
  @param [in] level           Option protocol level
  @param [in] OptionName      Name of the option
  @param [in] pOptionValue    Buffer containing the option value
  @param [in] OptionLength    Length of the buffer in bytes

  @retval EFI_SUCCESS - Option successfully set

 **/
typedef
EFI_STATUS
(* PFN_API_OPTION_SET) (
  IN ESL_SOCKET * pSocket,
  IN int level,
  IN int OptionName,
  IN CONST void * pOptionValue,
  IN socklen_t OptionLength
  );

/**
  Initialize the network specific portions of an ::ESL_PORT structure.

  This routine initializes the network specific portions of an
  ::ESL_PORT structure for use by the socket.

  This support routine is called by ::EslSocketPortAllocate
  to connect the socket with the underlying network adapter
  running the IPv4 protocol.

  @param [in] ppPort      Address of an ESL_PORT structure
  @param [in] pIpAddress  Buffer containing IP4 network address of the local host
  @param [in] PortNumber  Tcp4 port number
  @param [in] DebugFlags  Flags for debug messages

  @retval EFI_SUCCESS - Socket successfully created

 **/
typedef
EFI_STATUS
(* PFN_API_PORT_ALLOC) (
  IN ESL_PORT * pPort,
  IN CONST UINT8 * pIpAddress,
  IN UINT16 PortNumber,
  IN UINTN DebugFlags
  );

/**
  Close a network specific port.

  This routine releases the resources allocated by the
  network specific PortAllocate routine.

  This routine is called by ::EslSocketPortCloseRxDone as
  the last step of closing processing.
  See the \ref Tcp4PortCloseStateMachine section.
  
  @param [in] pPort       Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS     The port is closed
  @retval other           Port close error

**/
typedef
EFI_STATUS
(* PFN_API_PORT_CLOSE) (
  IN ESL_PORT * pPort
  );

/**
  Perform the network specific close operation on the port.

  This routine performs the network specific operation to
  shutdown receive operations on the port.

  This routine is called by the ::EslSocketPortCloseTxDone
  routine after the port completes all of the transmission.

  @param [in] pPort           Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS         The port is closed, not normally returned
  @retval EFI_NOT_READY       The port is still closing
  @retval EFI_ALREADY_STARTED Error, the port is in the wrong state,
                              most likely the routine was called already.

**/
typedef
EFI_STATUS
(* PFN_API_PORT_CLOSE_OP) (
  IN ESL_PORT * pPort
  );

/**
  Free a receive packet

  This routine performs the network specific operations necessary
  to free a receive packet.

  This routine is called by ::EslSocketPortCloseTx to free a
  receive packet.

  @param [in] pPacket         Address of an ::ESL_PACKET structure.
  @param [in, out] pRxBytes   Address of the count of RX bytes

**/
typedef
VOID
(* PFN_API_PORT_CLOSE_PF) (
  IN ESL_PACKET * pPacket,
  IN OUT size_t * pRxBytes
  );

/**
  Receive data from a network connection.

  @param [in] pSocket         Address of a ESL_SOCKET structure
  
  @param [in] Flags           Message control flags
  
  @param [in] BufferLength    Length of the the buffer
  
  @param [in] pBuffer         Address of a buffer to receive the data.
  
  @param [in] pDataLength     Number of received data bytes in the buffer.

  @param [out] pAddress       Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @retval EFI_SUCCESS - Socket data successfully received

**/
typedef
EFI_STATUS
(* PFN_API_RECEIVE) (
  IN ESL_SOCKET * pSocket,
  IN INT32 Flags,
  IN size_t BufferLength,
  IN UINT8 * pBuffer,
  OUT size_t * pDataLength,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Cancel the receive operations

  @param [in] pSocket         Address of a ESL_SOCKET structure
  
  @retval EFI_SUCCESS - The cancel was successful

 **/
typedef
EFI_STATUS
(* PFN_API_RX_CANCEL) (
  IN ESL_SOCKET * pSocket
  );

/**
  Buffer data for transmission over a network connection.

  @param [in] pSocket         Address of a ESL_SOCKET structure

  @param [in] Flags           Message control flags

  @param [in] BufferLength    Length of the the buffer

  @param [in] pBuffer         Address of a buffer to receive the data.

  @param [in] pDataLength     Number of received data bytes in the buffer.

  @param [in] pAddress        Network address of the remote system address

  @param [in] AddressLength   Length of the remote network address structure

  @retval EFI_SUCCESS - Socket data successfully buffered

**/
typedef
EFI_STATUS
(* PFN_API_TRANSMIT) (
  IN ESL_SOCKET * pSocket,
  IN int Flags,
  IN size_t BufferLength,
  IN CONST UINT8 * pBuffer,
  OUT size_t * pDataLength,
  IN const struct sockaddr * pAddress,
  IN socklen_t AddressLength
  );

/**
  Process the transmit completion

  This routine calls ::EslSocketTxComplete to handle the
  transmit completion.

  This routine is called by the network layers upon the completion
  of a transmit operation.

  @param [in] Event     The urgent transmit completion event

  @param [in] pIo       The ESL_IO_MGMT structure address

**/
typedef
VOID
(* PFN_API_TX_COMPLETE) (
  IN EFI_EVENT Event,
  IN ESL_IO_MGMT * pIo
  );

/**
  Socket type control structure

  This driver uses this structure to define the API for the socket type.
**/
typedef struct {
  int DefaultProtocol;                      ///<  Default protocol
  UINTN ServiceListOffset;                  ///<  Offset in ::ESL_LAYER for the list of services
  UINTN MinimumAddressLength;               ///<  Minimum address length in bytes
  PFN_API_ACCEPT pfnAccept;                 ///<  Accept a network connection
  PFN_API_CONNECT_START pfnConnectStart;    ///<  Start the connection to a remote system
  PFN_API_CONNECT_POLL pfnConnectPoll;      ///<  Poll for connection complete
  PFN_API_GET_LOCAL_ADDR pfnGetLocalAddr;   ///<  Get local address
  PFN_API_GET_RMT_ADDR pfnGetRemoteAddr;    ///<  Get remote address
  PFN_API_IS_CONFIGURED pfnIsConfigured;    ///<  Determine if the socket is configured
  PFN_API_LISTEN pfnListen;                 ///<  Listen for connections on known server port
  PFN_API_OPTION_GET pfnOptionGet;          ///<  Get the option value
  PFN_API_OPTION_SET pfnOptionSet;          ///<  Set the option value
  PFN_API_PORT_ALLOC pfnPortAllocate;       ///<  Allocate the network specific resources for the port
  PFN_API_PORT_CLOSE pfnPortClose;          ///<  Close the network specific resources for the port
  PFN_API_PORT_CLOSE_PF pfnPortClosePktFree;///<  Free the receive packet
  PFN_API_PORT_CLOSE_OP pfnPortCloseRxStop; ///<  Perform the close operation on the port
  BOOLEAN bPortCloseComplete;               ///<  TRUE = Close is complete after close operation
  PFN_API_RECEIVE pfnReceive;               ///<  Attempt to receive some data
  PFN_API_RX_CANCEL pfnRxCancel;            ///<  Cancel a receive operation
  PFN_API_TRANSMIT pfnTransmit;             ///<  Attempt to buffer a packet for transmit
  PFN_API_TX_COMPLETE pfnTxComplete;        ///<  TX completion for normal data
  PFN_API_TX_COMPLETE pfnTxOobComplete;     ///<  TX completion for urgent data
} ESL_PROTOCOL_API;


/**
  Socket control structure

  The driver uses this structure to manage the socket.
**/
typedef struct _ESL_SOCKET {
  UINTN Signature;          ///<  Structure identification

  //
  //  Protocol binding
  //
  EFI_SOCKET_PROTOCOL SocketProtocol; ///<  Socket protocol declaration
  CONST ESL_PROTOCOL_API * pApi;      ///<  API for the protocol

  //
  //  Socket management
  //
  ESL_SOCKET * pNext;           ///<  Next socket in the list of sockets
  int errno;                    ///<  Error information for this socket
  EFI_STATUS Status;            ///<  Asyncronous error information for this socket
  SOCKET_STATE State;           ///<  Socket state

  //
  //  Socket options
  //
  BOOLEAN bOobInLine;           ///<  TRUE if out-of-band messages are to be received inline with normal data
  BOOLEAN bIncludeHeader;       ///<  TRUE if including the IP header

  //
  //  Socket data
  //
  int Domain;                   ///<  Specifies family of protocols
  int Type;                     ///<  Specifies how to make network connection
  int Protocol;                 ///<  Specifies lower layer protocol to use
  BOOLEAN bConfigured;          ///<  Set after the socket is configured

  BOOLEAN bRxDisable;           ///<  Receive disabled via shutdown
  size_t RxBytes;               ///<  Total Rx bytes
  size_t RxOobBytes;            ///<  Urgent Rx bytes
  EFI_STATUS RxError;           ///<  Error during receive

  BOOLEAN bTxDisable;           ///<  Transmit disabled via shutdown
  size_t TxBytes;               ///<  Normal Tx bytes
  size_t TxOobBytes;            ///<  Urgent Tx bytes
  EFI_STATUS TxError;           ///<  Error during transmit

  //
  //  Pending connection data
  //
  BOOLEAN bConnected;           ///<  Set when connected, cleared by poll
  EFI_STATUS ConnectStatus;     ///<  Connection status
  UINTN MaxFifoDepth;           ///<  Maximum FIFO depth
  UINTN FifoDepth;              ///<  Number of sockets in the FIFO
  ESL_SOCKET * pFifoHead;       ///<  Head of the FIFO
  ESL_SOCKET * pFifoTail;       ///<  Tail of the FIFO
  ESL_SOCKET * pNextConnection; ///<  Link in the FIFO

  //
  //  Network use
  //
  ESL_PORT * pPortList;         ///<  List of ports managed by this socket
  EFI_EVENT WaitAccept;         ///<  Wait for accept completion

  //
  //  Receive data management
  //
  UINT32 MaxRxBuf;                  ///<  Maximum size of the receive buffer
  struct timeval RxTimeout;         ///<  Receive timeout
  ESL_PACKET * pRxFree;             ///<  Free packet list
  ESL_PACKET * pRxOobPacketListHead;///<  Urgent data list head
  ESL_PACKET * pRxOobPacketListTail;///<  Urgent data list tail
  ESL_PACKET * pRxPacketListHead;   ///<  Normal data list head
  ESL_PACKET * pRxPacketListTail;   ///<  Normal data list tail

  //
  //  Transmit data management
  //
  UINTN TxPacketOffset;             ///<  Offset for data pointer in ::ESL_PACKET
  UINTN TxTokenEventOffset;         ///<  Offset to the Event in the TX token
  UINTN TxTokenOffset;              ///<  Offset for data pointer in TX token
  UINT32 MaxTxBuf;                  ///<  Maximum size of the transmit buffer
  ESL_PACKET * pTxOobPacketListHead;///<  Urgent data list head
  ESL_PACKET * pTxOobPacketListTail;///<  Urgent data list tail
  ESL_PACKET * pTxPacketListHead;   ///<  Normal data list head
  ESL_PACKET * pTxPacketListTail;   ///<  Normal data list tail
}GCC_ESL_SOCKET;

#define SOCKET_FROM_PROTOCOL(a)  CR (a, ESL_SOCKET, SocketProtocol, SOCKET_SIGNATURE)  ///< Locate ESL_SOCKET from protocol

/**
  Socket layer control structure

  The driver uses this structure to manage the driver.
**/
typedef struct {
  UINTN Signature;              ///<  Structure identification

  //
  //  Service binding interface
  //
  CONST EFI_SERVICE_BINDING_PROTOCOL * pServiceBinding; ///<  Driver's binding

  //
  //  Image data
  //
  EFI_HANDLE ImageHandle;       ///<  Image handle

  //
  //  Network services
  //
  ESL_SERVICE * pIp4List;       ///<  List of Ip4 services
  ESL_SERVICE * pTcp4List;      ///<  List of Tcp4 services
  ESL_SERVICE * pUdp4List;      ///<  List of Udp4 services

  //
  //  Socket management
  //
  ESL_SOCKET * pSocketList;     ///<  List of sockets
} ESL_LAYER;

#define LAYER_FROM_SERVICE(a) CR (a, ESL_LAYER, ServiceBinding, LAYER_SIGNATURE) ///< Locate ESL_LAYER from service binding

//------------------------------------------------------------------------------
// Data
//------------------------------------------------------------------------------

extern ESL_LAYER mEslLayer;

extern CONST ESL_PROTOCOL_API cEslIp4Api;
extern CONST ESL_PROTOCOL_API cEslTcp4Api;
extern CONST ESL_PROTOCOL_API cEslUdp4Api;

extern CONST EFI_SERVICE_BINDING_PROTOCOL mEfiServiceBinding;

//------------------------------------------------------------------------------
// Socket Support Routines
//------------------------------------------------------------------------------

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
  );

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

  @retval EFI_SUCCESS - The structures were properly initialized

**/
EFI_STATUS
EslSocketIoFree (
  IN ESL_PORT * pPort,
  IN ESL_IO_MGMT ** ppFreeQueue,
  IN UINTN DebugFlags,
  IN CHAR8 * pEventName
  );

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
  IN EFI_EVENT_NOTIFY pfnCompletion
  );

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
  );

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
  );

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
  );

/**
  Allocate and initialize a ESL_PORT structure.

  This routine initializes an ::ESL_PORT structure for use by
  the socket.  This routine calls a routine via
  ESL_PROTOCOL_API::pfnPortAllocate to initialize the network
  specific resources.  The resources are released later by the
  \ref PortCloseStateMachine.

  This support routine is called by:
  <ul>
    <li>::EslIp4Bind</li>
    <li>::EslTcp4Bind</li>
    <li>::EslTcp4ListenComplete</li>
    <li>::EslUdp4Bind::</li>
  to connect the socket with the underlying network adapter
  to the socket.

  @param [in] pSocket     Address of an ::ESL_SOCKET structure.
  @param [in] pService    Address of an ::ESL_SERVICE structure.
  @param [in] ChildHandle TCP4 child handle
  @param [in] pIpAddress  Buffer containing IP4 network address of the local host
  @param [in] PortNumber  Tcp4 port number
  @param [in] DebugFlags  Flags for debug messages
  @param [out] ppPort     Buffer to receive new ::ESL_PORT structure address

  @retval EFI_SUCCESS - Socket successfully created

 **/
EFI_STATUS
EslSocketPortAllocate (
  IN ESL_SOCKET * pSocket,
  IN ESL_SERVICE * pService,
  IN EFI_HANDLE ChildHandle,
  IN CONST UINT8 * pIpAddress,
  IN UINT16 PortNumber,
  IN UINTN DebugFlags,
  OUT ESL_PORT ** ppPort
  );

/**
  Close a port.

  This routine releases the resources allocated by ::EslSocketPortAllocate.
  This routine calls ESL_PROTOCOL_API::pfnPortClose to release the network
  specific resources.

  This routine is called by:
  <ul>
    <li>::EslIp4PortAllocate - Port initialization failure</li>
    <li>::EslSocketPortCloseRxDone - Last step of close processing</li>
    <li>::EslTcp4ConnectComplete - Connection failure and reducint the port list to a single port</li>
    <li>::EslTcp4PortAllocate - Port initialization failure</li>
    <li>::EslUdp4PortAllocate - Port initialization failure</li>
  </ul>
  See the \ref PortCloseStateMachine section.
  
  @param [in] pPort       Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS     The port is closed
  @retval other           Port close error

**/
EFI_STATUS
EslSocketPortClose (
  IN ESL_PORT * pPort
  );

/**
  Process the port close completion event

  This routine attempts to complete the port close operation.

  This routine is called by the TCP layer upon completion of
  the close operation.
  See the \ref PortCloseStateMachine section.

  @param [in] Event     The close completion event

  @param [in] pPort     Address of an ::ESL_PORT structure.

**/
VOID
EslSocketPortCloseComplete (
  IN EFI_EVENT Event,
  IN ESL_PORT * pPort
  );

/**
  Port close state 3

  This routine determines the state of the receive operations and
  continues the close operation after the pending receive operations
  are cancelled.

  This routine is called by
  <ul>
    <li>::EslIp4RxComplete</li>
    <li>::EslSocketPortCloseTxDone</li>
    <li>::EslTcp4PortCloseComplete</li>
    <li>::EslUdp4RxComplete</li>
  </ul>
  to determine the state of the receive operations.
  See the \ref Tcp4PortCloseStateMachine section.

  @param [in] pPort       Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS         The port is closed
  @retval EFI_NOT_READY       The port is still closing
  @retval EFI_ALREADY_STARTED Error, the port is in the wrong state,
                              most likely the routine was called already.

**/
EFI_STATUS
EslSocketPortCloseRxDone (
  IN ESL_PORT * pPort
  );

/**
  Start the close operation on a port, state 1.

  This routine marks the port as closed and initiates the \ref
  PortCloseStateMachine. The first step is to allow the \ref
  TransmitEngine to run down.

  This routine is called by ::EslSocketCloseStart to initiate the socket
  network specific close operation on the socket.

  @param [in] pPort       Address of an ::ESL_PORT structure.
  @param [in] bCloseNow   Set TRUE to abort active transfers
  @param [in] DebugFlags  Flags for debug messages

  @retval EFI_SUCCESS         The port is closed, not normally returned
  @retval EFI_NOT_READY       The port has started the closing process
  @retval EFI_ALREADY_STARTED Error, the port is in the wrong state,
                              most likely the routine was called already.

**/
EFI_STATUS
EslSocketPortCloseStart (
  IN ESL_PORT * pPort,
  IN BOOLEAN bCloseNow,
  IN UINTN DebugFlags
  );

/**
  Port close state 2

  This routine determines the state of the transmit engine and
  continue the close operation after the transmission is complete.
  The next step is to stop the \ref Tcp4ReceiveEngine.
  See the \ref PortCloseStateMachine section.

  This routine is called by ::EslSocketPortCloseStart to determine
  if the transmission is complete.

  @param [in] pPort           Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS         The port is closed, not normally returned
  @retval EFI_NOT_READY       The port is still closing
  @retval EFI_ALREADY_STARTED Error, the port is in the wrong state,
                              most likely the routine was called already.

**/
EFI_STATUS
EslSocketPortCloseTxDone (
  IN ESL_PORT * pPort
  );

/**
  Complete the transmit operation

  This support routine handles the transmit completion processing for
  the various network layers.  It frees the ::ESL_IO_MGMT structure
  and and frees packet resources by calling ::EslSocketPacketFree.
  Transmit errors are logged in ESL_SOCKET::TxError.
  See the \ref TransmitEngine section.

  This routine is called by:
  <ul>
    <li>::EslIp4TxComplete</li>
    <li>::EslTcp4TxComplete</li>
    <li>::EslTcp4TxOobComplete</li>
    <li>::EslUdp4TxComplete</li>
  </ul>

  @param [in] pIo             Address of an ::ESL_IO_MGMT structure
  @param [in] LengthInBytes   Length of the data in bytes
  @param [in] Status          Transmit operation status
  @param [in] pQueueType      Zero terminated string describing queue type
  @param [in] ppQueueHead     Transmit queue head address
  @param [in] ppQueueTail     Transmit queue tail address
  @param [in] ppActive        Active transmit queue address
  @param [in] ppFree          Free transmit queue address

 **/
VOID
EslSocketTxComplete (
  IN ESL_IO_MGMT * pIo,
  IN UINT32 LengthInBytes,
  IN EFI_STATUS Status,
  IN CONST CHAR8 * pQueueType,
  IN ESL_PACKET ** ppQueueHead,
  IN ESL_PACKET ** ppQueueTail,
  IN ESL_IO_MGMT ** ppActive,
  IN ESL_IO_MGMT ** ppFree
  );

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
  );

//------------------------------------------------------------------------------
// Ip4 Routines
//------------------------------------------------------------------------------

/**
  Set the default remote system address.

  This routine sets the default remote address for a SOCK_RAW
  socket using the IPv4 network layer.

  This routine is called by ::EslSocketConnect to initiate the IPv4
  network specific connect operations.  The connection processing is
  limited to setting the default remote network address.

  @param [in] pSocket         Address of an ::ESL_SOCKET structure.

  @param [in] pSockAddr       Network address of the remote system.
    
  @param [in] SockAddrLength  Length in bytes of the network address.
  
  @retval EFI_SUCCESS   The connection was successfully established.
  @retval EFI_NOT_READY The connection is in progress, call this routine again.
  @retval Others        The connection attempt failed.

 **/
EFI_STATUS
EslIp4Connect (
  IN ESL_SOCKET * pSocket,
  IN const struct sockaddr * pSockAddr,
  IN socklen_t SockAddrLength
  );

/**
  Get the local socket address

  This routine returns the IPv4 address associated with the local
  socket.

  This routine is called by ::EslSocketGetLocalAddress to determine the
  network address for the SOCK_RAW socket.

  @param [in] pSocket             Address of an ::ESL_SOCKET structure.

  @param [out] pAddress           Network address to receive the local system address

  @param [in,out] pAddressLength  Length of the local network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
EFI_STATUS
EslIp4GetLocalAddress (
  IN ESL_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Get the remote socket address

  This routine returns the address of the remote connection point
  associated with the SOCK_RAW socket.

  This routine is called by ::EslSocketGetPeerAddress to detemine
  the IPv4 address associated with the network adapter.

  @param [in] pSocket             Address of an ::ESL_SOCKET structure.

  @param [out] pAddress           Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
EFI_STATUS
EslIp4GetRemoteAddress (
  IN ESL_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Get the option value

  This routine handles the IPv4 level options.

  The ::EslSocketOptionGet routine calls this routine to retrieve
  the IPv4 options one at a time by name.

  @param [in] pSocket           Address of an ::ESL_SOCKET structure
  @param [in] level             Option protocol level
  @param [in] OptionName        Name of the option
  @param [out] ppOptionData     Buffer to receive address of option value
  @param [out] pOptionLength    Buffer to receive the option length

  @retval EFI_SUCCESS - Socket data successfully received

 **/
EFI_STATUS
EslIp4OptionGet (
  IN ESL_SOCKET * pSocket,
  IN int level,
  IN int OptionName,
  OUT CONST void ** __restrict ppOptionData,
  OUT socklen_t * __restrict pOptionLength
  );

/**
  Set the option value

  This routine handles the IPv4 level options.

  The ::EslSocketOptionSet routine calls this routine to adjust
  the IPv4 options one at a time by name.

  @param [in] pSocket         Address of an ::ESL_SOCKET structure
  @param [in] level           Option protocol level
  @param [in] OptionName      Name of the option
  @param [in] pOptionValue    Buffer containing the option value
  @param [in] OptionLength    Length of the buffer in bytes

  @retval EFI_SUCCESS - Option successfully set

 **/
EFI_STATUS
EslIp4OptionSet (
  IN ESL_SOCKET * pSocket,
  IN int level,
  IN int OptionName,
  IN CONST void * pOptionValue,
  IN socklen_t OptionLength
  );

/**
  Initialize the network specific portions of an ::ESL_PORT structure.

  This routine initializes the network specific portions of an
  ::ESL_PORT structure for use by the socket.

  This support routine is called by ::EslSocketPortAllocate
  to connect the socket with the underlying network adapter
  running the IPv4 protocol.

  @param [in] ppPort      Address of an ESL_PORT structure
  @param [in] pIpAddress  Buffer containing IP4 network address of the local host
  @param [in] PortNumber  Port number - not used
  @param [in] DebugFlags  Flags for debug messages

  @retval EFI_SUCCESS - Socket successfully created

 **/
EFI_STATUS
EslIp4PortAllocate (
  IN ESL_PORT * pPort,
  IN CONST UINT8 * pIpAddress,
  IN UINT16 PortNumber,
  IN UINTN DebugFlags
  );

/**
  Close an IP4 port.

  This routine releases the resources allocated by
  ::EslIp4PortAllocate.

  This routine is called by:
  <ul>
    <li>::EslIp4PortAllocate - Port initialization failure</li>
    <li>::EslIp4PortCloseRxDone - Last step of close processing</li>
  </ul>
  See the \ref Ip4PortCloseStateMachine section.

  @param [in] pPort       Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS     The port is closed
  @retval other           Port close error

**/
EFI_STATUS
EslIp4PortClose (
  IN ESL_PORT * pPort
  );

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
EslIp4PortClosePacketFree (
  IN ESL_PACKET * pPacket,
  IN OUT size_t * pRxBytes
  );

/**
  Perform the network specific close operation on the port.

  This routine performs a cancel operations on the IPv4 port to
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
EslIp4PortCloseRxStop (
  IN ESL_PORT * pPort
  );

/**
  Receive data from a network connection.

  This routine attempts to return buffered data to the caller.  The
  data is only removed from the normal queue, the message flag
  MSG_OOB is ignored.  See the \ref Ip4ReceiveEngine section.

  This routine is called by ::EslSocketReceive to handle the network
  specific receive operation to support SOCK_RAW sockets.

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
EslIp4Receive (
  IN ESL_SOCKET * pSocket,
  IN INT32 Flags,
  IN size_t BufferLength,
  IN UINT8 * pBuffer,
  OUT size_t * pDataLength,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Cancel the receive operations

  This routine cancels the pending receive operations.
  See the \ref Ip4ReceiveEngine section.

  This routine is called by ::EslSocketShutdown when the socket
  layer is being shutdown.

  @param [in] pSocket   Address of an ::ESL_SOCKET structure
  
  @retval EFI_SUCCESS - The cancel was successful

 **/
EFI_STATUS
EslIp4RxCancel (
  IN ESL_SOCKET * pSocket
  );

/**
  Process the receive completion

  This routine keeps the IPv4 driver's buffer and queues it in
  in FIFO order to the data queue.  The IP4 driver's buffer will
  be returned by either ::EslIp4Receive or ::EslSocketPortCloseTxDone.
  See the \ref Tcp4ReceiveEngine section.

  This routine is called by the IPv4 driver when data is
  received.

  @param [in] Event     The receive completion event

  @param [in] pPort     The address of an ::ESL_PORT structure

**/
VOID
EslIp4RxComplete (
  IN EFI_EVENT Event,
  IN ESL_PORT * pPort
  );

/**
  Start a receive operation

  This routine posts a receive buffer to the IPv4 driver.
  See the \ref Ip4ReceiveEngine section.

  This support routine is called by:
  <ul>
    <li>::EslIp4SocketIsConfigured to start the recevie engine for the new socket.</li>
    <li>::EslIp4Receive to restart the receive engine to release flow control.</li>
    <li>::EslIp4RxComplete to continue the operation of the receive engine if flow control is not being applied.</li>
  </ul>

  @param [in] pPort       Address of an ::ESL_PORT structure.

 **/
VOID
EslIp4RxStart (
  IN ESL_PORT * pPort
  );

/**
  Determine if the socket is configured.

  This routine uses the flag ESL_SOCKET::bConfigured to determine
  if the network layer's configuration routine has been called.
  This routine calls the bind and configuration routines if they
  were not already called.  After the port is configured, the
  \ref Ip4ReceiveEngine is started.

  This routine is called by EslSocketIsConfigured to verify
  that the socket is configured.

  @param [in] pSocket         Address of an ::ESL_SOCKET structure
  
  @retval EFI_SUCCESS - The port is connected
  @retval EFI_NOT_STARTED - The port is not connected

 **/
 EFI_STATUS
 EslIp4SocketIsConfigured (
  IN ESL_SOCKET * pSocket
  );

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
EslIp4TxBuffer (
  IN ESL_SOCKET * pSocket,
  IN int Flags,
  IN size_t BufferLength,
  IN CONST UINT8 * pBuffer,
  OUT size_t * pDataLength,
  IN const struct sockaddr * pAddress,
  IN socklen_t AddressLength
  );

/**
  Process the transmit completion

  This routine use ::EslSocketTxComplete to perform the transmit
  completion processing for data packets.

  This routine is called by the IPv4 network layer when a data
  transmit request completes.

  @param [in] Event     The normal transmit completion event

  @param [in] pIo       The address of an ::ESL_IO_MGMT structure

**/
VOID
EslIp4TxComplete (
  IN EFI_EVENT Event,
  IN ESL_IO_MGMT * pIo
  );

//------------------------------------------------------------------------------
// Tcp4 Routines
//------------------------------------------------------------------------------

/**
  Accept a network connection.

  This routine waits for a network connection to the socket and
  returns the remote network address to the caller if requested.

  This routine is called by ::EslSocketAccept to handle the TCPv4 protocol
  specific accept operations for SOCK_STREAM and SOCK_SEQPACKET sockets.
  See the \ref ConnectionManagement section.

  @param [in] pSocket   Address of an ::ESL_SOCKET structure.

  @param [in] pSockAddr       Address of a buffer to receive the remote
                              network address.

  @param [in, out] pSockAddrLength  Length in bytes of the address buffer.
                                    On output specifies the length of the
                                    remote network address.

  @retval EFI_SUCCESS   Remote address is available
  @retval Others        Remote address not available

 **/
EFI_STATUS
EslTcp4Accept (
  IN ESL_SOCKET * pSocket,
  IN struct sockaddr * pSockAddr,
  IN OUT socklen_t * pSockAddrLength
  );

/**
  Poll for completion of the connection attempt.

  This routine polls the ESL_SOCKET::bConnected flag to determine
  when the connection attempt is complete.

  This routine is called from ::EslSocketConnect to determine when
  the connection is complete.  The ESL_SOCKET::bConnected flag is
  set by ::EslTcp4ConnectComplete when the TCPv4 layer establishes
  a connection or runs out of local network adapters.  This routine
  gets the connection status from ESL_SOCKET::ConnectStatus.

  @param [in] pSocket   Address of an ::ESL_SOCKET structure.

  @retval EFI_SUCCESS   The connection was successfully established.
  @retval EFI_NOT_READY The connection is in progress, call this routine again.
  @retval Others        The connection attempt failed.

 **/
EFI_STATUS
EslTcp4ConnectPoll (
  IN ESL_SOCKET * pSocket
  );

/**
  Connect to a remote system via the network.

  This routine starts the connection processing for a SOCK_STREAM
  or SOCK_SEQPAKCET socket using the TCPv4 network layer.

  This routine is called by ::EslSocketConnect to initiate the TCPv4
  network specific connect operations.  The connection processing is
  initiated by this routine and finished by ::EslTcp4ConnectComplete.

  @param [in] pSocket         Address of an ::ESL_SOCKET structure.

  @param [in] pSockAddr       Network address of the remote system.
    
  @param [in] SockAddrLength  Length in bytes of the network address.
  
  @retval EFI_SUCCESS   The connection was successfully established.
  @retval EFI_NOT_READY The connection is in progress, call this routine again.
  @retval Others        The connection attempt failed.

 **/
EFI_STATUS
EslTcp4ConnectStart (
  IN ESL_SOCKET * pSocket,
  IN const struct sockaddr * pSockAddr,
  IN socklen_t SockAddrLength
  );

/**
  Get the local socket address.

  This routine returns the IPv4 address and TCP port number associated
  with the local socket.

  This routine is called by ::EslSocketGetLocalAddress to determine the
  network address for the SOCK_STREAM or SOCK_SEQPACKET socket.
  
  @param [in] pSocket             Address of an ::ESL_SOCKET structure.

  @param [out] pAddress           Network address to receive the local system address

  @param [in,out] pAddressLength  Length of the local network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
EFI_STATUS
EslTcp4GetLocalAddress (
  IN ESL_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Get the remote socket address.

  This routine returns the address of the remote connection point
  associated with the SOCK_STREAM or SOCK_SEQPACKET socket.

  This routine is called by ::EslSocketGetPeerAddress to detemine
  the TCPv4 address and por number associated with the network adapter.

  @param [in] pSocket             Address of an ::ESL_SOCKET structure.

  @param [out] pAddress           Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
EFI_STATUS
EslTcp4GetRemoteAddress (
  IN ESL_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Establish the known port to listen for network connections.

  This routine places the port into a state that enables connection
  attempts.

  This routine is called by ::EslSocketListen to handle the network
  specifics of the listen operation for SOCK_STREAM and SOCK_SEQPACKET
  sockets.  See the \ref ConnectionManagement section.

  @param [in] pSocket   Address of an ::ESL_SOCKET structure.

  @retval EFI_SUCCESS - Socket successfully created
  @retval Other - Failed to enable the socket for listen

**/
EFI_STATUS
EslTcp4Listen (
  IN ESL_SOCKET * pSocket
  );

/**
  Process the connection attempt

  A system has initiated a connection attempt with a socket in the
  listen state.  Attempt to complete the connection.

  The TCPv4 layer calls this routine when a connection is made to
  the socket in the listen state.  See the
  \ref ConnectionManagement">Connection Management</a> section.

  @param [in] Event     The listen completion event

  @param [in] pPort     Address of an ::ESL_PORT structure.

**/
VOID
EslTcp4ListenComplete (
  IN EFI_EVENT Event,
  IN ESL_PORT * pPort
  );

/**
  Initialize the network specific portions of an ::ESL_PORT structure.

  This routine initializes the network specific portions of an
  ::ESL_PORT structure for use by the socket.

  This support routine is called by ::EslSocketPortAllocate
  to connect the socket with the underlying network adapter
  running the TCPv4 protocol.

  @param [in] ppPort      Address of an ESL_PORT structure
  @param [in] pIpAddress  Buffer containing IP4 network address of the local host
  @param [in] PortNumber  Tcp4 port number
  @param [in] DebugFlags  Flags for debug messages

  @retval EFI_SUCCESS - Socket successfully created

 **/
EFI_STATUS
EslTcp4PortAllocate (
  IN ESL_PORT * pPort,
  IN CONST UINT8 * pIpAddress,
  IN UINT16 PortNumber,
  IN UINTN DebugFlags
  );

/**
  Close a TCP4 port.

  This routine releases the resources allocated by
  ::EslTcp4PortAllocate.

  This routine is called by:
  <ul>
    <li>::EslTcp4ConnectComplete - Connection failure and reducint the port list to a single port</li>
    <li>::EslTcp4PortAllocate - Port initialization failure</li>
    <li>::EslTcp4PortCloseRxDone - Last step of close processing</li>
  </ul>
  See the \ref Tcp4PortCloseStateMachine section.
  
  @param [in] pPort       Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS     The port is closed
  @retval other           Port close error

**/
EFI_STATUS
EslTcp4PortClose (
  IN ESL_PORT * pPort
  );

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
EslTcp4PortClosePacketFree (
  IN ESL_PACKET * pPacket,
  IN OUT size_t * pRxBytes
  );

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
EslTcp4PortCloseRxStop (
  IN ESL_PORT * pPort
  );

/**
  Receive data from a network connection.

  This routine attempts to return buffered data to the caller.  The
  data is removed from the urgent queue if the message flag MSG_OOB
  is specified, otherwise data is removed from the normal queue.
  See the \ref Tcp4ReceiveEngine section.

  This routine is called by ::EslSocketReceive to handle the network
  specific receive operation to support SOCK_STREAM and SOCK_SEQPACKET
  sockets.

  @param [in] pSocket         Address of an ::ESL_SOCKET structure.
  
  @param [in] Flags           Message control flags
  
  @param [in] BufferLength    Length of the the buffer
  
  @param [in] pBuffer         Address of a buffer to receive the data.
  
  @param [in] pDataLength     Number of received data bytes in the buffer.

  @param [out] pAddress       Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @retval EFI_SUCCESS - Socket data successfully received

 **/
EFI_STATUS
EslTcp4Receive (
  IN ESL_SOCKET * pSocket,
  IN INT32 Flags,
  IN size_t BufferLength,
  IN UINT8 * pBuffer,
  OUT size_t * pDataLength,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Cancel the receive operations

  This routine cancels the pending receive operations.
  See the \ref Tcp4ReceiveEngine section.

  This routine is called by ::EslSocketShutdown when the socket
  layer is being shutdown.

  @param [in] pSocket   Address of an ::ESL_SOCKET structure.
  
  @retval EFI_SUCCESS - The cancel was successful

 **/
EFI_STATUS
EslTcp4RxCancel (
  IN ESL_SOCKET * pSocket
  );

/**
  Process the receive completion

  This routine queues the data in FIFO order in either the urgent
  or normal data queues depending upon the type of data received.
  See the \ref Tcp4ReceiveEngine section.

  This routine is called by the TCPv4 driver when some data is
  received.
  
  Buffer the data that was just received.

  @param [in] Event     The receive completion event

  @param [in] pPort     Address of an ::ESL_PORT structure

**/
VOID
EslTcp4RxComplete (
  IN EFI_EVENT Event,
  IN ESL_PORT * pPort
  );

/**
  Start a receive operation

  This routine posts a receive buffer to the TCPv4 driver.
  See the \ref Tcp4ReceiveEngine section.

  This support routine is called by:
  <ul>
    <li>::EslTcp4ListenComplete to start the recevie engine for the new socket.</li>
    <li>::EslTcp4Receive to restart the receive engine to release flow control.</li>
    <li>::EslTcp4RxComplete to continue the operation of the receive engine if flow control is not being applied.</li>
  </ul>

  @param [in] pPort       Address of an ::ESL_PORT structure.

 **/
VOID
EslTcp4RxStart (
  IN ESL_PORT * pPort
  );

/**
  Determine if the socket is configured.

  This routine uses the flag ESL_SOCKET::bConfigured to determine
  if the network layer's configuration routine has been called.

  This routine is called by EslSocketIsConfigured to verify
  that the socket has been configured.

  @param [in] pSocket   Address of an ::ESL_SOCKET structure.
  
  @retval EFI_SUCCESS - The port is connected
  @retval EFI_NOT_STARTED - The port is not connected

 **/
 EFI_STATUS
 EslTcp4SocketIsConfigured (
  IN ESL_SOCKET * pSocket
  );

/**
  Buffer data for transmission over a network connection.

  This routine buffers data for the transmit engine in one of two
  queues, one for urgent (out-of-band) data and the other for normal
  data.  The urgent data is provided to TCP as soon as it is available,
  allowing the TCP layer to schedule transmission of the urgent data
  between packets of normal data.

  This routine is called by ::EslSocketTransmit to buffer
  data for transmission.  When the transmit engine has resources,
  this routine will start the transmission of the next buffer on
  the network connection.

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
EslTcp4TxBuffer (
  IN ESL_SOCKET * pSocket,
  IN int Flags,
  IN size_t BufferLength,
  IN CONST UINT8 * pBuffer,
  OUT size_t * pDataLength,
  IN const struct sockaddr * pAddress,
  IN socklen_t AddressLength
  );

/**
  Process the normal data transmit completion

  This routine use ::EslSocketTxComplete to perform the transmit
  completion processing for normal data.

  This routine is called by the TCPv4 network layer when a
  normal data transmit request completes.

  @param [in] Event     The normal transmit completion event

  @param [in] pIo       The ESL_IO_MGMT structure address

**/
VOID
EslTcp4TxComplete (
  IN EFI_EVENT Event,
  IN ESL_IO_MGMT * pIo
  );

/**
  Process the urgent data transmit completion

  This routine use ::EslSocketTxComplete to perform the transmit
  completion processing for urgent data.

  This routine is called by the TCPv4 network layer when a
  urgent data transmit request completes.

  @param [in] Event     The urgent transmit completion event

  @param [in] pIo       The ESL_IO_MGMT structure address

**/
VOID
EslTcp4TxOobComplete (
  IN EFI_EVENT Event,
  IN ESL_IO_MGMT * pIo
  );

//------------------------------------------------------------------------------
// Udp4 Routines
//------------------------------------------------------------------------------

/**
  Set the default remote system address.

  This routine sets the default remote address for a SOCK_DGRAM
  socket using the UDPv4 network layer.

  This routine is called by ::EslSocketConnect to initiate the UDPv4
  network specific connect operations.  The connection processing is
  limited to setting the default remote network address.

  @param [in] pSocket         Address of an ::ESL_SOCKET structure.

  @param [in] pSockAddr       Network address of the remote system.
    
  @param [in] SockAddrLength  Length in bytes of the network address.
  
  @retval EFI_SUCCESS   The connection was successfully established.
  @retval EFI_NOT_READY The connection is in progress, call this routine again.
  @retval Others        The connection attempt failed.

 **/
EFI_STATUS
EslUdp4Connect (
  IN ESL_SOCKET * pSocket,
  IN const struct sockaddr * pSockAddr,
  IN socklen_t SockAddrLength
  );

/**
  Get the local socket address

  This routine returns the IPv4 address and UDP port number associated
  with the local socket.

  This routine is called by ::EslSocketGetLocalAddress to determine the
  network address for the SOCK_DGRAM socket.

  @param [in] pSocket             Address of an ::ESL_SOCKET structure.

  @param [out] pAddress           Network address to receive the local system address

  @param [in,out] pAddressLength  Length of the local network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
EFI_STATUS
EslUdp4GetLocalAddress (
  IN ESL_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Get the remote socket address

  This routine returns the address of the remote connection point
  associated with the SOCK_DGRAM socket.

  This routine is called by ::EslSocketGetPeerAddress to detemine
  the UDPv4 address and port number associated with the network adapter.

  @param [in] pSocket             Address of an ::ESL_SOCKET structure.

  @param [out] pAddress           Network address to receive the remote system address

  @param [in,out] pAddressLength  Length of the remote network address structure

  @retval EFI_SUCCESS - Address available
  @retval Other - Failed to get the address

**/
EFI_STATUS
EslUdp4GetRemoteAddress (
  IN ESL_SOCKET * pSocket,
  OUT struct sockaddr * pAddress,
  IN OUT socklen_t * pAddressLength
  );

/**
  Initialize the network specific portions of an ::ESL_PORT structure.

  This routine initializes the network specific portions of an
  ::ESL_PORT structure for use by the socket.

  This support routine is called by ::EslSocketPortAllocate
  to connect the socket with the underlying network adapter
  running the UDPv4 protocol.

  @param [in] ppPort      Address of an ESL_PORT structure
  @param [in] pIpAddress  Buffer containing IP4 network address of the local host
  @param [in] PortNumber  Udp4 port number
  @param [in] DebugFlags  Flags for debug messages

  @retval EFI_SUCCESS - Socket successfully created

 **/
EFI_STATUS
EslUdp4PortAllocate (
  IN ESL_PORT * pPort,
  IN CONST UINT8 * pIpAddress,
  IN UINT16 PortNumber,
  IN UINTN DebugFlags
  );

/**
  Close a UDP4 port.

  This routine releases the resources allocated by
  ::EslUdp4PortAllocate.

  This routine is called by:
  <ul>
    <li>::EslUdp4PortAllocate - Port initialization failure</li>
    <li>::EslUdp4PortCloseRxDone - Last step of close processing</li>
  </ul>
  See the \ref Udp4PortCloseStateMachine section.

  @param [in] pPort       Address of an ::ESL_PORT structure.

  @retval EFI_SUCCESS     The port is closed
  @retval other           Port close error

**/
EFI_STATUS
EslUdp4PortClose (
  IN ESL_PORT * pPort
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

//------------------------------------------------------------------------------

#endif  //  _SOCKET_H_
