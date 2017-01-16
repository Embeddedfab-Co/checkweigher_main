/***************************************************************
 *  Source File: EF_TIVA_uart.c
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

#include "../MCAL/EF_TIVA_uart.h"

#define MAX_RX_TRIALS_NUMBER        3
#define MAX_RX_GET_ARRAY_LENGTH     25

// avaliable UART , UART0 , UART6 reserved
#define MAX_NUMBER_OF_UARTS         6

/*
*	 0- UART1 (Tx to B1  ,  Rx to B0) , is limited to 3.6v
*    1- UART2 (Tx to D7  ,  Rx to D6) , with special configuration
*    2- UART3 (Tx to C7  ,  Rx to C6)
*    3- UART4 (Tx to C5  ,  Rx to C4)
*    4- UART5 (Tx to E5  ,  Rx to E4)
*    5- UART7 (Tx to E1  ,  Rx to E0)
*/
// B, D , C , C , E ,E

const  U32_t GPIO_SystCtlClock_Array [MAX_NUMBER_OF_UARTS] = 
	{SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOE };
const  U32_t UART_SystCtlClock_Array [MAX_NUMBER_OF_UARTS] = 
	{SYSCTL_PERIPH_UART1, SYSCTL_PERIPH_UART2, SYSCTL_PERIPH_UART3, SYSCTL_PERIPH_UART4, SYSCTL_PERIPH_UART5, SYSCTL_PERIPH_UART7};
/*{GPIO_BASE_ELEMENT, UART_BASE_ELEMENT, GPIO_RX_ELEMENT, GPIO_TX_ELEMENT, MODULE_RX_ELEMENT, MODULE_TX_ELEMENT, INT_UART_ELEMENT}*/
#define GPIO_BASE_ELEMENT 0
#define UART_BASE_ELEMENT 1
#define GPIO_RX_ELEMENT 2
#define GPIO_TX_ELEMENT 3
#define MODULE_RX_ELEMENT 4
#define MODULE_TX_ELEMENT 5
#define INT_UART_ELEMENT 6

/*typedef enum
{
	GPIO_BASE_ELEMENT , UART_BASE_ELEMENT, GPIO_RX_ELEMENT, GPIO_TX_ELEMENT, MODULE_RX_ELEMENT, MODULE_TX_ELEMENT, INT_UART_ELEMENT
}TenumUART_Array_Elements;*/
const  U32_t UART_PinConfiguration_Array [MAX_NUMBER_OF_UARTS][7] =
	{{GPIO_PORTB_BASE, UART1_BASE, GPIO_PIN_0, GPIO_PIN_1 , GPIO_PB0_U1RX, GPIO_PB1_U1TX, INT_UART1},
	 {GPIO_PORTD_BASE, UART2_BASE, GPIO_PIN_6, GPIO_PIN_7 , GPIO_PD6_U2RX, GPIO_PD7_U2TX, INT_UART2},
	 {GPIO_PORTC_BASE, UART3_BASE, GPIO_PIN_6, GPIO_PIN_7 , GPIO_PC6_U3RX, GPIO_PC7_U3TX, INT_UART3},
	 {GPIO_PORTC_BASE, UART4_BASE, GPIO_PIN_4, GPIO_PIN_5 , GPIO_PC4_U4RX, GPIO_PC5_U4TX, INT_UART4},
	 {GPIO_PORTE_BASE, UART5_BASE, GPIO_PIN_4, GPIO_PIN_5 , GPIO_PE4_U5RX, GPIO_PE5_U5TX, INT_UART5},
	 {GPIO_PORTE_BASE, UART7_BASE, GPIO_PIN_0, GPIO_PIN_1 , GPIO_PE0_U7RX, GPIO_PE1_U7TX, INT_UART7}
	 };


static volatile uint8_t  UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile uint8_t  UART_LastRxError;
static volatile uint16_t UART_RxHead;
static volatile uint16_t UART_RxTail;

