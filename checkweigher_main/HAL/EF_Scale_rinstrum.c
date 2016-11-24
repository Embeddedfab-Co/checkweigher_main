
/***************************************************************
 *  Source File: EF_Scale.c
 *
 *  Description: This simple driver for  .
 *
 *  History:     Version 1.0  - INIT Version
 *  Date   :     10/01/2015
 *  -------------------------------------------------------
 *  Author :
 *
 *  Copyright (c) EmbeddedFab, 2015
 *
 *  This unpublished material is proprietary to EmbeddedFab.
 *  All rights reserved. The methods and techniques described
 *  herein are considered trade secrets and/or confidential.
 *  Reproduction or distribution, in whole or in part, is
 *  forbidden except by express written permission of
 *  EmbeddedFab.
 **************************************************************/
#include "EF_Scale_rinstrum.h"

static const char ZeroingCommand []  = "20120008:8002\r\n" ;    /* Zero Command Frame */
static const char GetGrossCommand [] = "20050026:\r\n" ;        /* Get Gross Weigth Command Frame */
static const char PrintKeyCommand [] = "1F120008:8005\r\n" ;    /* to present Print key as if User Pressed it , use this Command Frame */
volatile U8_t  gStableWeightFlag     = 0;                       /* 3: special Error , 0: false , 1: true(gStableWeight_ISR is ready to read)*/
         U32_t gStableWeight_ISR     = 0;                       /* Global Gross Weigth using in ISR ,check gStableWeightFlag , then read it */

UART_cfg_str  ScaleUart_cfg = {SCALE_UART, 9600, NUMBER_OF_BITS_8, ONE_STOP_BIT, NO_PARITY, FALSE, FALSE, TRUE, TRUE};

/*********************************************************************
 * Function    : float ConvertToFloat_Kg ( U8_t * RxPrintArray)
 *
 * DESCRIPTION : Parcing Array of Data (ex. "  3.456 Kg G\r\n") to get Weigth in Kg
 *
 * PARAMETERS  : RxPrintArray: pointer to Rx Data
 *
 * Return Value: Weigth in float (kg)
 ***********************************************************************/
float ConvertToFloat_Kg ( U8_t * RxPrintArray)
{
    double number_afterPoint  = 0.0;
    U8_t   u8AfterPoint_times = 0 ;
    double IntNumber          = 0.0;
    U8_t   u8Iterator         = 0;

    /* Convert Number from ASCII to float, get the Integer number before the Dot */
    for (u8Iterator = 1 ; RxPrintArray[u8Iterator] != '.'; u8Iterator++)
    {
        IntNumber = IntNumber*10 + RxPrintArray[u8Iterator] - '0';
    }

    u8Iterator++;       /* add 1 to avoid the dot */
    /* Looping to get the number after Dot */
    while (RxPrintArray[u8Iterator] != ' ')
    {
       u8AfterPoint_times++;
       number_afterPoint = number_afterPoint*10 + RxPrintArray[u8Iterator] - '0';
       u8Iterator++;
    }
    for (u8Iterator= 0 ; u8Iterator < u8AfterPoint_times ; u8Iterator++ )
    {
        number_afterPoint = number_afterPoint/10;
    }

    IntNumber = IntNumber + number_afterPoint;

    return IntNumber;
}


/*********************************************************************
 * Function    : U32_t ConvertToInt_Grams ( U8_t * RxPrintArray)
 *
 * DESCRIPTION : Parcing Array of Data (ex. "  3.456 Kg G\r\n") to get Weigth in g
 *
 * PARAMETERS  : RxPrintArray: pointer to Rx Data
 *
 * Return Value: Weigth in int (grams)
 ***********************************************************************/
