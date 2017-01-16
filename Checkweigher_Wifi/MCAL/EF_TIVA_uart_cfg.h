/***************************************************************
 *  Source File: EF_TIVA_uart_cfg.h
 *
 *  Description: UART driver for TivaC
 *
 *  History:     Version 1.0  - INIT Version
 *  Date   :     25/08/2014
 *  -------------------------------------------------------
 *  Author :     EmbeddedFab.
 *
 *  Copyright (C) <2014>  <EmbeddedFab>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>

 **************************************************************/

#ifndef UART_CFG_H
#define UART_CFG_H
 #define TM4C123G
 //#define ATMEGA
 
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "inc/hw_gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "inc/hw_uart.h"
#include "../Utilities/EF_SpechialTIMER.h"
#include "../Utilities/std_types.h"

/**************************************************
 * Predefine variables
 **************************************************/
 #ifdef ATMEGA
#define NO_PARITY 0
#define EVEN_PARITY 2
#define ODD_PARITY	3
#define ONE_STOP_BIT 0
#define TWO_STOP_BITS 1
#endif

#ifdef TM4C123G
#define NO_PARITY  	 	   UART_CONFIG_PAR_NONE
#define EVEN_PARITY		   UART_CONFIG_PAR_EVEN
#define ODD_PARITY		   UART_CONFIG_PAR_ODD
#define ONE_PARITY		   UART_CONFIG_PAR_ONE
#define ZERO_PARITY		   UART_CONFIG_PAR_ZERO
#define ONE_STOP_BIT  	   UART_CONFIG_STOP_ONE
#define TWO_STOP_BITS	   UART_CONFIG_STOP_TWO
#define NUMBER_OF_BITS_8   UART_CONFIG_WLEN_8
#define NUMBER_OF_BITS_7   UART_CONFIG_WLEN_7
#define NUMBER_OF_BITS_6   UART_CONFIG_WLEN_6
#define NUMBER_OF_BITS_5   UART_CONFIG_WLEN_5
#endif
/**************************************************
 * configuration types
 *************************************************/

#ifdef TM4C123G
/*NOTE : UART0 is reserved for programming/debugging (limited to 3.6 v as max voltage)
*		 UART6 is reserved for USB device connector
*		 UART1 is limited to 3.6 v as max voltage.
*/
/*
*    0- UART1 (Tx to B1  ,  Rx to B0) , is limited to 3.6v
*    1- UART2 (Tx to D7  ,  Rx to D6) , with special configuration
*    2- UART3 (Tx to C7  ,  Rx to C6)
*    3- UART4 (Tx to C5  ,  Rx to C4)
*    4- UART5 (Tx to E5  ,  Rx to E4)
*    5- UART7 (Tx to E1  ,  Rx to E0)
*/
typedef enum
{
	UART1 = 0, /*UART1 -(only of these available UARTs)- is limited to 3.6 v as max voltage.*/
	UART2,
	UART3,
	UART4,
	UART5,
	UART7
}TenumUART_Ports;
#endif

typedef struct
{
	#ifdef TM4C123G
	TenumUART_Ports UART_Number;
	#endif
	U32_t baudrate;
	U8_t numberOfDataBits;
	U8_t stopBits;
	U8_t parity;
	BOOLEAN RXInterruptEnable;
	BOOLEAN TXInterruptEnable;
	BOOLEAN ReceiverEnable;
	BOOLEAN TransmitterEnable;

}UART_cfg_str;



#endif