/****************************************************************************
* Function    : void_UART_init
*
* DESCRIPTION : init the uart
*
* PARAMETERS  : uart_cfg : pointer to UART_cfg_str which has prameters to init the uart
*
* Return Value: None.
*
******************************************************************************/
void EF_void_UART_Init(UART_cfg_str *uart_cfg)
{
    UART_RxHead = 0;
    UART_RxTail = 0;

	if ((uart_cfg->UART_Number) > MAX_NUMBER_OF_UARTS )
	{
		return; /*if it is not returned void ,return false*/
	}
	/*give clock to GPIO PORT*/
	SysCtlPeripheralEnable( GPIO_SystCtlClock_Array [uart_cfg-> UART_Number] );
	/*Because of special Configuration to PD7*/
	if (uart_cfg->UART_Number == UART2 )
	{ 
		HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
		HWREG(GPIO_PORTD_BASE + GPIO_O_CR ) = 0xff;
		HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
	}
	/*give clock to module*/
	SysCtlPeripheralEnable(UART_SystCtlClock_Array[ uart_cfg-> UART_Number ]);
	/*configure UART pin and define which module will work on this pin*/
	if ( uart_cfg-> TransmitterEnable ) 
	{
		GPIOPinConfigure( UART_PinConfiguration_Array[uart_cfg->UART_Number][MODULE_TX_ELEMENT] );
		GPIOPinTypeUART ( UART_PinConfiguration_Array[uart_cfg->UART_Number][GPIO_BASE_ELEMENT], UART_PinConfiguration_Array[uart_cfg->UART_Number][GPIO_TX_ELEMENT ] );
	}		
	if ( uart_cfg-> ReceiverEnable ) 
	{
		GPIOPinConfigure( UART_PinConfiguration_Array[uart_cfg->UART_Number][MODULE_RX_ELEMENT]);
		GPIOPinTypeUART (UART_PinConfiguration_Array[uart_cfg->UART_Number][GPIO_BASE_ELEMENT], UART_PinConfiguration_Array[uart_cfg->UART_Number][GPIO_RX_ELEMENT ] );
	}
	/*  inside it UARTEnable*/
	UARTConfigSetExpClk(UART_PinConfiguration_Array[uart_cfg->UART_Number][UART_BASE_ELEMENT], SysCtlClockGet(), uart_cfg->baudrate,
						(uart_cfg->numberOfDataBits | uart_cfg->stopBits | uart_cfg->parity));

	if ( (uart_cfg-> RXInterruptEnable) || (uart_cfg-> TXInterruptEnable) )
	{
		IntMasterEnable();
		IntEnable(UART_PinConfiguration_Array[uart_cfg->UART_Number][INT_UART_ELEMENT]);
		if ( (uart_cfg-> RXInterruptEnable)  )
		{ //RT :  Receive Timeout Interrupt Mask
			UARTIntEnable( UART_PinConfiguration_Array[uart_cfg->UART_Number][UART_BASE_ELEMENT] , UART_INT_RX | UART_INT_RT);
		}
		if ( (uart_cfg-> TXInterruptEnable) )
		{
			UARTIntEnable( UART_PinConfiguration_Array[uart_cfg->UART_Number][UART_BASE_ELEMENT] , UART_INT_TX | UART_INT_RT);
		}
	}

	EF_void_TimerCreate(UART_TIMER_ID , UART_WAIT_TIME);
}


