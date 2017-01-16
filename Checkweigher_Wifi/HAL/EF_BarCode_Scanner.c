/*
 * EF_BareCode_Scanner.c
 *
 *  Created on: 2 Nov 2015
 *      Author: EmbeddedFab
 */

#include "../MCAL/EF_TIVA_uart.h"
#include "../Utilities/EF_SpechialTIMER.h"
#include "utils/uartstdio.h"


#define BARCODE_UART        UART2
extern volatile U8_t gU8BarCodeUartInit_FirstTime ;       /* used to make UARTinit in the First trial to get the BarCode */
UART_cfg_str  BarCodeUart_cfg = { BARCODE_UART, 9600, NUMBER_OF_BITS_8, ONE_STOP_BIT, NO_PARITY, FALSE, FALSE, TRUE, TRUE};

/*********************************************************************
* Function    : EF_BOOLEAN_BarCodeScanner_Init ()
*
* DESCRIPTION : initialize the Uart
*
* PARAMETERS  : None.
*
* Return Value: Void.
***********************************************************************/
BOOLEAN EF_BOOLEAN_BarCodeScanner_Init(void)
{
    EF_void_UART_Init (&BarCodeUart_cfg);
    _delay_ms(1000);

    return 0;
}



/*********************************************************************
* Function    : EF_BOOLEAN_BarCodeScanner_ReadData (U32_t * BarCodeMeasuredWeight)
*
* DESCRIPTION : try to Read Barcode
*
* PARAMETERS  : pU32BarCodeMeasuredWeight : pointer to the returned read Barcode.
*
* Return Value: True if get the barcode correctly or false if fail.
***********************************************************************/
BOOLEAN EF_BOOLEAN_BarCodeScanner_ReadData(U32_t * pU32BarCodeMeasuredWeight)
{
    U8_t  u8ReturnStatus          = TRUE ;
    U8_t  ScannedReturnArray[20]  = {0};
    U32_t u32Weigth               = 0;
    U8_t  u8Index                 = 0;
#ifdef UART_TERMINAL_DEBUG
    U8_t  iterator                = 0;
#endif

    if (gU8BarCodeUartInit_FirstTime == TRUE)
    {
        EF_void_UART_Init (&BarCodeUart_cfg);
        gU8BarCodeUartInit_FirstTime = FALSE;
    }

    /* receive the Data until get (.) , as the barcode like as 321.0 */
    u8ReturnStatus = EF_BOOLEAN_UART_GetArray( BARCODE_UART , ScannedReturnArray , '.' , 10);
    if (u8ReturnStatus == TRUE)
    {
#ifdef UART_TERMINAL_DEBUG
        for ( iterator = 0; ScannedReturnArray[iterator] != '.'; iterator++)
        {
            UARTprintf( "%c" ,  ScannedReturnArray[iterator] );
        }
#endif
        /* Parcing the Array and convert the Number from Ascii to integer */
        while ( (ScannedReturnArray[u8Index] != '.') && (u8Index < 20) )
        {
            ScannedReturnArray[u8Index] = ScannedReturnArray[u8Index] - '0';
            u32Weigth = u32Weigth *10  + ScannedReturnArray[u8Index];
            u8Index++;
        }

        *pU32BarCodeMeasuredWeight = u32Weigth;
    }

    return u8ReturnStatus;
}


