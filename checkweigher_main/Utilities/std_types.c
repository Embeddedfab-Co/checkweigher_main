#include "../Utilities/std_types.h"

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

void _delay_ms (U32_t MiliSeconds)
{
	/*NOTE:  SysCtlClockGet with 80MHZ return wrong number! */
	U32_t Sys_ClockCycles = 0;
	Sys_ClockCycles = SysCtlClockGet();
	/*
	 *  SysCtlDelay function takes 3 clock cycles
	 * Cycles of 1s = (Sys_ClockCycles)/(3)
	 * Cycles of 1ms = (Sys_ClockCycles)/(3*1000)
	 * * */
	SysCtlDelay ( ((Sys_ClockCycles)/(3*1000))*MiliSeconds );
}

void _delay_us (U32_t MicroSeconds)
{
	/* NOTE:  SysCtlClockGet with 80MHZ return wrong number! */
	U32_t Sys_ClockCycles = 0;
	Sys_ClockCycles = SysCtlClockGet();
	/*
	 * SysCtlDelay function takes 3 clock cycles
	 * Cycles of 1s = (Sys_ClockCycles)/(3)
	 * Cycles of 1us = (Sys_ClockCycles)/(3*1000000)
	 *  * */
	SysCtlDelay ( ((Sys_ClockCycles)/(3*1000000))*MicroSeconds );
}