/****************************************************************************
* Function    : BOOLEAN_UART_getchar
*
* DESCRIPTION : Getting the RX byte ,used timer to be unStuck
*
* PARAMETERS  : UART_Number : define which uart Module was used
* 				returnedValue: pointer to the Rx byte
*
* Return Value: BOOLEAN : return True if Byte is recieved or false
*
******************************************************************************/
BOOLEAN EF_BOOLEAN_UART_GetChar(U8_t UART_Number,U8_t * returnedValue)
{
    BOOLEAN reciveFlag   = FALSE;
    U32_t  u32ReceivedData = 0;


    EF_void_TimerStart(UART_TIMER_ID);
    /* USART Transmit Complete */
    /*UARTCharsAvail:
     *Returns true if there is data in the receive FIFO or
     *Returns false if there is no data in the receive FIFO.*/
    while( (!( UARTCharsAvail(UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT]))) &&
            (!EF_BOOLEAN_Timer_IsTimedOut(UART_TIMER_ID)) );

    if( /* (UARTCharsAvail(UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT]) == FALSE) && */ (EF_BOOLEAN_Timer_IsTimedOut(UART_TIMER_ID) == TRUE))
    {
        reciveFlag = FALSE;
    }
    else
    {
        reciveFlag     = TRUE;
    }
    EF_void_TimerStop(UART_TIMER_ID);
    EF_void_TimerReset(UART_TIMER_ID);

    //returned -1 if there is no data
    u32ReceivedData = UARTCharGetNonBlocking(UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT]);
    *returnedValue = (U8_t)u32ReceivedData;

    return reciveFlag;
}

//
//{
//    S8_t reciveFlag   = FALSE;
//    int getdata       = 0xFF00;
//
//	EF_void_TimerStart(UART_TIMER_ID);
//	/* USART Transmit Complete */
//	/*UARTCharsAvail:
//	 *Returns true if there is data in the receive FIFO or
//	 *Returns false if there is no data in the receive FIFO.*/
//	do
//	{
//	    //returned -1 if there is no data
//	    getdata= UARTCharGetNonBlocking(UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT]);
//	}while( ( (getdata & (0xFF00)) >= 0 ) && (!EF_BOOLEAN_Timer_IsTimedOut(UART_TIMER_ID)) );
//
//    if (EF_BOOLEAN_TimerCheck(UART_TIMER_ID) == TRUE)
//    {
//        reciveFlag = FALSE;
//    }
//    else
//    {
//        reciveFlag = TRUE;
//    }
//
//    EF_void_TimerStop(UART_TIMER_ID);
//    EF_void_TimerReset(UART_TIMER_ID);
//
////    if(!( UARTCharsAvail(UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT])))
////    {
////    	reciveFlag = FALSE;
////    }
//    *returnedValue = (U8_t)getdata;
//
//	return reciveFlag;
//}



int16_t readFromUART()
{
    uint16_t tmptail;
    uint8_t  data;


    if ( UART_RxHead == UART_RxTail )
    {
        return UART_NO_DATA;   /* no data  */
    }

    /* calculate /store buffer index */
    tmptail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
    UART_RxTail = tmptail;
//    #ifdef PLUS_BOARD
    if (((UART_RxTail + 1) & UART_RX_BUFFER_MASK)== UART_RxHead)
    {
        isArduinoRXBufferEmpty = true;
    }
//    #endif
    /* get data from receive buffer */
    data = UART_RxBuf[tmptail];

    return (UART_LastRxError << 8) + data;
}


int16_t getAvailableDataCountOnUART()
{
    int16_t returnedValue = 0;

    returnedValue =  (UART_RX_BUFFER_SIZE + UART_RxHead - UART_RxTail) & UART_RX_BUFFER_MASK;
    return  returnedValue;
}




