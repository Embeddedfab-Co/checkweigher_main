/***************************************************************
 *  Source File: EF_TIVA_Timer.c
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

#include "EF_TIVA_Timer.h"
#include "EF_TIVA_EEPROM.h"
#include "../Services/EF_UserInterface_cfg.h"
extern volatile  U8_t gU8StopOperations_Flag ;
volatile U32_t gCounterEvery_3Hours  = 0;
U32_t gU32CounterExpired_3Months = 0;
U32_t FirstReleaseFlag = 0;

// Global variables

static void (*voidFuncPtr)(void);


/****************************************************************************
* Function    : void_ucPORT_init()
*
* DESCRIPTION : this function initializes the timer module with the configuration data
*
* PARAMETERS  : None
*
* Return Value: None
******************************************************************************/
void EF_void_Timer_PORTinit(void)
{
	//give clock to it's peripheral 
	SysCtlPeripheralEnable(SYS_CLOCK_TIMER);
	SysCtlDelay(5);
	//Timer	16 bit
	TimerConfigure(TIMER_BASE_ADD, TIMER_CONFIGURATION);
	/* pre-scaler */
	TimerPrescaleSet (TIMER_BASE_ADD, TIMER_TYPE , TIMER_PRESCALE - 1 );
	//TIMERCYCLES : loaded value
	TimerLoadSet(TIMER_BASE_ADD, TIMER_TYPE, TIMERCYCLES);
	/*NOTE: don't forget putting ISR_Function in the StartupCode */
	TimerIntEnable(TIMER_BASE_ADD, TIMER_INT_FLAG_TYPE );
	IntEnable(TIMER_INT_TYPE);
	//Global interrupt
	 IntMasterEnable();
	 // Enable the timer.
	 TimerEnable(TIMER_BASE_ADD, TIMER_TYPE);

	 /* if 3Month Peroid to Close Operation Task Enabled, Check here if 3Month Come --> Enable Flag to close Operation Task */
#ifdef TRAILS_3_MONTHS
	 /* get gCounterExpired_3Months from EEprom */
	 EF_BOOLEAN_EEPROM_ReadNBytes (&gU32CounterExpired_3Months  , TIMER_EXPIRED_3Months_EEPROM_ADD , 4);

	 if (gU32CounterExpired_3Months >= TIMER_90DAY_PER_3HOURS)
	 {
	     gU8StopOperations_Flag = TRUE;
	 }

#endif
}

/****************************************************************************
* Function    : void_TIMER_setCallBack
*
* DESCRIPTION : this call back function in the timer module.
*				Attach a user function to a timer interrupt
*
* PARAMETERS  : void (*userFunc)(void)  : pointer to the call back function
*
* Return Value: None
*
*  NOTE		  : "Don't forget putting Timer_ISR function in the startup code"
******************************************************************************/
void EF_void_Timer_SetISRCallBack( void (*userFunc)(void) )
{
		// the supplied user's function
		voidFuncPtr = userFunc;
}

/****************************************************************************
* Function    : void_TIMER_removeCallBack()
*
* DESCRIPTION : Detach a user function from a timer interrupt
*
* PARAMETERS  : None
*
* Return Value: None
******************************************************************************/
void EF_void_Timer_RemoveTimerISRCallBack(void)
{
		// set the interrupt function to run nothing
		voidFuncPtr = NULL;

}



/****************************************************************************
* Function    : Timer_W0_ISR
*
* DESCRIPTION : Interrupt handler for  overflow interrup
*
* PARAMETERS  : None
*
* Return Value: None
*
* NOTE		  : "Don't forget putting this function in the startup code"
******************************************************************************/


 void Timer_W0_ISR(void )
{
/* if 3Month Peroid to Close Operation Task Enabled, Check here if 3Month Come --> Enable Flag to close Operation Task
 * else Counter Every 3 Hours to Get the 3Month Peroid */
#ifdef TRAILS_3_MONTHS
     if (gU8StopOperations_Flag == FALSE)
     {
         if (gCounterEvery_3Hours >= TIMER_EXPIRED_3_HOURS)
         {
             /* Here 3hours peroid passed , check if 3Months passed ? */
             EF_BOOLEAN_EEPROM_ReadNBytes (&gU32CounterExpired_3Months  , TIMER_EXPIRED_3Months_EEPROM_ADD , 4);

             if (gU32CounterExpired_3Months >= TIMER_90DAY_PER_3HOURS)
             {
                 gU8StopOperations_Flag = TRUE;
             }
             else
             {
                 /* Every 3Hours increase Counter to get the 3Month Peroid */
                 gCounterEvery_3Hours = 0;
                 gU32CounterExpired_3Months++ ;
                 EF_BOOLEAN_EEPROM_WriteNBytes(&gU32CounterExpired_3Months  , TIMER_EXPIRED_3Months_EEPROM_ADD , 4);
             }
         }
         else
         {
             /* Count Variable to Reaching 3Hours Passed */
             gCounterEvery_3Hours ++;
         }
     }
#endif

	// Clear the timer interrupt.
	TimerIntClear(TIMER_BASE_ADD, TIMER_INT_FLAG_TYPE);
	 // if a user function is defined, execute it
	if(voidFuncPtr != NULL)
		voidFuncPtr();	
}


