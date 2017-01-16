
#include "EF_SpechialTIMER.h"
/**************************************************
 * 	global variables
 *************************************************/
static OS_TIMER timers_list[MAX_NUMBER_OF_TIMERS];

/****************************************************************************
* Function    : void_SCHEDULER_timerISRCallback
*
* DESCRIPTION : Function to set the systemTik Flag, "Called in the TIMER ISR"
*
* PARAMETERS  : None
*
* Return Value: None
*
*  NOTE		  : "Don't forget putting Timer_ISR function in the startup code"
******************************************************************************/
static void void_OS_timerISRCallback(void)
{
	//count the enabled timer and if it reached the timeOut ,counter=0 and set timeOut Flag
	EF_void_TimersUpdate();
}

/****************************************************************************
* Function    : void_timerDelete
*
* DESCRIPTION : make all timer_list_parameters = 0
*
* PARAMETERS  : timerNumber
*
* Return Value: None
*
******************************************************************************/
void EF_void_TimerDelete(U8_t timerNumber)
{
	timers_list[timerNumber].counter = 0;
	timers_list[timerNumber].timeOut = 0;
	timers_list[timerNumber].isTimedout = FALSE;
	timers_list[timerNumber].isEnables  =  FALSE;
}

/****************************************************************************
* Function    : void_timerInit
*
* DESCRIPTION : init the timer ,define the callback function and
* 				call void_timerDelete().
*
* PARAMETERS  : None.
*
* Return Value: None.
*
******************************************************************************/
void EF_void_TimerInit(void)
{
	U16_t timersCounter;
	//don't forget put ISR function in startup code
	EF_void_Timer_PORTinit();
	// set the timer interrupt callback
	EF_void_Timer_SetISRCallBack(void_OS_timerISRCallback);
	// set all Timers to the initial state
	for( timersCounter=0 ; timersCounter < MAX_NUMBER_OF_TIMERS ; timersCounter++ )
	{
		EF_void_TimerDelete(timersCounter);
	}
}
/****************************************************************************
* Function    : void_timerInit
*
* DESCRIPTION : set the time out for defined timer.
*
* PARAMETERS  : timerNumber.
* 				timerTimeout.
*
* Return Value: None.
*
******************************************************************************/
void EF_void_TimerCreate(U8_t timerNumber , U32_t timerTimeout) /* Timer timeout in ticks */
{
	if(timers_list[timerNumber].timeOut == 0)
	{
		timers_list[timerNumber].timeOut = timerTimeout;
		timers_list[timerNumber].counter = 0 ;
		timers_list[timerNumber].isTimedout = FALSE ;
		timers_list[timerNumber].isEnables = FALSE;
	}
}
/****************************************************************************
* Function    : void_timersUpdate
*
* DESCRIPTION : count the enabled timer and if it reached the timeOut ,counter=0 and set timeOut Flag
* 				this function called in the ISR by call back function.
*
* PARAMETERS  : None.
*
* Return Value: None.
*
******************************************************************************/
void EF_void_TimersUpdate(void)
{
	U16_t timersCounter=0;

	for( timersCounter=0 ; timersCounter<MAX_NUMBER_OF_TIMERS ; timersCounter++ )
	{
		if(timers_list[timersCounter].isEnables == TRUE)
		{
			timers_list[timersCounter].counter++ ;

			if(timers_list[timersCounter].counter >= timers_list[timersCounter].timeOut)
			{
			    //Note: That I commented this Line
			    /* timers_list[timersCounter].counter = 0; */
				timers_list[timersCounter].isTimedout = TRUE;
			}
			else
			{
			    //Add this Line to reset this flag if isTimedout
                timers_list[timersCounter].isTimedout = FALSE;
			}
		}
	}
}
/****************************************************************************
* Function    : bool_timerCheck
*
* DESCRIPTION : check for time out
*
* PARAMETERS  : timerNumber
*
* Return Value: BOOLEAN : return True when timer timeout
******************************************************************************/
BOOLEAN EF_BOOLEAN_TimerCheck(U8_t timerNumber)
{
	BOOLEAN timerTimeout = FALSE;
	if(timers_list[timerNumber].isTimedout)
	{
		timers_list[timerNumber].isTimedout = FALSE; //take Care!
		timerTimeout = TRUE;
	}
	return timerTimeout;
}

/****************************************************************************
* Function    : EF_BOOLEAN_Timer_IsTimedOut(U8_t timerNumber)
*
* DESCRIPTION : return the status of timer if it finished or not, not change Timer status
*
* PARAMETERS  : timerNumber
*
* Return Value: True if time out.
******************************************************************************/
BOOLEAN EF_BOOLEAN_Timer_IsTimedOut(U8_t timerNumber)
{
    return (timers_list[timerNumber].isTimedout);
}

/****************************************************************************
* Function    : void_timerStart
*
* DESCRIPTION : enable this timer by making isEnables = True
*
* PARAMETERS  : timerNumber
*
* Return Value: None.
*
******************************************************************************/
void EF_void_TimerStart(U8_t timerNumber)
{
	if(timers_list[timerNumber].timeOut != 0)
	{
		timers_list[timerNumber].isEnables = TRUE;
	}
}

/****************************************************************************
* Function    : EF_u8_TimerIsEnabled(U8_t timerNumber)
*
* DESCRIPTION : return the status of Counter is Enabled or not
*
* PARAMETERS  : timerNumber: Number of Timer
*
* Return Value: True if Enableand False if disable.
******************************************************************************/
BOOLEAN EF_u8_TimerIsEnabled(U8_t timerNumber)
{
   return ( timers_list[timerNumber].isEnables);
}

/****************************************************************************
* Function    : EF_void_TimerChangeTimeOut(U8_t timerNumber ,U32_t NewTimeOut)
*
* DESCRIPTION : change the time out of the Given timer
*
* PARAMETERS  : timerNumber: Number of Timer
*               NewTimeOut : New Time Out Value
*
* Return Value: Void.
******************************************************************************/
void EF_void_TimerChangeTimeOut(U8_t timerNumber ,U32_t NewTimeOut)
{
    timers_list[timerNumber].timeOut = NewTimeOut;
}
/****************************************************************************
* Function    : EF_u32_TimerGetCounterValue (U8_t timerNumber )
*
* DESCRIPTION : return the current Counter Value of the wanted Counter.
*
* PARAMETERS  : timerNumber: Number of Timer
*
* Return Value: Counter Value.
******************************************************************************/
U32_t EF_u32_TimerGetCounterValue (U8_t timerNumber )
{
    return ( timers_list[timerNumber].counter);
}

/****************************************************************************
* Function    : void_timerStop
*
* DESCRIPTION : stop this timer by making isEnables = False
*
* PARAMETERS  : timerNumber
*
* Return Value: None.
*
******************************************************************************/
void EF_void_TimerStop(U8_t timerNumber)
{
	timers_list[timerNumber].isEnables = FALSE;
	// Note: that I added this Line
	timers_list[timerNumber].isTimedout = FALSE;
}
/****************************************************************************
* Function    : void_timerReset
*
* DESCRIPTION : make counter = 0
*
* PARAMETERS  : timerNumber
*
* Return Value: None.
*
******************************************************************************/
void EF_void_TimerReset(U8_t timerNumber)
{
	timers_list[timerNumber].counter = 0;
}

void EF_void_Timer_TurnOff(U8_t timerNumber)
{
    timers_list[timerNumber].counter = 0;
    timers_list[timerNumber].isEnables = FALSE;
    // Note: that I added this Line
    timers_list[timerNumber].isTimedout = FALSE;
}
