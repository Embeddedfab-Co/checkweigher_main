/***************************************************************
 *  Source File: EF_IP_IP_CARD.h
 *
 *  Description: EF_IP_IP_CARD driver
 *
 *  History:     Version 1.0  - INIT Version
 *  Date   :     28/010/2015
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
#ifndef _EF_IP_OP_CARD_H_
#define _EF_IP_OP_CARD_H_
#include "../Utilities/std_types.h"


#define OVER_WEIGH_ALARM_ON      TRUE
#define OVER_WEIGH_ALARM_OFF     FALSE

#define UNDER_WEIGH_ALARM_ON     TRUE
#define UNDER_WEIGH_ALARM_OFF    FALSE

#define NORMAL_WEIGH_ALARM_ON    TRUE
#define NORMAL_WEIGH_ALARM_OFF   FALSE


enum TurnONCommandElement
{
    TURN_ON_ALL_OUT = 0,
    TURN_ON_OUT1,   //Buzzer
    TURN_ON_OUT2,   //green
    TURN_ON_OUT3,   //yellow -->under
    TURN_ON_OUT4,   //red
    TURN_ON_OUT5,   // start/stop led = green
    TURN_ON_OUT6,
    TURN_ON_OUT7
};


#ifndef BUTTON_PRESSED
#define BUTTON_PRESSED               1
#define BUTTON_RELEASED              0
#endif

#define STATUS_ON                    1
#define STATUS_OFF                   0
#define STATUS_ERROR                 3

enum TurnOFFCommandElement
{
    TURN_OFF_ALL_OUT ,
    TURN_OFF_OUT1, //buzzer
    TURN_OFF_OUT2,
    TURN_OFF_OUT3,
    TURN_OFF_OUT4,
    TURN_OFF_OUT5,
    TURN_OFF_OUT6,
    TURN_OFF_OUT7
};

enum ToggleCommandElement{
    TOGGLE_ALL_OUT,
    TOGGLE_OUT1,
    TOGGLE_OUT2,
    TOGGLE_OUT3,
    TOGGLE_OUT4,
    TOGGLE_OUT5,
    TOGGLE_OUT6,
    TOGGLE_OUT7
};

enum OutputStatusCommandElement{
    STATUS_OUT1,
    STATUS_OUT2,
    STATUS_OUT3,
    STATUS_OUT4,
    STATUS_OUT5,
    STATUS_OUT6,
    STATUS_OUT7
};

enum ReadInputCommandElement
{
    READ_ALL_INPUT ,
    READ_INPUT1,
    READ_INPUT2,
    READ_INPUT3,
    READ_INPUT4,
    READ_INPUT5,
    READ_INPUT6,
    READ_INPUT7
};


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
BOOLEAN EF_BOOLEAN_IOCard_Init(void);

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
BOOLEAN EF_BOOLEAN_IOCard_TurnOn(U8_t* TurnOnData);

/*********************************************************************
 * Function    : EF_BOOLEAN_IP_OP_Card_TurnOff(U8_t* TurnOffData);
 *
 * DESCRIPTION : This function used to Turn OFF  output you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: BOOLEAN to check for Errors
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_TurnOff(U8_t* TurnOffData);

/*********************************************************************
 * Function    : EF_BOOLEAN_IP_OP_Card_Toggle (U8_t* ToggleData);
 *
 * DESCRIPTION : This function used to Toggle output you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: BOOLEAN to check for Errors
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_Toggle(U8_t* ToggleData, U8_t* ReturnToggleData );

/*********************************************************************
 * Function    : EF_BOOLEAN_IP_OP_Card_Status ((U8_t* StatusData);
 *
 * DESCRIPTION : This function used to Read Output status  you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: BOOLEAN to check for Errors
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_Status(U8_t* StatusOut,U8_t* StatusData);

/*********************************************************************
 * Function    : EF_IP_OP_Card_Read_Input (U8_t* ReadInput);
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_Read_Input(U8_t* ReadInput, U8_t* ReturnInputData );

/*********************************************************************
 * Function    :EF_IP_OP_Card_OverWeightLedAlarm(U8_t Data );
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_OverWeightLed(U8_t OverWeightStatus );

/*********************************************************************
 * Function    : EF_IP_OP_Card_UnderWeightLedAlarm(U8_t Data );
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_UnderWeightLed(U8_t UnderWeightStatus );

/*********************************************************************
 * Function    : EF_IP_OP_Card_NormalWeightLedAlarm(U8_t Data );
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_AcceptLed(U8_t NormalWeightStatus );

/*********************************************************************
 * Function    : EF_IP_OP_Card_StartStopLed(U8_t Data );
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_StartStopLed(U8_t StartStopStatus );

/*********************************************************************
 * Function    : EF_IP_OP_Card_Read_Input (U8_t* ReadInput);
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_BuzzerAlarm(U32_t DelayTime_MiliSec);

/*********************************************************************
 * Function    : EF_IP_OP_Card_Read_Input (U8_t* ReadInput);
 *
 * DESCRIPTION :if ReturnData =1 --> switch on else if 0 --> off
 *
 * PARAMETERS  : none.
 *
 * Return Value: 0 error, 1 ok then check...
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_ReadStartStopSwitch();

/*********************************************************************
 * Function    : EF_IP_OP_Card_Read_Input (U8_t* ReadInput);
 *
 * DESCRIPTION :This function used to Read Input you need
 *
 * PARAMETERS  : none.
 *
 * Return Value: U8_t to return specific input value
 **********************************************************************/
BOOLEAN EF_BOOLEAN_IOCard_ReadPhotoCell();

void  EF_IP_OP_Card_TestProgram(void);


#endif
