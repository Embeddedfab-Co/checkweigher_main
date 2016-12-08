#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include <string.h>
#include "Utilities/EF_SpechialTIMER.h"
#include "MCAL/EF_TIVA_uart.h"
#include "HAL/EF_Scale_rinstrum.h"
#include "Services/EF_UserInterface.h"
#include "HAL/EF_BarCode_Scanner.h"
#include "MCAL/EF_TIVA_EEPROM.h"
#include "MCAL/EF_TIVA_uart_cfg.h"
#include "Services/EF_PrintMenus.h"
#include "Services/EF_PrintMenus_cfg.h"

#include "Utilities/std_types.h"

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    EF_void_UserInterface_Init ();

    while (1)
    {
        EF_v_UI_SystemStates ();
    }
}