U32_t float_ConvertToInt_Grams ( U8_t * RxPrintArray)
{
    U16_t u16Number_afterPoint = 0;    /* to calculate the number after point in it */
    U8_t  u8AfterPoint_times   = 0;    /* to calculate the number of digits of number after point */
    U32_t u32IntNumber         = 0;    /* to calculate All number in it */
    U8_t  u8Iterator           = 0;


    /* Parcing Array of Data (ex. "  3.456 Kg G\r\n")... get the Number before '.' */
    for (u8Iterator = 0 ; RxPrintArray[u8Iterator] != '.'; u8Iterator++)
    {
        if (RxPrintArray[u8Iterator] == ' ')
        {
            u32IntNumber = 0;
        }
        else
        {
            u32IntNumber = u32IntNumber*10 + RxPrintArray[u8Iterator] - '0';
        }
    }

    u8Iterator++;       /* add 1 to avoid the dot */

    /* get the Number after '.' */
    while (RxPrintArray[u8Iterator] != ' ')
    {
       u16Number_afterPoint = u16Number_afterPoint*10 + RxPrintArray[u8Iterator] - '0';
       u8AfterPoint_times++;
       u8Iterator++;
    }

    u32IntNumber = u32IntNumber*1000 + u16Number_afterPoint;

    return u32IntNumber;

}

/*********************************************************************
* Function    : EF_BOOLEAN_ESP8266_ReadUntilExpectedKeyword
*
* DESCRIPTION : This function used to receive Array by UART, ans stop until Expected
*               keyword received or when timeout occured
*
* PARAMETERS  : ExpectedKeyword : Expected array to stop when find it or when time out
*               ExpectedKeyword_Length : length of ExpectedKeyword Array.
*               TimeOut : mili seconds (Ex: if timeout 5000 means 5sec ), using to extract
*                         from this function if ExpectedArray was not found
*
* Return Value: TRUE  : means OK, found this Expected word in Receiving progress.
*               FALSE : means time is out and not found the ExpectedArray
*
* NOTE: initialize the timer and global interrupt before using this function.
**********************************************************************/
BOOLEAN EF_BOOLEAN_SCALE_ReadUntilExpectedKeyword (U8_t* ExpectedKeyword, U8_t ExpectedKeyword_Length, U16_t TimeOut)
{
    U8_t u8Index          = 0 ;
    U8_t RxData[20]       = {0};    /* array to receive uart in it and compare with Expected Array */
    U8_t u8ReturnedStatus = TRUE;


    /* using Timer to extract if reaching time out */
    EF_void_TimerDelete( SCALE_TIMER_ID );
    EF_void_TimerCreate( SCALE_TIMER_ID , TimeOut);
    EF_void_TimerStart ( SCALE_TIMER_ID);

    /* receive Array until get the Expected Keyword */
    for (u8Index = 0 ; (u8Index < ExpectedKeyword_Length) && (u8ReturnedStatus != FALSE); u8Index++)
    {
        u8ReturnedStatus = EF_BOOLEAN_UART_GetChar( SCALE_UART, &RxData[u8Index] );
    }

    while ( (EF_BOOLEAN_TimerCheck( SCALE_TIMER_ID ) == FALSE) && (u8ReturnedStatus != FALSE))
    {
        /* compare with every receiving array with ExpectedKeyword_Length characters then shift one byte
         * to compare with every Similar width array */
        for (u8Index =0 ; u8Index < ExpectedKeyword_Length; u8Index++)
        {
            if (RxData[u8Index] != ExpectedKeyword[u8Index])
            {
                break;
            }
            if (u8Index == ExpectedKeyword_Length -1)
            {
#ifdef UART_TERMINAL_DEBUG
                UARTprintf( "\n---Found Keyword---\n" );
                UARTprintf( "%s\n\n" , ExpectedKeyword );
#endif
                EF_void_TimerStop  ( SCALE_TIMER_ID );
                EF_void_TimerReset ( SCALE_TIMER_ID );
                return TRUE;
            }
        }

        /* shift one byte and receive new byte then compare */
        for (u8Index =0 ; u8Index < ExpectedKeyword_Length-1; u8Index++)
        {
            RxData[u8Index] = RxData[u8Index+1];
        }

        EF_BOOLEAN_UART_GetChar( SCALE_UART, &RxData[ExpectedKeyword_Length-1] );
    }
#ifdef UART_TERMINAL_DEBUG
    UARTprintf("\n---Time Out---\n");
#endif

    EF_void_TimerStop(SCALE_TIMER_ID);
    EF_void_TimerReset(SCALE_TIMER_ID);

    return FALSE;
}


/*************************************************
*     API Functions
*************************************************/

