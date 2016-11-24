/*
 * OS_TIMER_cfg.h
 *
 *  Created on: Jan 29, 2015
 *      Author: MELSOBKY
 * 	File description:
 *      Module Configuration File
 *
 */



/**************************************************
 * configuration types
 *************************************************/
#ifndef OS_TIMER_CFG_H_
#define OS_TIMER_CFG_H_

#include "../Utilities/std_types.h"
#include "../MCAL/EF_TIVA_Timer.h"
/**************************************************
 * predifine variables
 **************************************************/
#define MAX_NUMBER_OF_TASKS    5
#define MAX_NUMBER_OF_TIMERS   10


#define SCALE_TIMER_ID              0
#define GET_ALL_FRAME_TIMER_ID      1
#define GET_OPER_VALUES_TIMER_ID    2
#define ERROR_PHOTO_TIMER_ID        3
#define STABLE_SCALE_TIMER_ID       4
#define SAVING_TOL_TIMER_ID         5
#define UART_TIMER_ID               6
#define SPI_TIMER_ID                7
#define NEW_OPERATION_TIMER_ID      8


#define UART_WAIT_TIME              100
#define SPI_TIMEOUT                 200
#define GET_ALL_FRAME_TIMEOUT       200
#define DISPLAY_ERROR_TIMEOUT       1000
#define GET_OPER_VALUES_TIMEOUT     4000
#define NEW_OPERATION_TIMEOUT       1000

/**************************************************
 * configuration types
 *************************************************/
 
typedef struct timer_struct
{
  volatile U32_t timeOut ;
  volatile U32_t counter ;
  volatile BOOLEAN isTimedout;
  volatile BOOLEAN isEnables;
}OS_TIMER;

#endif
