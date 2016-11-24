/***************************************************************
 *  Source File: EF_IP_IP_CARD.c
 *
 *  Description: EF_IP_IP_CARD driver
 *
 *  History:     Version 1.0  - INIT Version
 *  Date   :     28/10/2015
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
#include "../Utilities/EF_SpechialTIMER.h"
#include "EF_IP_OP_card.h"
#include "../Utilities/std_types.h"
#include "string.h"

#define IP_OP_CARD_UART              UART5
#define RX_ARRAY_LENGTH              3          /* number of receiving array elements , Kit send it as Response */

#define BUZZER_ALARM                 TURN_ON_OUT1
#define NORMAL_WEIGH_ALARM_LED       TURN_ON_OUT2
#define OVER_WEIGH_ALARM_LED         TURN_ON_OUT4
#define UNDER_WEIGH_ALARM_LED        TURN_ON_OUT3
#define START_STOP_INDICATION_LED    TURN_ON_OUT7

#define START_STOP_SWITCH_STATUS     READ_INPUT2
#define PHOTO_CELL_STATUS            READ_INPUT1


const char * TurnONCommandArray [8] =
{
        {"AN"},	// Turn on All relays (All On)
        {"N1"}, //Turn on Relay1
        {"N2"},	//Turn on Relay2
        {"N3"},	//Turn on Relay3
        {"N4"},	//Turn on Relay4
        {"N5"},	//Turn on Relay5
        {"N6"},	//Turn on Relay6
        {"N7"},	//Turn on Relay7
};

const char * TurnOFFCommandArray [8] =
{
        {"AF"}, //Turn off All relays(All Off)
        {"F1"}, //Turn off Relay1
        {"F2"}, //Turn off Relay2
        {"F3"},	//Turn off Relay3
        {"F4"},	//Turn off Relay4
        {"F5"},	//Turn off Relay5
        {"F6"},	//Turn off Relay6
        {"F7"},	//Turn off Relay7
};

const char * ToggleCommandArray [8] =
{
        {"AT"},	//Toggle(reverse) all relays status (All Toggle)
        {"T1"},	//Toggle(reverse) Relay1 state
        {"T2"},	//Toggle(reverse) Relay2 state
        {"T3"},	//Toggle(reverse) Relay3 state
        {"T4"},	//Toggle(reverse) Relay4 state
        {"T5"},	//Toggle(reverse) Relay5 state
        {"T6"},	//Toggle(reverse) Relay6 state
        {"T7"},	//Toggle(reverse) Relay7 state
};

const char * OutputStatusCommandArray [7] =
{
        {"R1"},	//Get Relay1 state
        {"R2"},	//Get Relay2 state
        {"R3"},	//Get Relay3 state
        {"R4"},	//Get Relay4 state
        {"R5"},	//Get Relay5 state
        {"R6"},	//Get Relay6 state
        {"R7"},	//Get Relay7 state
};

const char * ReadInputCommandArray [8] =
{
        {"IA"},	//Read all inputs
        {"I1"},	//Read input1 state
        {"I2"},	//Read input2 state
        {"I3"},	//Read input3 state
        {"I4"},	//Read input4 state
        {"I5"},	//Read input5 state
        {"I6"},	//Read input6 state
        {"I7"},	//Read input7 state
};
UART_cfg_str  uart_cfg = {IP_OP_CARD_UART, 9600, NUMBER_OF_BITS_8, ONE_STOP_BIT,NO_PARITY,FALSE,FALSE,TRUE,TRUE};