/*********************************************************************
 * Function    : BOOLEAN EF_BOOLEAN_Scale_Init (U8_t b_isISR)
 *
 * DESCRIPTION : Initializes Uart
 *
 * PARAMETERS  : b_isISR: define Enable Rx Interrupt or not, if 1 : Enable Interrupt
 *
 * Return Value: The status either success (1) or failure (0)
 ***********************************************************************/
BOOLEAN EF_BOOLEAN_Scale_Init (U8_t b_isISR)
{
    if (b_isISR == TRUE)
    {
       ScaleUart_cfg .RXInterruptEnable=TRUE;
    }

    EF_void_UART_Init (&ScaleUart_cfg);
    return TRUE;

}

/*********************************************************************
* Function    : BOOLEAN EF_BOOLEAN_Scale_Set_Zeroing (void)
*
* DESCRIPTION : Nothing
*
* PARAMETERS  : Nothing
*
* Return Value: The status either success (1) or failure (-1).
***********************************************************************/
BOOLEAN EF_BOOLEAN_Scale_Set_Zeroing (void)
{
    U8_t u8ZeroingStatus = 0;
    U8_t u8Iterator      = 0;


    /* Send Zeroing Command via Uart , repeat many time if not Zeroing to prevent Errors */
    for (u8Iterator = 0; ( (u8Iterator < ZEROING_TRAILS) && (u8ZeroingStatus == FALSE) ); u8Iterator++)
    {
        EF_void_UART_SendArray( SCALE_UART, (U8_t*) ZeroingCommand, strlen(ZeroingCommand));

        /* if zeroing done , it should receive "9F120008:0000\x0D\x0A" */
        u8ZeroingStatus = EF_BOOLEAN_SCALE_ReadUntilExpectedKeyword ( (U8_t*)":0000", strlen (":0000"), 1000);
    }

	return u8ZeroingStatus;
}

/*********************************************************************
* Function    : BOOLEAN  EF_BOOLEAN_Scale_Get_Gross_Weight (U32_t * u32WeightInGrams,
*                                                        BOOLEAN * bScaleError)
*
* DESCRIPTION : Gets the real value of gross weight.
*
* PARAMETERS  : u32WeightInGrams: Pointer to the Gross weight in Grams
*               bScaleError
*
* Return Value: BOOLEAN to check for Errors
***********************************************************************/
BOOLEAN  EF_BOOLEAN_Scale_Get_Gross_Weight (U32_t * u32WeightInGrams, BOOLEAN * bScaleError)
{
    U8_t u8ReturnedStatus = 0;
    U8_t ReturnedArray[25] = {0};


    /* to clear UART Buffer */
    EF_void_UART_Init (&ScaleUart_cfg);

    /* Send Get Gross Command */
    EF_void_UART_SendArray( SCALE_UART, (U8_t*) GetGrossCommand, strlen(GetGrossCommand));

    /* Get the Response */
    u8ReturnedStatus = EF_BOOLEAN_UART_GetArray( SCALE_UART,  ReturnedArray, '\n');

    if (u8ReturnedStatus == FALSE)
    {
        return FALSE;
    }
    /* check the error bit in the response frame */
    if ( (ReturnedArray[0] & (1<< ERROR_BIT) ) == (1<< ERROR_BIT) )
    {
        return RETURN_ERROR_REPONSE;
    }

     /* Convert From Ascii (Like 1.545 kg) to Integer number ,but in grams */
    *u32WeightInGrams = float_ConvertToInt_Grams( (U8_t *) (ReturnedArray+9));

    return TRUE;

}

/*********************************************************************
* Function    : S16_Scale_Get_Stable_Weight  (void);
*
* DESCRIPTION : Get the real number of weight in grams between 0 and
*               50000
*
* PARAMETERS  : Nothing
*
* Return Value: The value of weight. And returns -1 to indicate that
*               print isn't available
***********************************************************************/
#ifndef WEIGHT_STUB
BOOLEAN  B_Scale_Get_Stable_Weight_raw (volatile U16_t * u16WeightInGrams, BOOLEAN * bScaleError)
{

	return TRUE;
}


