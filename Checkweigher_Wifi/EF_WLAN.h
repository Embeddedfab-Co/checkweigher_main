/*
 * EF_WLAN.h
 *
 *  Created on: 1 Nov 2016
 *      Author: Mohamed Samy
 *
 *  Description: Header file of Wireless LAN (WLAN) library. Contains its macros definitions,
 *               Global variables declaration, and shared functions declarations.
 */

#ifndef EF_WLAN_H_
#define EF_WLAN_H_

#include "simplelink.h"
#include "sl_common.h"              /* Configures SSID name, Type and Password*/
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/flash.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/rom_map.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
//#include "drivers/pinout.h"
//#include "io.h"

#include "driverlib/fpu.h"



#define APPLICATION_VERSION "1.2.0"

//*****************************************************************************
//
// Define WLAN IP properties.
//
//*****************************************************************************
#define WLAN_IP_ADDRESS     0xC0A801C8
#define WLAN_SUBNET_MASK    0xFFFFFF00
#define WLAN_GATEWAY        0xC0A80101
#define WLAN_DNS_SERVER     0xC0A80101

//*****************************************************************************
//
// Define to open print in debugging mode and close it in production mode.
//
//*****************************************************************************
#define PRINT_VERBOSE
//*****************************************************************************
//
// Defines for Buffers Sizes.
//
//*****************************************************************************
#define WLAN_RX_STRING_SIZE  28
//*****************************************************************************
//
// Defines for Sockets properties. The hardware limits the number of client to 7
//
//*****************************************************************************
#define MAX_SOCKET_NUMBER     8
//*****************************************************************************
//
// Defines for Connection States.
//
//*****************************************************************************
#define DISCONNECTED          0
#define CONNECTED             1


#define SL_STOP_TIMEOUT       0xFF

/* IP addressed of server side socket. Should be in long format,
 * E.g: 0xc0a8010a == 192.168.1.10 */
#define IP_ADDR               0xc0a80832      /* Static Server Tablet address 192.168.1.50 */
#define PORT_NUM              5000            /* Port number to be used */

#define BUF_SIZE              1000
#define NO_OF_PACKETS         2

/* Application specific status/error codes */
typedef enum{
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,                         /* Choosing this number to avoid overlap w/ host-driver's error codes */
    TCP_SEND_ERROR             = DEVICE_NOT_IN_STATION_MODE - 1,
    TCP_RECV_ERROR             = TCP_SEND_ERROR -1,
    STATUS_CODE_MAX            = -0xBB8
}e_AppStatusCodes;

/*
 * Wifi module GLOBAL VARIABLES -- Start
 */
typedef union
{
    _u8  BsdBuf[BUF_SIZE];
    _u32 demobuf[BUF_SIZE/4];
}  uBuffer;


extern uBuffer  uBuf;
extern _u8      BsdBuf_1[BUF_SIZE];
extern _u8      g_Status;
extern uint32_t g_ui32SysClock;                 /* System clock shared between the WiFi part and Ethernet part*/
extern _i16     SockID_sr_1;                    /* The main socket ID*/
extern _i16     SockID_sr_2[MAX_SOCKET_NUMBER]; /* Array of clients sockets (server tablet and clients tablets) */
extern _u8      gu8APConnectionStatus;           /* Access point connection status */
/*
 * GLOBAL VARIABLES -- End
 */

_i32 configureSimpleLinkToDefaultState();
_i32 establishConnectionWithAP();
_i32 initializeAppVariables();
_i32 BsdTcpClient(_u16 Port);
_i32 BsdTcpServer(_u16 Port);
void displayBanner();
_i8 EF_WLAN_Init();
void EF_WLAN_Transmit(_i16 s16SocketNumber, const void *pDataBuffer, _i16 s16BufferLength);
int16_t EF_WLAN_Receive(_i16 s16SocketNumber, void *pDataBuffer, _i16 s16BufferLength);
_i16 EF_WLAN_Server_Accept_Connection(_i16 s16SocketNumber, SlSockAddr_t *pIP_Address_Accepted, SlSocklen_t *pIpAddressLength);
void EF_WLAN_Close_Connection(_i16 s16SocketNumber);


#endif /* EF_WLAN_H_ */