/*********************************************************************
 * Function    : EF_BOOLEAN_IP_OP_Card_Init (void);
 *
 * DESCRIPTION : This function used to initialise Input output card ,
 *                init UART and Timer , Send some Init Commands
 *
 * PARAMETERS  : none.
 *
 * Return Value: BOOLEAN to check for Errors
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_Init(void)
{
    EF_void_UART_Init (&uart_cfg);
    _delay_ms(1000);

    return 0;
}


/*********************************************************************
 * Function    : EF_BOOLEAN_IP_OP_Card_TurnOn(U8_t* TurnOnData);
 *
 * DESCRIPTION : This function used to Turn ON  output you need
 *
 *
 * PARAMETERS  : none.
 *
 * Return Value: BOOLEAN to check for Errors
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_TurnOn(U8_t* TurnOnData)
{
    U8_t u8ReturnStatus               = 0;
    U8_t Returndata[RX_ARRAY_LENGTH]  = {0,0,0};
    U8_t u8Index                      = 0;


    EF_void_UART_Init (&uart_cfg);
    EF_void_UART_SendArray( IP_OP_CARD_UART, (U8_t*)TurnOnData, strlen((const char*) TurnOnData));

    for (u8Index =0 ; u8Index < RX_ARRAY_LENGTH; u8Index++)
    {
        EF_BOOLEAN_UART_GetChar( IP_OP_CARD_UART, &Returndata[u8Index]);
    }

    if( Returndata[0] == 'o')
    {
        u8ReturnStatus = TRUE;
    }
    else if ( Returndata[0] == 'r')
    {
        u8ReturnStatus = FALSE;
    }


    return u8ReturnStatus;
}
/*********************************************************************
 * Function    : EF_BOOLEAN_IP_OP_Card_TurnOff(U8_t* TurnOffData);
 *
 * DESCRIPTION : This function used to Turn OFF  output you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: BOOLEAN to check for Errors
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_TurnOff(U8_t* TurnOffData)
{
    U8_t u8ReturnStatus              = 0;
    U8_t Returndata[RX_ARRAY_LENGTH] =  {0,0,0};
    U8_t u8Index                     = 0;


    EF_void_UART_Init (&uart_cfg);
    EF_void_UART_SendArray(IP_OP_CARD_UART,(U8_t*)TurnOffData,strlen((const char*) TurnOffData));

    for (u8Index = 0 ; u8Index < RX_ARRAY_LENGTH; u8Index++)
    {
        EF_BOOLEAN_UART_GetChar(IP_OP_CARD_UART,&Returndata[u8Index]);
    }

    if( Returndata[0] == 'o')
    {
        u8ReturnStatus = TRUE;
    }
    else if( Returndata[0] == 'r')
    {
        u8ReturnStatus = FALSE;
    }

    return u8ReturnStatus;
}

/*********************************************************************
 * Function    : EF_BOOLEAN_IP_OP_Card_Toggle (U8_t* ToggleData);
 *
 * DESCRIPTION : This function used to Toggle output you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: BOOLEAN to check for Errors
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_Toggle(U8_t* ToggleData, U8_t* ReturnToggleData )
{
    U8_t u8ReturnStatus = 0;
    U8_t Returndata[RX_ARRAY_LENGTH] =  {0,0,0};
    U8_t u8Index = 0;


    EF_void_UART_Init (&uart_cfg);
    EF_void_UART_SendArray(IP_OP_CARD_UART,(U8_t*)ToggleData,strlen((const char*) ToggleData));

    for (u8Index =0 ; u8Index < RX_ARRAY_LENGTH; u8Index++)
    {
        EF_BOOLEAN_UART_GetChar(IP_OP_CARD_UART,&Returndata[u8Index]);
    }

    if( Returndata[0] == 'n')
    {
        u8ReturnStatus = TRUE ;
        *ReturnToggleData = 'n' ;
    }
    else if( Returndata[0] == 'f')
    {
        u8ReturnStatus = TRUE ;
        *ReturnToggleData = 'f' ;
    }

    else  if( Returndata[0] == 'r')
    {
        u8ReturnStatus=FALSE;
        *ReturnToggleData = 'r' ;
    }


    return u8ReturnStatus;
}

/*********************************************************************
 * Function    : EF_BOOLEAN_IP_OP_Card_Status ((U8_t* StatusData);
 *
 * DESCRIPTION : This function used to Read Output status  you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: BOOLEAN to check for Errors
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_Status(U8_t* StatusOut,U8_t* StatusData)
{
    U8_t u8ReturnStatus = 0;
    U8_t Returndata[RX_ARRAY_LENGTH] =  {0,0,0};
    U8_t u8Index = 0;


    EF_void_UART_Init (&uart_cfg);
    EF_void_UART_SendArray(IP_OP_CARD_UART,(U8_t*)StatusOut,strlen((const char*) StatusOut));

    for (u8Index =0 ; u8Index < RX_ARRAY_LENGTH; u8Index++)
    {
        EF_BOOLEAN_UART_GetChar(IP_OP_CARD_UART,&Returndata[u8Index]);
    }

    if( Returndata[0] == 'n')
    {
        u8ReturnStatus = TRUE ;
        *StatusData = 'n' ;

    }
    else if( Returndata[0] == 'f')
    {
        u8ReturnStatus = TRUE ;
        *StatusData = 'f' ;

    }

    else if( Returndata[0] == 'r')
    {
        u8ReturnStatus=FALSE;
        *StatusData = 'r' ;

    }

    return u8ReturnStatus;
}

/*********************************************************************
 * Function    : EF_IP_OP_Card_Read_Input (U8_t* ReadInput);
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_Read_Input(U8_t* ReadInput, U8_t* ReturnInputData )
{
    U8_t u8ReturnStatus = 0;
    U8_t Returndata[RX_ARRAY_LENGTH] =  {0,0,0};
    U8_t u8Index = 0;


    EF_void_UART_Init (&uart_cfg);
    EF_void_UART_SendArray(IP_OP_CARD_UART,(U8_t*)ReadInput,strlen((const char*) ReadInput));

    for (u8Index =0 ; u8Index < RX_ARRAY_LENGTH; u8Index++)
    {
        EF_BOOLEAN_UART_GetChar(IP_OP_CARD_UART,&Returndata[u8Index]);
    }


    if( Returndata[0] == 'n')
    {
        u8ReturnStatus =  STATUS_ON ;
        *ReturnInputData = TRUE ;
    }
    else  if( Returndata[0] == 'f')
    {
        u8ReturnStatus = STATUS_OFF ;
        *ReturnInputData = FALSE ;
    }

    else  if( Returndata[0] == 'r')
    {
        u8ReturnStatus = STATUS_ERROR;
        //        *ReturnInputData = 'r' ;
    }


    return u8ReturnStatus;
}




/*********************************************************************
 * Function    :EF_IP_OP_Card_OverWeightLedAlarm(U8_t Data );
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
void  EF_IP_OP_Card_TestProgram(void)
{
    U8_t temp = 0;

    EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_ALL_OUT]);
    _delay_ms(1000);
    EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[TURN_OFF_ALL_OUT]);
    _delay_ms(1000);
    /////////////////////////////////////////////////////////////////////////////////////
    EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT1]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT2]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT3]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT4]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT5]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT6]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT7]);
    _delay_ms(100);
    ///////////////////////////////////////////////////////////////////////////////////////
    EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT1]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT2]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT3]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT4]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT5]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT6]);
    _delay_ms(100);
    EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT7]);

    _delay_ms(1000);
    EF_BOOLEAN_IOCard_Toggle((U8_t*)ToggleCommandArray[TOGGLE_ALL_OUT],&temp);
    _delay_ms(1000);
    EF_BOOLEAN_IOCard_Toggle((U8_t*)ToggleCommandArray[TOGGLE_ALL_OUT],&temp);
    _delay_ms(1000);

    while(1)
    {
        EF_BOOLEAN_IOCard_Read_Input((U8_t*)ReadInputCommandArray[READ_INPUT1],&temp);
        if(temp==TRUE) EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT1]);
        else if(temp==FALSE)  EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT1]);

        EF_BOOLEAN_IOCard_Read_Input((U8_t*)ReadInputCommandArray[READ_INPUT2],&temp);
        if(temp==TRUE) EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT2]);
        else if(temp==FALSE)  EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT2]);

        EF_BOOLEAN_IOCard_Read_Input((U8_t*)ReadInputCommandArray[READ_INPUT3],&temp);
        if(temp==TRUE) EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT3]);
        else if(temp==FALSE)  EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT3]);

        EF_BOOLEAN_IOCard_Read_Input((U8_t*)ReadInputCommandArray[READ_INPUT4],&temp);
        if(temp==TRUE) EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT4]);
        else if(temp==FALSE)  EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT4]);

        EF_BOOLEAN_IOCard_Read_Input((U8_t*)ReadInputCommandArray[READ_INPUT5],&temp);
        if(temp==TRUE) EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT5]);
        else if(temp==FALSE)  EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT5]);

        EF_BOOLEAN_IOCard_Read_Input((U8_t*)ReadInputCommandArray[READ_INPUT6],&temp);
        if(temp==TRUE) EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT6]);
        else if(temp==FALSE)  EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT6]);

        EF_BOOLEAN_IOCard_Read_Input((U8_t*)ReadInputCommandArray[READ_INPUT7],&temp);
        if(temp==TRUE) EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[TURN_ON_OUT7]);
        else if(temp==FALSE)  EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnOFFCommandArray[TURN_OFF_OUT7]);
    }

}



/*********************************************************************
 * Function    :EF_IP_OP_Card_OverWeightLedAlarm(U8_t Data );
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_OverWeightLed(U8_t OverWeightStatus )
{
    U8_t u8ReturnStatus = 0;


    EF_void_UART_Init (&uart_cfg);

    if( OverWeightStatus == TRUE )
    {
        u8ReturnStatus = EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[OVER_WEIGH_ALARM_LED]);
    }

    else if (OverWeightStatus == FALSE )
    {
        u8ReturnStatus =  EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[OVER_WEIGH_ALARM_LED]);
    }


    return u8ReturnStatus;
}



/*********************************************************************
 * Function    : EF_IP_OP_Card_UnderWeightLedAlarm(U8_t Data );
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_UnderWeightLed(U8_t UnderWeightStatus )
{
    U8_t u8ReturnStatus = 0;


    EF_void_UART_Init (&uart_cfg);

    if( UnderWeightStatus == TRUE )
    {
        u8ReturnStatus = EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[UNDER_WEIGH_ALARM_LED]);
    }

    else if (UnderWeightStatus == FALSE )
    {
        u8ReturnStatus = EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[UNDER_WEIGH_ALARM_LED]);
    }

    return u8ReturnStatus;
}



/*********************************************************************
 * Function    : EF_IP_OP_Card_NormalWeightLedAlarm(U8_t Data );
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_AcceptLed(U8_t NormalWeightStatus )
{
    U8_t u8ReturnStatus = 0;


    if( NormalWeightStatus == TRUE )
    {
        u8ReturnStatus = EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[NORMAL_WEIGH_ALARM_LED]);
    }

    else if (NormalWeightStatus == FALSE )
    {
        u8ReturnStatus = EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[NORMAL_WEIGH_ALARM_LED]);
    }

    return u8ReturnStatus;
}

/*********************************************************************
 * Function    : EF_IP_OP_Card_StartStopLed(U8_t Data );
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_StartStopLed(U8_t StartStopStatus )
{
    U8_t u8ReturnStatus = 0;


    EF_void_UART_Init (&uart_cfg);

    if( StartStopStatus == TRUE )
    {
        u8ReturnStatus = EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[START_STOP_INDICATION_LED]);
    }

    else if (StartStopStatus == FALSE )
    {
        u8ReturnStatus = EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[START_STOP_INDICATION_LED]);
    }

    return u8ReturnStatus;
}

/*********************************************************************
 * Function    : EF_IP_OP_Card_Read_Input (U8_t* ReadInput);
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_BuzzerAlarm( U32_t DelayTime_MiliSec)
{

    U8_t u8ReturnStatus = 0;


    EF_void_UART_Init (&uart_cfg);

    u8ReturnStatus = EF_BOOLEAN_IOCard_TurnOn((U8_t*)TurnONCommandArray[BUZZER_ALARM]);
    _delay_ms(DelayTime_MiliSec);

    u8ReturnStatus = EF_BOOLEAN_IOCard_TurnOff((U8_t*)TurnOFFCommandArray[BUZZER_ALARM]);

    return u8ReturnStatus;
}


/*********************************************************************
 * Function    : EF_IP_OP_Card_Read_Input (U8_t* ReadInput);
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_ReadStartStopSwitch()
{
    U8_t u8ReturnStatus = 0;
    U8_t u8ReturnData   = 0;


    u8ReturnStatus = EF_BOOLEAN_IOCard_Read_Input((U8_t*)ReadInputCommandArray[START_STOP_SWITCH_STATUS],&u8ReturnData);
    //TODO : remove delay, Just for test
    _delay_ms(20);

    return u8ReturnStatus;

}

/*********************************************************************
 * Function    : EF_IP_OP_Card_Read_Input (U8_t* ReadInput);
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_ReadPhotoCell()
{
    U8_t u8ReturnStatus = 0;
    U8_t u8ReturnData   = 0;


    u8ReturnStatus = EF_BOOLEAN_IOCard_Read_Input((U8_t*)ReadInputCommandArray[PHOTO_CELL_STATUS],&u8ReturnData);

    return u8ReturnStatus;
}