/*********************************************************************
* Function    : BOOLEAN  EF_BOOLEAN_Scale_Get_Stable_Weight (U32_t * u32WeightInGrams,
*                                                        BOOLEAN * bScaleError)
*
* DESCRIPTION : Gets the real value of gross weight.
*
* PARAMETERS  : u32WeightInGrams: Pointer to the Gross weight in Grams
*               bScaleError
*
* Return Value: BOOLEAN to check for Errors
***********************************************************************/
BOOLEAN  EF_BOOLEAN_Scale_Get_Stable_Weight (volatile U32_t * u32WeightInGrams, BOOLEAN * bScaleError)
{
     U8_t b_ReturnedStatus = 0;
     U8_t ReturnedArray[20] = {0};  /* to receive the Response in it */


     /* to clear Rx Uart Buffer berfore Recieving new data */
     EF_void_UART_Init (&ScaleUart_cfg);
     /* Send Get Stable Weight Command  */
     EF_void_UART_SendArray( SCALE_UART, (U8_t*) PrintKeyCommand, strlen(PrintKeyCommand));
     /* Wait for Response until 5 sec (Large delay to wait stabililty) , response like: "  4.567 Kg G"
      * wait until getting the start byte of response frame " " . if Scale was not stable, no data will be received*/
     b_ReturnedStatus = EF_BOOLEAN_SCALE_ReadUntilExpectedKeyword ((U8_t*)" ", 1, 5000);

     if (b_ReturnedStatus == TRUE)
     {
         /* get the Response Array*/
         b_ReturnedStatus = EF_BOOLEAN_UART_GetArray( SCALE_UART,  ReturnedArray, '\n');

          if (b_ReturnedStatus == TRUE)
          {
              //TODO : check For Error bit
              /* check the error bit in the response frame */
              if ( (ReturnedArray[0] & (1<< ERROR_BIT) ) == (1<< ERROR_BIT) )
              {
                  return RETURN_ERROR_REPONSE;
              }
              /* convert the number from ASCII to Integer in grams  */
              *u32WeightInGrams = float_ConvertToInt_Grams( (U8_t *) (ReturnedArray ));
          }
     }

     return b_ReturnedStatus;
}


#else
//BOOLEAN  B_Scale_Get_Stable_Weight (volatile U16_t * u16WeightInGrams, BOOLEAN * bScaleError)
//{
//	static U8_t u8WeightCounter = 0;
//
//	if(u8WeightCounter > 255)
//	{
//		u8WeightCounter = 0;
//	}
//
//	*u16WeightInGrams = u8WeightCounter++;
//
//	return TRUE;
//}
#endif
/*********************************************************************
* Function    : F64_Get_Increased_Resolution_Weight  (void);
*
* DESCRIPTION : Gets the real value of increased resolution weight.
*
* PARAMETERS  : Nothing
*
* Return Value: The value of returned weight with increased resolution
*               between 0 grams and 50000 grams. And returns -1 to
*               indicate that error occured with increased resolution.
***********************************************************************/
BOOLEAN  B_Scale_Get_Increased_Resolution_Weight (FP64 * f64WeightInGrams, BOOLEAN * bScaleError)
{


	return TRUE;

}

/*********************************************************************
* Function    : s16_Get_Indicated_Weight_Number(S8_t *);
*
* DESCRIPTION : Gets the real value of indicated weight.
*
* PARAMETERS  : Nothing
*
* Return Value: The real value of indicated weight between 0 grams and
*               50000 grams. And returns -1 to indicate overload.
***********************************************************************/
BOOLEAN  B_Scale_Get_Indicated_Weight (U16_t * u16WeightInGrams, BOOLEAN * bScaleError)
{


	return TRUE;
}

/*********************************************************************
* Function    : B_Get_All_Weight  (S16_t *);
*
* DESCRIPTION : Get the real values of all weights in grams between
*               0 and 5000 grams. We get the net, tare and gross
*               weights
*
* PARAMETER 1 : The returned frame of containing real values of all
*               weights between 0 grams and 50000 grams. And returns
*               -1 to indicate ADC out error.
*
* Return Value: void
***********************************************************************/
BOOLEAN B_Scale_Get_All_Weight  (U16_t *u16Weights, BOOLEAN * bScaleError)
{


	return TRUE;
}

