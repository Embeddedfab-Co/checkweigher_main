/***************************************************************
 *  Source File: EF_TIVA_Timer_cfg.h
 *
 *  Description: Timer driver for TivaC
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

#ifndef TIMER_CFG_H
#define TIMER_CFG_H

#include <stdint.h>
#include <stdbool.h>
//#include "inc/tm4c123gh6pm.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "../Utilities/std_types.h"

#define SYS_CLOCK_TIMER  		SYSCTL_PERIPH_WTIMER0
#define TIMER_BASE_ADD 			WTIMER0_BASE
/* if timer 16 bit : TIMER_CONFIGURATION (if periodic): TIMER_CFG_A_PERIODIC|TIMER_CFG_SPLIT_PAIR
*  it may be timer 32 bit or wide timer 32 or 64 bit , with anther configuration*/
#define TIMER_CONFIGURATION 	TIMER_CFG_A_PERIODIC|TIMER_CFG_SPLIT_PAIR
//TIMER_TYPE can be : TIMER_A, TIMER_B, or TIMER_BOTH.
#define TIMER_TYPE 				TIMER_A
/* the pre-scale = the number you want to divide by  , but NOTE:
*if timer 16 bit , the max number to pre-scale = 256   = (2^(8))
*if timer 32 bit , the max number to pre-scale = 65536 = (2^(16))
*/
#define TIMER_PRESCALE			1024	
#define SYSTEMTICKTIME			1000  //1ms

#define CYCLES_PER_US 			((SysCtlClockGet()/ TIMER_PRESCALE )/1000000 ) 	// system tick is 1 micro
#define CYCLES_PER_5_MLI_S 		((SysCtlClockGet()/ TIMER_PRESCALE )/200) 	// system tick is 5 ms
#define CYCLES_PER_10_MLI_S 	((SysCtlClockGet()/ TIMER_PRESCALE )/100) 	// system tick is 10 ms
#define CYCLES_PER_20_MLI_S		((SysCtlClockGet()/ TIMER_PRESCALE )/50) 	// system tick is 20 ms
#define CYCLES_PER_V_MLI_S  	((SysCtlClockGet()/ TIMER_PRESCALE )/SYSTEMTICKTIME) 	// system tick	

#define TIMERCYCLES				CYCLES_PER_V_MLI_S
/*INTERRUPT_TYPE : may be time out , match , event ,... */
#define TIMER_INT_FLAG_TYPE 	TIMER_TIMA_TIMEOUT
#define TIMER_INT_TYPE			INT_WTIMER0A


#endif