/****************************************************************************
* Function    : BOOLEAN_UART_getchar
*
* DESCRIPTION : Getting the RX byte ,used timer to be unStuck
*
* PARAMETERS  : UART_Number : define which uart Module was used
*               ReturnedArray: pointer to the Rx array
*               ByteToStop: byte to stop when finding it.
*
* Return Value: BOOLEAN : return True if Byte is recieved or false
*
******************************************************************************/
BOOLEAN EF_BOOLEAN_UART_GetArray(U8_t UART_Number, U8_t * ReturnedArray, U8_t ByteToStop, U8_t u8MaxNumberOfRxBytes)
{
    U8_t index           = 0 ;
    U8_t b_Status        = 0 ;
    U8_t ReceivingTrials = 0 ;  /* try to receive many trial if fail, noto stuck */
    U8_t ReceviedByte    = 0 ;


    do
    {
        b_Status = EF_BOOLEAN_UART_GetChar( UART_Number, &ReceviedByte );

        if ( b_Status == TRUE )
        {
            *(ReturnedArray + index) = ReceviedByte;
            index++;
        }
        else
        {
            ReceivingTrials++;
        }
    }while ( (*(ReturnedArray + index-1) != ByteToStop) && (ReceivingTrials <= MAX_RX_TRIALS_NUMBER) && (index < u8MaxNumberOfRxBytes) );

    if (index >= u8MaxNumberOfRxBytes)
    {
        b_Status = FALSE;
    }


    return b_Status;
}
/****************************************************************************
* Function    : BOOLEAN_UART_putchar
*
* DESCRIPTION : Sending the TX byte ,used timer to be unStuck
*
* PARAMETERS  : UART_Number : define which uart Module was used
* 				Tx_byte: Tx byte
*
* Return Value: BOOLEAN : return True if Byte is transmitted or false
*
******************************************************************************/
BOOLEAN EF_BOOLEAN_UART_PutChar(U8_t UART_Number, U8_t Tx_byte)
{
	BOOLEAN sendFlag = TRUE;
	EF_void_TimerStart(UART_TIMER_ID);

    while( (!( UARTSpaceAvail(UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT]))) &&
    	   (!EF_BOOLEAN_TimerCheck(UART_TIMER_ID)) );

    EF_void_TimerStop(UART_TIMER_ID);
	EF_void_TimerReset(UART_TIMER_ID);
	sendFlag     = TRUE;
    if(!( UARTSpaceAvail(UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT])))
    {
    	sendFlag = FALSE;
    }

	UARTCharPutNonBlocking(UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT], Tx_byte);    // send character

	return sendFlag;

}
/****************************************************************************
* Function    : u8_UART_waitchar
*
* DESCRIPTION :  Getting the RX byte
*
* PARAMETERS  : UART_Number : define which uart Module was used
*
* Return Value: U8_t : Rx byte
*
******************************************************************************/

U8_t EF_u8_UART_WaitChar(U8_t UART_Number)
{
	U8_t Rx_byte ;
	Rx_byte = UARTCharGet (UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT]);
	return ( Rx_byte );
}

/****************************************************************************
* Function    : void_UART_putchar
*
* DESCRIPTION :  Sending the TX byte
*
* PARAMETERS  : UART_Number : define which uart Module was used
*				Tx_byte
*
* Return Value: None
*
******************************************************************************/
void EF_void_UART_PutChar(U8_t UART_Number, U8_t Tx_byte)
{
	UARTCharPut( UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT], Tx_byte);
}

/****************************************************************************
* Function    : USART_SEND_ARRAY
*
* DESCRIPTION : sending arrays of bytes
*
*
* PARAMETERS  : UART_Number : define which uart Module was used
*				array 		: pointer to TX data
*				Length		: lenght of array
*
* Return Value: None
*
******************************************************************************/
void EF_void_UART_SendArray(U8_t UART_Number, U8_t *array, U8_t Length)
{
    U8_t ArrayIndex;
    for(ArrayIndex = 0; ArrayIndex < Length; ArrayIndex++)
    {
		UARTCharPut( UART_PinConfiguration_Array[UART_Number][UART_BASE_ELEMENT], array[ArrayIndex]);
	}
}

/****************************************************************************
* Function    : EF_void_UART_SendU16
*
* DESCRIPTION : sending short integer ( 2bytes )
*
*
* PARAMETERS  : UART_Number : define which uart Module was used
*               u16Data     : Data wanted to be sent
*
* Return Value: None
*
******************************************************************************/
void EF_void_UART_SendU16(U8_t UART_Number, U16_t u16Data)
{
    /* delay to give time to the anther module to process : todo : if not needed remove */
    _delay_ms(1);

    /* send the MSByte first */
    EF_BOOLEAN_UART_PutChar(UART_Number, (U8_t) (u16Data >> 8));

    /* delay to give time to the anther module to process */
    _delay_ms(1);

    EF_BOOLEAN_UART_PutChar(UART_Number, (U8_t) u16Data);
}

