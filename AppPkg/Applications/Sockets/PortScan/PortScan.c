/** @file
  Port scan application

  Copyright (c) 2011, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PortScan.h"

UINT8 mRxBuffer[65536];
UINT8 mTxBuffer[65536];

#define FLG_FIN       0x01
#define FLG_SYN       0x02
#define FLG_RST       0x04
#define FLG_PSH       0x08
#define FLG_ACK       0x10
#define FLG_URG       0x20

/**
  Determine which TCP ports are open on a remote system

  @param [in] ArgC      Argument count
  @param [in] ArgV      Argument value array

  @retval 0             Successfully operation
 **/
int
PortScan (
  IN int ArgC,
  IN char **ArgV
  )
{
  ssize_t BytesReceived;
  ssize_t BytesTransmitted;
  UINT32 Checksum;
  UINT16 Identification;
  struct sockaddr_in LocalAddress;
  UINT32 LocalInput[4];
  int MaxPortNumber;
  UINT8 * pData;
  UINT16 * pEnd;
  UINT8 * pIpHeader;
  int PortNumber;
  UINT8 * pRxIpHeader;
  UINT8 * pRxTcpHeader;
  UINT8 * pTcpHeader;
  UINT16 * pWord;
  struct sockaddr_in RemoteAddress;
  socklen_t RemoteAddressLength;
  UINT64 ResponseTimeDelay;
  int RetVal;
  SOCKET s;
  struct servent * pServEnt;
  UINT16 SourcePort;
  EFI_STATUS Status;
  struct sockaddr_in TargetAddress;
  UINT32 TargetInput[4];
  UINT32 TcpChecksum;
  EFI_EVENT Timer;
  int TxBytes;
  int Value;

  //
  //  Assume success
  //
  RetVal = 0;

  //
  //  Set the ID value
  //
  Identification = 0xdead;
  SourcePort = 54321;

  //
  //  Validate the arguments
  //
  if (( 3 > ArgC )
    || ( 4 != sscanf ( ArgV[1],
                       "%d.%d.%d.%d",
                       &LocalInput[0],
                       &LocalInput[1],
                       &LocalInput[2],
                       &LocalInput[3]))
    || ( 224 < LocalInput[0])
    || ( 255 < LocalInput[1])
    || ( 255 < LocalInput[2])
    || ( 255 < LocalInput[3])
    || (( 0 == LocalInput[0])
        && ( 0 == LocalInput[1])
        && ( 0 == LocalInput[2])
        && ( 0 == LocalInput[3]))
    || ( 4 != sscanf ( ArgV[2],
                       "%d.%d.%d.%d",
                       &TargetInput[0],
                       &TargetInput[1],
                       &TargetInput[2],
                       &TargetInput[3]))
    || ( 224 < TargetInput[0])
    || ( 255 < TargetInput[1])
    || ( 255 < TargetInput[2])
    || ( 255 < TargetInput[3])
    || (( 0 == TargetInput[0])
        && ( 0 == TargetInput[1])
        && ( 0 == TargetInput[2])
        && ( 0 == TargetInput[3]))) {
    printf ( "%s  <local IP address>  <target IP address>\r\n", ArgV[0]);
    RetVal = EINVAL;
  }
  else {
    //
    //  Set the local address
    //
    memset ( &LocalAddress, 0, sizeof ( LocalAddress ));
    SIN_FAMILY ( LocalAddress ) = AF_INET;
    SIN_LEN ( LocalAddress ) = sizeof ( LocalAddress );
    SIN_ADDR ( LocalAddress ) = LocalInput[0]
                              | ( LocalInput[1] << 8 )
                              | ( LocalInput[2] << 16 )
                              | ( LocalInput[3] << 24 );

    //
    //  Set the target address
    //
    memset ( &TargetAddress, 0, sizeof ( TargetAddress ));
    SIN_FAMILY ( TargetAddress ) = AF_INET;
    SIN_LEN ( TargetAddress ) = sizeof ( TargetAddress );
    SIN_ADDR ( TargetAddress ) = TargetInput[0]
                               | ( TargetInput[1] << 8 )
                               | ( TargetInput[2] << 16 )
                               | ( TargetInput[3] << 24 );

    //
    //  Determine the maximum port number
    //
    MaxPortNumber = 1024;

    //
    //  Set the receive time delay
    //
    ResponseTimeDelay = PORT_RESPONSE_MSEC;
    ResponseTimeDelay *= 1000 * 10;

    //
    //  Create the timer event
    //
    Status = OsTimerCreate ( &Timer );
    if ( !EFI_ERROR ( Status )) {
      //
      //  Open the socket
      //
      s = socket ( AF_INET, SOCK_RAW, IPPROTO_TCP );
      if ( -1 != s ) {
        //
        //  Set socket mode to include the header
        //
        Value = 1;
        RetVal = setsockopt ( s,
                              IPPROTO_IP,
                              IP_HDRINCL,
                              (UINT8 *)&Value,
                              sizeof ( Value ));
        if ( -1 != RetVal ) {
          //
          //  Bind to the local adapter
          //
          RetVal = bind ( s,
                          (struct sockaddr *)&LocalAddress,
                          sizeof ( LocalAddress ));
          if ( -1 != RetVal ) {
            //
            //  Attempt to connect to the remote system
            //
            RetVal = connect ( s,
                               (struct sockaddr *)&TargetAddress,
                               sizeof ( TargetAddress ));
            if ( -1 != RetVal ) {
              //
              //  Build the IP header
              //  See http://www.ietf.org/rfc/rfc791.txt
              //
              pData = &mTxBuffer[0];
              pIpHeader = pData;
              *pData++ = 0x45;        //  IPv: 4, Header length: 5 * 32-bits
              *pData++ = 0;
              pData += 2;             //  Skip over packet length
              *pData++ = (UINT8)Identification; //  16-bit ID value
              *pData++ = (UINT8)( Identification >> 8 );
              *pData++ = 0x40;        //  Don't fragment
              *pData++ = 0;           //  Fragment offset
              *pData++ = 128;         //  TTL
              *pData++ = IPPROTO_TCP; //  Protocol
              *pData++ = 0;           //  16-bit Checksum
              *pData++ = 0;

              *pData++ = (UINT8)SIN_ADDR ( LocalAddress );  //  Source address
              *pData++ = (UINT8)( SIN_ADDR ( LocalAddress ) >> 8 );
              *pData++ = (UINT8)( SIN_ADDR ( LocalAddress ) >> 16 );
              *pData++ = (UINT8)( SIN_ADDR ( LocalAddress ) >> 24 );

              *pData++ = (UINT8)SIN_ADDR ( TargetAddress ); //  Destination address
              *pData++ = (UINT8)( SIN_ADDR ( TargetAddress ) >> 8 );
              *pData++ = (UINT8)( SIN_ADDR ( TargetAddress ) >> 16 );
              *pData++ = (UINT8)( SIN_ADDR ( TargetAddress ) >> 24 );

              //
              //  Build the TCP header
              //  See http://www.ietf.org/rfc/rfc793.txt
              //
              pTcpHeader = pData;
              *pData++ = (UINT8)( SourcePort >> 8 );  //  16-bit Source port
              *pData++ = (UINT8)SourcePort;
              *pData++ = 0;         //  16-bit Destination port
              *pData++ = 0;
              *pData++ = 0xdb;      //  32-bit Sequence number
              *pData++ = 0xb1;
              *pData++ = 0x62;
              *pData++ = 0x54;
              *pData++ = 0;         //  32-bit Acknowledgement number
              *pData++ = 0;
              *pData++ = 0;
              *pData++ = 0;
              *pData++ = 0;         //  Header length in 32-bit words / reserved
              *pData++ = FLG_SYN;   //  Flags
              *pData++ = 0x20;      //  16-bit Window size: 8192
              *pData++ = 0;
              *pData++ = 0;         //  16-bit Checksum
              *pData++ = 0;
              *pData++ = 0;         //  16-bit Urgent pointer
              *pData++ = 0;

              //
              //  TCP Options
              //
/*
              *pData++ = 2;     //  Max segment size 1460
              *pData++ = 4;
              *pData++ = 5;
              *pData++ = 0xb4;

              *pData++ = 1;     //  NOP

              *pData++ = 3;     //  Windows scale 2
              *pData++ = 3;
              *pData++ = 2;

              *pData++ = 1;     //  NOP
              *pData++ = 1;     //  NOP

              *pData++ = 4;     //  TCP SACK Permitted
              *pData++ = 2;
*/

              //
              //  Set the header length in 32-bit words
              //
              pTcpHeader[12] = (UINT8)((( pData - pTcpHeader ) >> 2 ) << 4 );

              //
              //  Set the message length
              //
              TxBytes = (int)( pData - &mTxBuffer[0]);
              pIpHeader[2] = (UINT8)( TxBytes >> 8 );
              pIpHeader[3] = (UINT8)TxBytes;

              //
              //  Computer the IP header checksum
              //
              Checksum = 0;
              pWord = (UINT16 *)&mTxBuffer[0];
              pEnd = (UINT16 *)pTcpHeader;
              while ( pEnd > pWord ) {
                Checksum += htons ( *pWord++ );
              }
              while ( 0xffff < Checksum ) {
                Checksum = ( Checksum >> 16 ) + ( Checksum & 0xffff );
              }
              Checksum = ~Checksum;

              //
              //  Insert the IP header checksum
              //
              pIpHeader[10] = (UINT8)( Checksum >> 8 );
              pIpHeader[11] = (UINT8)Checksum;

              //
              //  Compute the TCP header checksum
              //
              TcpChecksum = IPPROTO_TCP;
              TcpChecksum += ((UINT16)( pData - pTcpHeader ));
              pWord = (UINT16 *)&pTcpHeader[-8];
              pEnd = (UINT16 *)pData;
              while ( pEnd > pWord ) {
                TcpChecksum += htons ( *pWord++ );
              }

              //
              //  Loop through the list of ports
              //
              for ( PortNumber = 1; MaxPortNumber > PortNumber; PortNumber++ ) {
                //
                //  Set the port number
                //
                pTcpHeader[2] = (UINT8)( PortNumber >> 8 );
                pTcpHeader[3] = (UINT8)PortNumber;

                //
                //  Finish computing the TCP header checksum
                //
                Checksum = TcpChecksum + htons( *(UINT16 *)&pTcpHeader[2]);
                while ( 0xffff < Checksum ) {
                  Checksum = ( Checksum >> 16 ) + ( Checksum & 0xffff );
                }
                Checksum = ~Checksum;

                //
                //  Insert the TCP header checksum
                //
                pTcpHeader[16] = (UINT8)( Checksum >> 8 );
                pTcpHeader[17] = (UINT8)Checksum;

                //
                //  Send the TCP packet - SYN RST
                //
                BytesTransmitted = sendto ( s,
                                            &mTxBuffer[0],
                                            TxBytes,
                                            0,
                                            (struct sockaddr *)&TargetAddress,
                                            sizeof ( TargetAddress ));
                if ( 0 < BytesTransmitted ) {
                  //
                  //  Start the timer
                  //
                  Status = OsTimerStart ( Timer, ResponseTimeDelay );
                  if ( !EFI_ERROR ( Status )) {
                    do {
                      RemoteAddressLength = sizeof ( RemoteAddress );
                      BytesReceived = recvfrom ( s,
                                                 &mRxBuffer[0],
                                                 sizeof ( mRxBuffer ),
                                                 0,
                                                 (struct sockaddr *)&RemoteAddress,
                                                 &RemoteAddressLength );
                      if ( 0 < BytesReceived ) {
                        //
                        //  Packet received
                        //  Validate the response (SYN ACK)
                        //
                        pRxIpHeader = &mRxBuffer[0];
                        pRxTcpHeader = &pRxIpHeader [( pRxIpHeader [0] & 0xf ) << 2 ];
                        if (( pIpHeader[12] == pRxIpHeader[16])   //  Our address
                          && ( pIpHeader[13] == pRxIpHeader[17])
                          && ( pIpHeader[14] == pRxIpHeader[18])
                          && ( pIpHeader[15] == pRxIpHeader[19])
                          && ( pIpHeader[16] == pRxIpHeader[12])  //  Target address
                          && ( pIpHeader[17] == pRxIpHeader[13])
                          && ( pIpHeader[18] == pRxIpHeader[14])
                          && ( pIpHeader[19] == pRxIpHeader[15])
                          && ( IPPROTO_TCP == pRxIpHeader[9])     //  TCP protocol
                          && ( pTcpHeader[0] == pRxTcpHeader[2])  //  Source port
                          && ( pTcpHeader[1] == pRxTcpHeader[3])
                          && ( pTcpHeader[2] == pRxTcpHeader[0])  //  Destination port
                          && ( pTcpHeader[3] == pRxTcpHeader[1])
                          && (( FLG_ACK | FLG_SYN ) == pRxTcpHeader[13])) {
                          //
                          //  Cancel the timeout
                          //
                          Status = OsTimerCancel ( Timer );
                          if ( EFI_ERROR ( Status )) {
                            RetVal = (int)Status;
                            printf ( "ERROR - OsTimerCancel error, Status: %d\r\n", Status );
                            break;
                          }

                          //
                          //  Display the port number and name
                          //
                          pServEnt = getservbyport ( PortNumber, "TCP" );
                          printf ( "%5d", PortNumber );
                          if ( NULL != pServEnt ) {
                            printf ( " - %s", pServEnt->s_name );
                          }
                          printf ( "\r\n" );
                        }
                        else {
                          //
                          //  Invalid response
                          //
                          BytesReceived = -1;
                        }
                      }
                      else {
                        //
                        //  No data received
                        //  Check for response timeout
                        //
                        Status = OsTimerExpired ( Timer );
                        if ( !EFI_ERROR ( Status )) {
                          //
                          //  Receive timeout
                          //
                          break;
                        }
                        Status = 0;
                      }
                    } while ( 0 >= BytesReceived );
                    if ( EFI_ERROR ( Status )) {
                      break;
                    }
                  }
                  else {
                    //
                    //  SetTimer error
                    //
                    RetVal = (int)Status;
                    printf ( "ERROR - OsTimerStart error, Status: %d\r\n", Status );
                    break;
                  }
                }
                else {
                  RetVal = GET_ERRNO;
                  printf ( "ERROR - sendto error, errno: %d\r\n", RetVal );
                  break;
                }
              }
            }
            else {
              //
              //  connect error
              //
              RetVal = GET_ERRNO;
              printf ( "ERROR - connect error, errno: %d\r\n", RetVal );
            }
          }
          else {
            //
            //  bind error
            //
            RetVal = GET_ERRNO;
            printf ( "ERROR - bind error, errno: %d\r\n", RetVal );
          }
        }
        else {
          //
          //  setsockopt error
          //
          RetVal = GET_ERRNO;
          printf ( "ERROR - setsockopt error, errno: %d\r\n", RetVal );
        }

        //
        //  Close the socket
        //
        CLOSE_SOCKET ( s );
        printf ( "Socket closed\r\n" );
      }
      else {
        //
        //  Socket creation error
        //
        RetVal = GET_ERRNO;
        printf ( "ERROR - socket error, errno: %d\r\n", RetVal );
      }

      //
      //  Done with the event
      //
      Status = OsTimerClose ( Timer );
      if ( EFI_ERROR ( Status )) {
        printf ( "ERROR - CloseEvent error, Status: %d\r\n", Status );
      }
    }
    else {
      //
      //  Create event error
      //
      RetVal = (int)Status;
      printf ( "ERROR - OsTimerCreate error, Status: %d\r\n", Status );
    }
  }

  //
  //  Return the operation status
  //
  return RetVal;
}