/*********************************************************************
* Function    : S16_Get_Count_Number  (void);
*
* DESCRIPTION : Gets the real value of scale count.
*
* PARAMETERS  : void
*
* Return Value: The real value of count. And returns negative values in
*               case of failure.
***********************************************************************/
BOOLEAN B_Scale_Get_Count  (U16_t * u16NetCount, BOOLEAN * bScaleError)
{


	return TRUE;
}

/*********************************************************************
* Function    : B_Get_Status(S8_t *);
*
* DESCRIPTION : Get the current status of the scale
*
* PARAMETERS  : Pointer to returned frame of status in ASCII.
*               Frame is in form [ADR][S][STATUS-1][STATUS-2][STATUS-3]
*               STATUS-1 : Stable or dynamic
*               STATUS-2 : Gross  or net
*               STATUS-3 : In range or out of range
*               The frame of length 15.
*
* Return Value: Returns true in case status is normal and returns -1 in
*               case the status is out of range
***********************************************************************/
#ifndef WEIGHT_STUB
BOOLEAN B_Scale_Get_Status  (BOOLEAN *bScaleError)
{

	return TRUE;
}
#else
BOOLEAN B_Scale_Get_Status  (BOOLEAN *bScaleError)
{
	*bScaleError = FALSE;

	return TRUE;
}
#endif
/*********************************************************************
* Function    : B_Apply_Tare  (void);
*
* DESCRIPTION : Apply taring for the scale
*
* PARAMETERS  : Nothing
*
* Return Value: Returns true for success, -1 for execution failure or
*               -2 for unavailability.
***********************************************************************/
#ifndef WEIGHT_STUB
S16_t s16_Scale_Apply_Tare  (BOOLEAN * bScaleError)
{

	return TRUE;
}
#else
S16_t s16_Scale_Apply_Tare  (BOOLEAN * bScaleError)
{
	*bScaleError = FALSE;
	return TRUE;
}
#endif
/*********************************************************************
* Function    : B_Clear_Tare_Memory  (void);
*
* DESCRIPTION : clears the tare memory
*
* PARAMETERS  : Nothing
*
* Return Value:  Returns true for success, -1 for execution failure or
*               -2 for unavailability.
***********************************************************************/
BOOLEAN B_Scale_Clear_Tare_Memory  (BOOLEAN * bScaleError)
{

	return TRUE;
}


/*********************************************************************
* Function    : ISR_Scale_GetStableWeigth ()
*
* DESCRIPTION : ISR Function to get stable gross weigth
*
* PARAMETERS  :
*
* Return Value:
***********************************************************************/
void ISR_Scale_GetStableWeigth ()
{
     U8_t  u8Rxdata = 0 ;
     U8_t  u8ReturnedStatus = 0;
     U32_t u32Status = 0;
     U8_t  ReturnedArray [15] = {0} ;

    u32Status = UARTIntStatus(SCALE_UART, true); //get interrupt status
    UARTIntClear(SCALE_UART, u32Status); //clear the asserted interrupts

    EF_BOOLEAN_UART_GetChar( SCALE_UART, &u8Rxdata);
    /* return like "  12.400 kg G" , parcing to get Space " ", then get Number */
    if (u8Rxdata == ' ')
    {
        /* if find " " , get Array to reach the end of frame */
        u8ReturnedStatus = EF_BOOLEAN_UART_GetArray( SCALE_UART,  ReturnedArray, '\n');
        if (u8ReturnedStatus == FALSE)
        {
            /* Problem in UART Get */
            gStableWeightFlag = 3;
        }
        else
        {
            /* TODO gStableWeightFlag must be =0 after using gStableWeight_ISR*/
            gStableWeightFlag = 1;
            gStableWeight_ISR = float_ConvertToInt_Grams( (U8_t *) (ReturnedArray ));
        }
    }
    else
    {
        gStableWeightFlag = 0;
    }


}

/* and use with ISR Function code in Main.c like this */
//        if (gStableWeightFlag == 1)
//        {
//            //test
//            gStableWeight_ISR = gStableWeight_ISR+1;
//
//            gStableWeightFlag=0;
//        }