void EF_void_UART_SendU32(U8_t UART_Number, U32_t u32Data)
{
    /* send the MSByte first */
    EF_BOOLEAN_UART_PutChar(UART_Number, (U8_t) (u32Data >> 24));

    EF_BOOLEAN_UART_PutChar(UART_Number, (U8_t) (u32Data >> 16));

    EF_BOOLEAN_UART_PutChar(UART_Number, (U8_t) (u32Data >> 8));

    EF_BOOLEAN_UART_PutChar(UART_Number, (U8_t) u32Data);
}

void EF_void_UART_HextoASCII(U8_t UART_Number, U8_t *hex)
{

    U8_t temp_L = 0;                    // dummy variable that will hold LSBs of Hex number
    U8_t temp_H = 0;                    // dummy variable that will hold MSBs of Hex number

    temp_H = *hex & 0xF0;           // Obtain the upper 4 bits (MSBs) of hex number
    temp_H = temp_H >> 4;           //

    if(temp_H >9)                   // Check if the number is a letter
    {
        temp_H += 0x37;             // Convert the number to a letter in ASCII
    }
    else
    {
        temp_H += 0x30;             // Convert the number to ASCII number
    }

    EF_void_UART_PutChar( UART_Number, temp_H);           // Display the number

    temp_L = *hex & 0x0F;           // Obtain the lower 4 bits (LSBs) of hex number

    if(temp_L >9)                   // Check if the the number is a letter
    {
        temp_L += 0x37;             // Convert the number to a letter in ASCII
    }
    else
    {
        temp_L += 0x30;             // Convert the number to ASCII number
    }

    EF_void_UART_PutChar( UART_Number, temp_L);           // Display the number
}




uint8_t getIsArduinoRXBufferEmptyFlag()
{
    return isArduinoRXBufferEmpty;
}

void setIsArduinoRXBufferEmptyFlag(uint8_t state)
{
    isArduinoRXBufferEmpty = state;
}


uint8_t getIsArduinoRXBufferOverFlowedFlag()
{
    return isArduinoRXBufferOverFlowed;
}

void setIsArduinoRXBufferOverFlowedFlag(uint8_t state)
{
    isArduinoRXBufferOverFlowed = state;
}



ISR_UART_SCREEN ()
{
   uint16_t tmphead;
   uint8_t data;
   uint8_t lastRxError;
   uint32_t ui32Status;


   data = HWREG(SCREEN_UART_BASE + UART_O_DR);
   ui32Status = UARTIntStatus(SCREEN_UART_BASE, true); //get interrupt status
   UARTIntClear(SCREEN_UART_BASE, ui32Status); //clear the asserted interrupts

   lastRxError = ( UARTRxErrorGet (SCREEN_UART_BASE) & (UART_RXERROR_FRAMING|UART_RXERROR_OVERRUN) );

   /* calculate buffer index */
   tmphead = ( UART_RxHead + 1) & UART_RX_BUFFER_MASK;
//   #ifdef PLUS_BOARD
   if ((((tmphead + 128) & UART_RX_BUFFER_MASK)== UART_RxTail))
   {
       isArduinoRXBufferEmpty = false;
   }
//   #endif

   if ( tmphead == UART_RxTail )
   {
       /* error: receive buffer overflow */
       lastRxError = UART_BUFFER_OVERFLOW >> 8;
//       #ifdef PLUS_BOARD
       isArduinoRXBufferOverFlowed = true;
//       #endif
   }
   else
   {
       /* store new index */
       UART_RxHead = tmphead;
       /* store received data in buffer */
       UART_RxBuf[tmphead] = data;
   }
   UART_LastRxError = lastRxError;
}


