/***************************************************************
 *  Source File: EF_UserInterface.c
 *
 *  Description: User Interface Module For Avon Project
 *               Using LCD, KeyPad and EEPROM
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
#include "EF_UserInterface.h"
#include "../HAL/EF_BarCode_Scanner.h"
#include "EF_UserInterface_cfg.h"
#include "../MCAL/EF_TIVA_EEPROM.h"
#include "EF_PrintMenus.h"
#include "EF_PrintMenus_cfg.h"

#include "../HAL/EF_IP_OP_card.h"

#include "../Utilities/EF_SpechialTIMER.h"
#include "../HAL/EF_Scale_rinstrum.h"

/*************************************************************
 * Definitions
 **************************************************************/
        U8_t  gu8OperSwitchStatus          = 0;         /* for Switch status */
        U8_t  gu8PatchAnalysisFlag          = 0;        /* used in Analysis Flag */
        U8_t  bScaleError                  = 0;         /* use with Scale Function Argument */
        U32_t gU32MaxCatogriesNumber       = 0;
        U32_t u32FirstRelease_EEPROM = 0;       /* help to init. at First Version Only, So check the FirstRelease_EEPROM Flag to int. them or not  */

        U8_t  gU8BarCodeUartInit_FirstTime = 0;         /* used to make UARTinit in the First trial to get the BarCode */
        U8_t  gU8StopOperations_Flag       = FALSE;     /* Flag to check if 3Months passed--> Diable Operation if Enabled TRAILS_3_MONTHS */
extern  U32_t gU32CounterExpired_3Months;
static  UART_cfg_str  uart_init_cfg        = { UART_TOUCH_SCREEN , SCREEN_UART_BAUDRATE, NUMBER_OF_BITS_8, ONE_STOP_BIT,
                                               NO_PARITY         , TRUE                , FALSE           , TRUE        , TRUE};
CategoryDetailsStr CategoryStruct[MAX_CATEGORY_NUMBER +1];  /* +1 as Categoreis start from 1 and finish at Number 21 */
SystemParametersStrT ParametersStr = { MAX_CATEGORY_WEIGTH , MAX_CATEGORY_WEIGTH , MAX_TOL_VALUE , MAX_CATEGORY_WEIGTH , DEFAULT_PASSWORD };
AnalysisStrT  AnalysisSystem = {0, 0, 0, 0, 0, 0};
AnalysisStrT  AnalysisPatch  = {0, 0, 0, 0, 0, 0};
TimeDetailsStr Time_StartAnalysis;
TimeDetailsStr Time_TillNowAnalysis;

/*************************************************************
 * Local Functions/Tasks/ System States
 **************************************************************/
static void void_InitConsole(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlDelay(5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlDelay(5);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 9600, 16000000);
}

/*********************************************************************
 * Function    : U32_t Convert
 *
 * DESCRIPTION : convert Date and Time to Minutes units
 *
 * PARAMETERS  : timeDetails : struct of type TimeDetailsStr has date and time
 *
 * Return Value: None.
 ***********************************************************************/
U32_t EF_v_ConvertRTC (TimeDetailsStr timeDetails )
{
    U32_t calculation = 0;


    /* convert BCD hexa format to Decimal for Ex: convert (21)hexa to be (21)decimal */
    timeDetails.Year    = timeDetails.Year   - (timeDetails.Year/16)*6;
    timeDetails.Month   = timeDetails.Month  - (timeDetails.Month/16)*6;
    timeDetails.Day     = timeDetails.Day    - (timeDetails.Day/16)*6;
    timeDetails.Hour    = timeDetails.Hour   - (timeDetails.Hour/16)*6;
    timeDetails.Minutes = timeDetails.Minutes- (timeDetails.Minutes/16)*6;

    calculation = timeDetails.Year *24*12*30*60 + timeDetails.Month *24*30*60
                        + timeDetails.Day*24*60 +timeDetails.Hour*60 + timeDetails.Minutes;
 return calculation;
}

/*********************************************************************
 * Function    : EF_v_WriteCatDetailsInEEPROM  ()
 *
 * DESCRIPTION : execute one time only (in the first Burning time ) to save the Weight Tolerance Array in EEPROM.
 *
 * PARAMETERS  : None.
 *
 * Return Value: None.
 ***********************************************************************/
void EF_v_WriteCatDetailsInEEPROM  ()
{
    U8_t  u8Iterator        = 0;
    U16_t u16MemoryLocation = 0;
    U32_t u32CatStatus      = TRUE;  //todo : what put in status , I didn't use it .
    U32_t Tolerance         = TOLERANCE_DEFAULT ;
    U32_t CategoryWeight    = 0;


    for (u8Iterator = MIN_CATEGORY_NUMBER; u8Iterator <= gU32MaxCatogriesNumber ; u8Iterator++)
    {
        CategoryWeight = CAT_WEIGHT_DEFAULT * u8Iterator;

        EF_BOOLEAN_EEPROM_WriteNBytes ( &CategoryWeight , CATEGORIES_ADD_EEPROM +  u16MemoryLocation*4      , 4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &Tolerance      , CATEGORIES_ADD_EEPROM + (u16MemoryLocation + 1)*4 , 4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &Tolerance      , CATEGORIES_ADD_EEPROM + (u16MemoryLocation + 2)*4 , 4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &u32CatStatus   , CATEGORIES_ADD_EEPROM + (u16MemoryLocation + 3)*4 , 4);

        u16MemoryLocation = u16MemoryLocation + 4;
    }
}

/*********************************************************************
 * Function    : void_GetToleranceFromEEPROM  ()
 *
 * DESCRIPTION : get the Weight Tolerance Array from EEPROM to read Tolerance fastly.
 *
 * PARAMETERS  : None.
 *
 * Return Value: None.
 ***********************************************************************/
void EF_v_GetCatDetalisFromEEPROM  ()
{
    U8_t  u8Iterator       = 0;
    U8_t  u8MemoryLocation = 0;
    U32_t u32CatStatus     = 0;

    for (u8Iterator = MIN_CATEGORY_NUMBER; u8Iterator <= gU32MaxCatogriesNumber ; u8Iterator++)
    {
        EF_BOOLEAN_EEPROM_ReadNBytes ( &CategoryStruct[u8Iterator].CategoryWeight  , CATEGORIES_ADD_EEPROM + (u8MemoryLocation)*4   , 4 );
        EF_BOOLEAN_EEPROM_ReadNBytes ( &CategoryStruct[u8Iterator].CategoryTol_Pos , CATEGORIES_ADD_EEPROM + (u8MemoryLocation + 1)*4 , 4 );
        EF_BOOLEAN_EEPROM_ReadNBytes ( &CategoryStruct[u8Iterator].CategoryTol_Neg , CATEGORIES_ADD_EEPROM + (u8MemoryLocation + 2)*4 , 4 );
        u32CatStatus = CategoryStruct[u8Iterator].CategoryStatus;
        EF_BOOLEAN_EEPROM_ReadNBytes ( &u32CatStatus  , CATEGORIES_ADD_EEPROM + (u8MemoryLocation + 3)*4 , 4 );
        u8MemoryLocation = u8MemoryLocation + 4;
    }
}


/* -------------------------------- Operation Task ------------------------------------- */

/*********************************************************************
 * Function    : vOperationTask()
 *
 * DESCRIPTION : to compare Scale Weight with Barcode code with help of Categories tolerance Values
 *
 * PARAMETERS  : Void.
 *
 * Return Value: None.
 ***********************************************************************/
void EF_v_UI_OperationTask()
{
    static U32_t u32BARCODEWeigth          = MIN_WEIGTH;       /* To get BARCODE Weigth */
    static U32_t u32ScaleWeigth            = MIN_WEIGTH;       /* To get Scale Weigth */
           BOOLEAN b_isAcceptPackage       = 0;                /* To define the Package was Accept , used it as argum. for Print Function */
    static U8_t  u8BarCode_Status          = BARCODE_FALSE;    /* To get Bar-Code weight Status */
    static U8_t  u8ScaleStableStatus       = SCALE_NOT_STABLE; /* To get Stable Scale weight Status */
           U32_t u32Tol_PositiveValue      = 0;                /* To save Positive Tolerance from EEPROM in it */
           U32_t u32Tol_NegativeValue      = 0;                /* To get Negative Tolerance  from EEPROM in it */
           S32_t s32Difference             = 0;                /* To save the Difference */
#ifdef USING_VARIABLE_CAT_WEIGHT
           U32_t CategoryWeight            = 0;
           U8_t  u8Counter                 = 0;
           U32_t u32TimerCompareValue      = GET_OPER_VALUES_TIMEOUT/2 ;  /* used in compare timer value */
#endif

    switch (gu8OperSwitchStatus)
    {
    case MENU_WITHOUT_VALUES_STATE :
        /* this state is used to initlize the Variables */
        u8BarCode_Status          = BARCODE_FALSE;
        u8ScaleStableStatus       = SCALE_NOT_STABLE;
        /* if BARCODEWeigth and ScaleWeigth is less than MinWeigth , it will give Error , So if it's Declaration Value is Less
         * than Min and No Value Come from Barcode/Scale then it will give Error, So MinWeigth in Declaration is needed */
        u32BARCODEWeigth          = MIN_WEIGTH;
        u32ScaleWeigth            = MIN_WEIGTH;
        u32TimerCompareValue      = GET_OPER_VALUES_TIMEOUT/2 ;     /* to print Error after this time */
        gu8OperSwitchStatus       = GET_ALL_DATA_STATE;
        EF_void_Timer_TurnOff ( GET_OPER_VALUES_TIMER_ID );
        EF_void_Timer_TurnOff ( NEW_OPERATION_TIMER_ID );
        break;


    case GET_ALL_DATA_STATE :
        /* this state helps me to start the Get Value timer in the first Entery of any new operation */
        /* Print Operation Menu if Not Error Found, else after Error ,Print Operation Menu */
        if ( (EF_u8_TimerIsEnabled ( ERROR_OPERATION_TIMER_ID ) == TRUE) )
        {
            if (EF_BOOLEAN_TimerCheck ( ERROR_OPERATION_TIMER_ID ) == TRUE)
            {
                EF_void_Timer_TurnOff ( ERROR_OPERATION_TIMER_ID );
                EF_void_PrintMenus_OperationWithZeroing ();
            }
        }
        else
        {
            EF_void_PrintMenus_OperationWithZeroing ();
        }

#ifdef USING_BARCODE
        /* Get Barcode and scale if it was possible */
#ifdef BARCODE_STUB
        u8BarCode_Status  = TRUE ;
        u32BARCODEWeigth  = 3000 ;
#else
         u8BarCode_Status   = EF_BOOLEAN_BarCodeScanner_ReadData ( &u32BARCODEWeigth ) ;
#endif
#endif

#ifdef USING_PROXIMITY
         /* NOTE: remeber Check if using  Proximity Stub */
        if (EF_BOOLEAN_IOCard_ReadPhotoCell() == FOUND_PACKAGE)
        {


#ifdef USING_BARCODE
#ifdef STUB_SCALE
            u8ScaleStableStatus = TRUE;
            u32ScaleWeigth      = 5000;
#else
            u8ScaleStableStatus = EF_BOOLEAN_Scale_Get_Stable_Weight ( &u32ScaleWeigth , &bScaleError );
#endif
#else
            gu8OperSwitchStatus   = GET_SCALE_STATE;
            EF_void_TimerStart ( GET_OPER_VALUES_TIMER_ID );

#endif
        }
        else
        {
            u8ScaleStableStatus = SCALE_NOT_STABLE;
        }
#else

#ifdef STUB_SCALE
        u8ScaleStableStatus = TRUE;
        u32ScaleWeigth     = 5000;
#else
        u8ScaleStableStatus = EF_BOOLEAN_Scale_Get_Stable_Weight ( &u32ScaleWeigth , &bScaleError );
#endif

#endif

#ifdef USING_BARCODE
        /* if Scale stable : print it, start Get Barcode timer and goto GET_BARCODE_STATE */
        if ( (u8BarCode_Status == BARCODE_OK) && (u8ScaleStableStatus == SCALE_STABLE) )
        {
            /* check for Weight limits. if Error found ,Print Weight Error and start Error Timer */
            if ( (u32BARCODEWeigth > ParametersStr.Barcode_Max) || (u32BARCODEWeigth < MIN_WEIGTH))
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_BARCODE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MAX_WEIGHT_V , ParametersStr.Barcode_Max, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );

                EF_void_TimerStart ( ERROR_OPERATION_TIMER_ID );
            }
            /* check for Weight limits. if Error found ,Print Weight Error and start Error Timer */
            else if ( (u32ScaleWeigth >  ParametersStr.Scale_Max) || (u32ScaleWeigth < MIN_WEIGTH))
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_SCALE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MAX_WEIGHT_V , ParametersStr.Scale_Max, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );
                EF_void_TimerStart    ( ERROR_OPERATION_TIMER_ID );
            }
            else
            {
                EF_void_Timer_TurnOff (ERROR_OPERATION_TIMER_ID);
                gu8OperSwitchStatus     = CALCULAION_STATE;
            }
        }
        else if ( (u8BarCode_Status == BARCODE_FALSE) && (u8ScaleStableStatus == SCALE_STABLE) )
        {
            /* check for Weight limits. if Error found ,Print Weight Error and start Error Timer */
            if ( (u32ScaleWeigth > ParametersStr.Scale_Max) || (u32ScaleWeigth < MIN_WEIGTH))
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_SCALE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MAX_WEIGHT_V , ParametersStr.Scale_Max, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );

                EF_void_TimerStart    ( ERROR_OPERATION_TIMER_ID );
            }
            else
            {
                EF_void_Timer_TurnOff (ERROR_OPERATION_TIMER_ID);
                EF_void_PrintMenus_OperationWithZeroing ();
                EF_void_PrintMenus_SendValue ( OPERATION_SCALE_WEIGHT_V , u32ScaleWeigth, TRUE );
                gu8OperSwitchStatus     = GET_BARCODE_STATE;
                EF_void_TimerStart ( GET_OPER_VALUES_TIMER_ID );
            }
        }
        /* else if Barcode found ,print it and goto GET_SCALE_STATE */
        else if ( (u8BarCode_Status == BARCODE_OK) && (u8ScaleStableStatus == SCALE_NOT_STABLE) )
        {
            /* check for Weight limits. if Error found ,Print Weight Error and start Error Timer */
            if ( (u32BARCODEWeigth > ParametersStr.Barcode_Max) || (u32BARCODEWeigth < MIN_WEIGTH))
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_BARCODE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MAX_WEIGHT_V , ParametersStr.Barcode_Max, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );

                EF_void_TimerStart ( ERROR_OPERATION_TIMER_ID );
            }
            else
            {
                EF_void_Timer_TurnOff ( ERROR_OPERATION_TIMER_ID );
                EF_void_PrintMenus_OperationWithZeroing ();
                EF_void_PrintMenus_SendValue ( OPERATION_BARCODE_WEIGHT_V , u32BARCODEWeigth, TRUE );
                gu8OperSwitchStatus     = GET_SCALE_STATE;
                EF_void_TimerStart ( GET_OPER_VALUES_TIMER_ID );
            }
        }
#endif
        break;

    case GET_BARCODE_STATE :
        /* if timer error is out and tries < 3 , print operation and try to get the value */
        if ( EF_BOOLEAN_TimerCheck (GET_OPER_VALUES_TIMER_ID) == TRUE)
        {
            EF_void_Timer_TurnOff ( GET_OPER_VALUES_TIMER_ID );
            gu8OperSwitchStatus = MENU_WITHOUT_VALUES_STATE;
        }
        /* if timer get barcode is out,print error for 1 sec and stop Start Timer */
        else if ( EF_u32_TimerGetCounterValue(GET_OPER_VALUES_TIMER_ID) >= u32TimerCompareValue )
        {
            u32TimerCompareValue      = GET_OPER_VALUES_TIMEOUT*4 ;

            if ( (EF_u8_TimerIsEnabled ( ERROR_OPERATION_TIMER_ID ) == FALSE) )
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_WATING_BARCODE_PHOTO);
            }
            else
            {
                if (EF_BOOLEAN_TimerCheck ( ERROR_OPERATION_TIMER_ID ) == TRUE)
                {
                    EF_void_Timer_TurnOff ( ERROR_OPERATION_TIMER_ID );
                }
            }
        }

        u8BarCode_Status  = EF_BOOLEAN_BarCodeScanner_ReadData ( &u32BARCODEWeigth ) ;
        if ( u8BarCode_Status == BARCODE_OK )
        {
            if ( (u32BARCODEWeigth > ParametersStr.Barcode_Max) || (u32BARCODEWeigth < MIN_WEIGTH))
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_BARCODE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MAX_WEIGHT_V , ParametersStr.Barcode_Max, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );

                EF_void_TimerStart ( ERROR_OPERATION_TIMER_ID );
                gu8OperSwitchStatus = MENU_WITHOUT_VALUES_STATE;
            }
            else
            {
                gu8OperSwitchStatus = CALCULAION_STATE;
            }
        }

#ifdef USING_PROXIMITY
        if (  (EF_BOOLEAN_IOCard_ReadPhotoCell() != FOUND_PACKAGE)   )
        {
            EF_void_PrintMenus_DisplayPhoto ( ERROR_PROXIMITY_PHOTO );
            EF_void_TimerStart    ( ERROR_OPERATION_TIMER_ID     );
            gu8OperSwitchStatus = MENU_WITHOUT_VALUES_STATE;
        }
#else

#endif
        break;

    case GET_SCALE_STATE :
        /* if timer error is out and tries < 3 , print operation and try to get the value */
        if ( EF_BOOLEAN_TimerCheck (GET_OPER_VALUES_TIMER_ID) == TRUE)
        {
            EF_void_Timer_TurnOff ( GET_OPER_VALUES_TIMER_ID );
            gu8OperSwitchStatus = MENU_WITHOUT_VALUES_STATE;
        }
        /* if timer get barcode is out,print error for 1 sec and stop Start Timer */
        else if ( EF_u32_TimerGetCounterValue(GET_OPER_VALUES_TIMER_ID) >= u32TimerCompareValue )
        {
            u32TimerCompareValue      = GET_OPER_VALUES_TIMEOUT*4 ;

            if ( (EF_u8_TimerIsEnabled ( ERROR_OPERATION_TIMER_ID ) == FALSE) )
            {
#ifdef USING_PROXIMITY

            if (EF_BOOLEAN_IOCard_ReadPhotoCell() != FOUND_PACKAGE)
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_PROXIMITY_PHOTO);
            }
            else
#endif
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_STABLE_SCALE_PHOTO);
            }

            }
            else
            {
                if (EF_BOOLEAN_TimerCheck ( ERROR_OPERATION_TIMER_ID ) == TRUE)
                {
                    EF_void_Timer_TurnOff ( ERROR_OPERATION_TIMER_ID );
                }
            }
        }


#ifdef USING_PROXIMITY
        if (EF_BOOLEAN_IOCard_ReadPhotoCell() == FOUND_PACKAGE)
        {
#ifdef STUB_SCALE
        u8ScaleStableStatus = TRUE;
        u32ScaleWeigth      = 5000;
#else
        u8ScaleStableStatus = EF_BOOLEAN_Scale_Get_Stable_Weight ( &u32ScaleWeigth , &bScaleError );
#endif            //todo : return or what ?
        }
        else
        {
            u8ScaleStableStatus = SCALE_NOT_STABLE;
        }
#else
        u8ScaleStableStatus = EF_BOOLEAN_Scale_Get_Stable_Weight ( &u32ScaleWeigth , &bScaleError );
#endif
        if ( u8ScaleStableStatus == SCALE_STABLE )
        {
            if ( (u32ScaleWeigth > ParametersStr.Scale_Max) || (u32ScaleWeigth < MIN_WEIGTH))
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_SCALE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MAX_WEIGHT_V , ParametersStr.Scale_Max, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );

                EF_void_TimerStart    ( ERROR_OPERATION_TIMER_ID     );
            }
            else
            {
                gu8OperSwitchStatus = CALCULAION_STATE;
            }
        }
        break;

   /* this state is used to decide the result and print Operation Menu with Values */
    case CALCULAION_STATE:
        EF_void_Timer_TurnOff ( GET_OPER_VALUES_TIMER_ID );

/************************************************************************************
 ************************ Get CAT. Number and ***************************************/

#ifndef USING_BARCODE
        if ( abs(u32ScaleWeigth - SCALE_STANDARD_WEIGHT) <= MIN_TOL_VALUE )
        {
            b_isAcceptPackage = ACCEPT_PACKET;
        }
        else
        {
            b_isAcceptPackage = UNDERLOAD_PACKET;

        }
#else
        /* get BarCode weigth Category */
 #ifdef SECOND_METHOD_CALCULATION
         /* from 800  to 1000 gram ===> Cat#2
          * From 1000 to 2000 gram ===> Cat#3 ,and so on
          * * */
         // compare with the Lowest tolerance , if ok -- print accept without any searching
         if ( abs(u32ScaleWeigth - u32BARCODEWeigth) <= MIN_TOL_VALUE)
         {
             AnalysisSystem.CorrectNum++;
             AnalysisSystem.CorrectWeight = AnalysisSystem.CorrectWeight + u32ScaleWeigth;
             b_isAcceptPackage = ACCEPT_PACKET;
             EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.CorrectNum    , CORRECT_PACKAGE_ADD_EEPROM , 4);
             EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.CorrectWeight   , CORRECT_WEIGHT_ADD_EEPROM , 4);

             if (gu8PatchAnalysisFlag == TRUE)
             {
                 AnalysisPatch.CorrectNum++;
                 AnalysisPatch.CorrectWeight = AnalysisPatch.CorrectWeight + u32ScaleWeigth;
                 EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.CorrectNum    , PATCH_CORRECT_NUM_EEPROM , 4);
                 EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.CorrectWeight , PATCH_CORRECT_WEIGHT_EEPROM , 4);
             }
         }
         else
         {
             for (u8Counter = 1 ; u8Counter <= gU32MaxCatogriesNumber ; u8Counter++)  //todo make max CATEGORIES numbers variable .. edited by user.
             {
                 CategoryWeight = CategoryStruct[u8Counter].CategoryWeight;
                 if (u32BARCODEWeigth <= CategoryWeight)
                 {
                     break;
                 }
             }
     #else
             /* from 500  to 1000 gram ===> Cat#1
              * From 1000 to 2000 gram ===> Cat#2 ,and so on
              * * */
             for (u8Counter = 1 ; u8Counter <= gU32MaxCatogriesNumber ; u8Counter++)
             {
                 CategoryWeight = CategoryStruct[u8Counter].CategoryWeight;

                 if ( u32BARCODEWeigth > CategoryWeight )
                 {
                     CategoryWeight = CategoryStruct[u8Counter+1].CategoryWeight;
                     if ( u32BARCODEWeigth <= CategoryWeight )
                     {
                         break;
                     }
                 }
                 else
                 {
                     break;
                     /* this occures when barcode was < 800 */
                 }
             }
     #endif

             s32Difference = u32BARCODEWeigth - u32ScaleWeigth;     /* Get the Difference to check for Tolerance to define Accept or not */
             if (s32Difference > 0)
             {
                 /* Check the Negative Tolerance as Scale < BarCode, get the NegTol from gloal struct */
                 u32Tol_NegativeValue = CategoryStruct[u8Counter].CategoryTol_Neg;

                 if ( u32Tol_NegativeValue >= s32Difference)
                 {
                     AnalysisSystem.CorrectNum++;
                     AnalysisSystem.CorrectWeight = AnalysisSystem.CorrectWeight + u32ScaleWeigth;
                     b_isAcceptPackage = ACCEPT_PACKET;
                     EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.CorrectNum    , CORRECT_PACKAGE_ADD_EEPROM , 4);
                     EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.CorrectWeight   , CORRECT_WEIGHT_ADD_EEPROM , 4);

                     if (gu8PatchAnalysisFlag == TRUE)
                     {
                         AnalysisPatch.CorrectNum++;
                         AnalysisPatch.CorrectWeight = AnalysisPatch.CorrectWeight + u32ScaleWeigth;
                         EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.CorrectNum    , PATCH_CORRECT_NUM_EEPROM , 4);
                         EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.CorrectWeight , PATCH_CORRECT_WEIGHT_EEPROM , 4);
                     }
                 }
                 else
                 {
                     AnalysisSystem.UnderloadsNum++;
                     AnalysisSystem.UnderloadsWeight = AnalysisSystem.UnderloadsWeight  + u32ScaleWeigth;
                     b_isAcceptPackage = UNDERLOAD_PACKET;
                     EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.UnderloadsNum, UNDERLOAD_PACKAGE_ADD_EEPROM, 4);
                     EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.UnderloadsWeight , UNDERLOAD_WEIGHT_ADD_EEPROM, 4);
                     if (gu8PatchAnalysisFlag == TRUE)
                     {
                         AnalysisPatch.UnderloadsNum++;
                         AnalysisPatch.UnderloadsWeight = AnalysisPatch.UnderloadsWeight  + u32ScaleWeigth;
                         EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.UnderloadsNum, PATCH_UNDERLOAD_PACKAGE_EEPROM, 4);
                         EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.UnderloadsWeight , PATCH_UNDERLOAD_WEIGHT_EEPROM, 4);
                     }
                 }
             }
             else
             {
                 s32Difference = u32ScaleWeigth - u32BARCODEWeigth ;   /* here the s32Difference is - . So make it Positive */
                 /* Check the Positive Tolerance as Scale > BarCode, get the PositiveTol from it's Place in EEPROM */
                 u32Tol_PositiveValue = CategoryStruct[u8Counter].CategoryTol_Pos;

                 if ( u32Tol_PositiveValue >= s32Difference)
                 {
                     AnalysisSystem.CorrectNum++;
                     AnalysisSystem.CorrectWeight = AnalysisSystem.CorrectWeight + u32ScaleWeigth;
                     b_isAcceptPackage = ACCEPT_PACKET;
                     EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.CorrectNum    , CORRECT_PACKAGE_ADD_EEPROM , 4);
                     EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.CorrectWeight   , CORRECT_WEIGHT_ADD_EEPROM , 4);
                     if (gu8PatchAnalysisFlag == TRUE)
                     {
                         AnalysisPatch.CorrectNum++;
                         AnalysisPatch.CorrectWeight = AnalysisPatch.CorrectWeight + u32ScaleWeigth;
                         EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.CorrectNum    , PATCH_CORRECT_NUM_EEPROM , 4);
                         EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.CorrectWeight , PATCH_CORRECT_WEIGHT_EEPROM , 4);
                     }
                 }
                 else
                 {
                     /* OVERLOAD */
                     AnalysisSystem.OverLoadNum++;
                     AnalysisSystem.OverLoadsWeight = AnalysisSystem.OverLoadsWeight + u32ScaleWeigth;
                     b_isAcceptPackage = OVERLOAD_PACKET;
                     EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.OverLoadNum  , OVERLOAD_PACKAGE_ADD_EEPROM, 4);
                     EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.OverLoadsWeight , OVERLOAD_WEIGHT_ADD_EEPROM, 4);
                     if (gu8PatchAnalysisFlag == TRUE)
                     {
                         AnalysisPatch.OverLoadNum++;
                         AnalysisPatch.OverLoadsWeight = AnalysisPatch.OverLoadsWeight + u32ScaleWeigth;
                         EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.OverLoadNum     , PATCH_OVERLOAD_PACKAGE_EEPROM, 4);
                         EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.OverLoadsWeight , PATCH_OVERLOAD_WEIGHT_EEPROM, 4);
                     }
                 }
             }
         }
#endif


        /* print them with Result */
        EF_void_PrintOperation (u32BARCODEWeigth,  u32ScaleWeigth,  b_isAcceptPackage );
#ifndef USING_PROXIMITY
        EF_void_TimerStart    ( NEW_OPERATION_TIMER_ID );       /* to go to new operation after defined time */
#endif
        gu8OperSwitchStatus = WAITING_NEW_OPERATION;            /* goto new state to enter New operation if Proximity show no thing */
        break;


    case WAITING_NEW_OPERATION:
#ifdef USING_PROXIMITY
        if (EF_BOOLEAN_IOCard_ReadPhotoCell() == NOT_FOUND_PACKAGE)
        {
            gu8OperSwitchStatus = MENU_WITHOUT_VALUES_STATE;   /* enter New operation if Proximity show no thing */
        }
#else
        if ( EF_BOOLEAN_TimerCheck (NEW_OPERATION_TIMER_ID) == TRUE)
        {
            EF_void_Timer_TurnOff ( NEW_OPERATION_TIMER_ID );
            gu8OperSwitchStatus = MENU_WITHOUT_VALUES_STATE;
        }
#endif
        break;

    default:
        break;
    }
}


/*********************************************************************************/
/* ---------------------------API Functions--------------------------------------*/
/*********************************************************************************/

/*********************************************************************
 * Function    : void EF_void_UI_Init ();
 *
 * DESCRIPTION : This function used to Initialize Lcd,KeyPad and EEPROM
 *
 * PARAMETERS  : Void.
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_UserInterface_Init ()
{
    U32_t RtcData = 0;
    U32_t RtcTime = 0;

    gU32MaxCatogriesNumber = MAX_CATEGORY_NUMBER; // todo make it configured by user and save in EEPROM and keep MAX_CATEGORY_NUMBER as a S.W limit for user

    /* Enable Timer Module */
    EF_void_TimerInit();

    /* Enable LCD */
    EF_void_UART_Init( &uart_init_cfg);

    /* Enable UART Dubegger if need it (it was with UART power)*/
    void_InitConsole();

    /* Enable interal EEPROM to save Weigth Tolerance and Correct&wrong Packages */
    EF_BOOLEAN_EEPROM_Init();

    /* Enable Scale UART without Interrupt */
    EF_BOOLEAN_Scale_Init(FALSE);

    /* There are variabled init. at First Version Only, So check the FirstRelease_EEPROM Flag to int. them or not */
    EF_BOOLEAN_EEPROM_ReadNBytes ( &u32FirstRelease_EEPROM  , FIRST_RELEASE_FLAG_EEPROM_ADD , 4);

    if (u32FirstRelease_EEPROM != FIRST_RELEASE_FLAG_VALUE)
    {
        /* Save 100 in it noto enter here again */
        u32FirstRelease_EEPROM = FIRST_RELEASE_FLAG_VALUE;
        EF_BOOLEAN_EEPROM_WriteNBytes ( &u32FirstRelease_EEPROM  , FIRST_RELEASE_FLAG_EEPROM_ADD , 4);

        u32FirstRelease_EEPROM = FIRST_RELEASE_FLAG_VALUE +1; /* need it in get rtc to be a flag for start time of system */

        /* Write EEPROM First Release only for Weigth Tolerance */
        EF_v_WriteCatDetailsInEEPROM();

        /* init the Package Numbers and weight for Analysis in their places in EEPROM with Value = 0 */
        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.CorrectNum   , CORRECT_PACKAGE_ADD_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.OverLoadNum  , OVERLOAD_PACKAGE_ADD_EEPROM,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.UnderloadsNum , UNDERLOAD_PACKAGE_ADD_EEPROM, 4);

        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.CorrectWeight  , CORRECT_WEIGHT_ADD_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.UnderloadsWeight, UNDERLOAD_WEIGHT_ADD_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisSystem.OverLoadsWeight , OVERLOAD_WEIGHT_ADD_EEPROM, 4);


        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.CorrectNum    , PATCH_CORRECT_NUM_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.OverLoadNum   , PATCH_UNDERLOAD_PACKAGE_EEPROM,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.UnderloadsNum , PATCH_OVERLOAD_PACKAGE_EEPROM, 4);

        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.CorrectWeight   , PATCH_CORRECT_WEIGHT_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.UnderloadsWeight, PATCH_UNDERLOAD_WEIGHT_EEPROM , 4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &AnalysisPatch.OverLoadsWeight , PATCH_OVERLOAD_WEIGHT_EEPROM, 4);

        /* Make gCounterExpired_3Months =0 to start timeout after 3Months to disable Operation if 3Months passed */
        gU32CounterExpired_3Months = 0;
        EF_BOOLEAN_EEPROM_WriteNBytes ( &gU32CounterExpired_3Months  , TIMER_EXPIRED_3Months_EEPROM_ADD , 4);

        EF_BOOLEAN_EEPROM_WriteNBytes( &ParametersStr.Password  ,  DEFAULT_PASSWORD, 4);

        EF_void_PrintMenus_GetRtc ();  /* get calender for the first time  to compare with it after that */
    }
    else
    {
        EF_BOOLEAN_EEPROM_ReadNBytes ( &AnalysisSystem.CorrectNum   , CORRECT_PACKAGE_ADD_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_ReadNBytes ( &AnalysisSystem.OverLoadNum  , OVERLOAD_PACKAGE_ADD_EEPROM,  4);
        EF_BOOLEAN_EEPROM_ReadNBytes ( &AnalysisSystem.UnderloadsNum , UNDERLOAD_PACKAGE_ADD_EEPROM, 4);

        EF_BOOLEAN_EEPROM_ReadNBytes ( &AnalysisSystem.CorrectWeight  , CORRECT_WEIGHT_ADD_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_ReadNBytes ( &AnalysisSystem.UnderloadsWeight, UNDERLOAD_WEIGHT_ADD_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_ReadNBytes ( &AnalysisSystem.OverLoadsWeight , OVERLOAD_WEIGHT_ADD_EEPROM, 4);

        EF_BOOLEAN_EEPROM_ReadNBytes  ( &RtcData , START_DATE_ADD ,  4); /* 0, Year, Month, Day */
        EF_BOOLEAN_EEPROM_ReadNBytes  ( &RtcTime , START_TIME_ADD ,  4);/* 0,    0, hour , Minutes */
        Time_StartAnalysis.Year = (U8_t)(RtcData>>16) ;
        Time_StartAnalysis.Month = (U8_t)(RtcData>>8) ;
        Time_StartAnalysis.Day = (U8_t)(RtcData) ;
        Time_StartAnalysis.Hour = (U8_t)(RtcTime>>8) ;
        Time_StartAnalysis.Minutes = (U8_t)(RtcTime) ;
    }

    EF_v_GetCatDetalisFromEEPROM  ();

    EF_BOOLEAN_EEPROM_ReadNBytes ( &ParametersStr.Password  ,  PASSWORD_EEPROM, 4);

#ifdef USING_PROXIMITY
    EF_BOOLEAN_IOCard_Init();
#endif
    /* Bar-Code Scanner */
    EF_BOOLEAN_BarCodeScanner_Init();

    EF_void_TimerCreate ( GET_ALL_FRAME_TIMER_ID   , GET_ALL_FRAME_TIMEOUT   );
    EF_void_TimerCreate ( SAVING_TOL_TIMER_ID      , DISPLAY_ERROR_TIMEOUT   );
    EF_void_TimerCreate ( ERROR_PHOTO_TIMER_ID     , DISPLAY_ERROR_TIMEOUT   );
    EF_void_TimerCreate ( GET_OPER_VALUES_TIMER_ID , GET_OPER_VALUES_TIMEOUT );
    EF_void_TimerCreate ( NEW_OPERATION_TIMER_ID   , NEW_OPERATION_TIMEOUT   );
    EF_void_TimerCreate ( ANALYSIS_PRINT_TIMER_ID  , ANALYSIS_PRINT_TIMEOUT  );
    EF_void_TimerCreate ( ERROR_OPERATION_TIMER_ID  , OPERATION_ERROR_TIMEOUT  );

}

/*********************************************************************
 * Function    : EF_v_UI_SystemStates ()
 *
 * DESCRIPTION : System main Function to parse the received data and serve it.
 *
 * PARAMETERS  : None.
 *
 * Return Value: None.
 ***********************************************************************/
void  EF_v_UI_SystemStates ()
{
    static U32_t u32CategoryWeighT   = 0;
    static U32_t SystemTime          = 0;
    static U32_t u32NegativeValue    = 0;
    static U32_t u32PositiveValue    = 0;
    static U32_t u32BarCode_Max      = 0;
    static U32_t u32Tolerance_Max    = 0;
    static U32_t u32Scale_Max        = 0;
           U32_t u32CatStatus        = 0;
    static U32_t u32PramCatNumber    = MIN_CATEGORY_NUMBER;
    static U32_t u32TolerCatNumber   = 0;
    static U32_t u32NewPassword      = 0;
    static U32_t u32OldPassword      = 0;
    static U8_t u32ParsingData[10]   = {0};                     /* get the parsing data of the received frame from it */
    static U16_t u16CurrentState     = 0;                       /* save the switch case */
    static U8_t  u8EditToleranceFlag = PREPARE_TO_EDIT;         /* define the state of Get data of Edit Tolerance Menu  */
    static U8_t  u8CatWeightFlag     = PREPARE_TO_EDIT;         /* define the state of Get data of Edit Tolerance Menu  */
    static U8_t  u8ChangePassFlag    = PREPARE_TO_EDIT;         /* define the state of Get data of Edit Tolerance Menu  */
    static U8_t  u8AnalysisFlag      = FALSE;
    static U8_t  u8ParsingDataLength = 0;
    static U8_t  u8AnaylsisPrint     = ANALYSIS_PRINT_NUMBERS;  /* to toggle between Analysis Cat. Numbers and Cat. weights  */
           U32_t RtcData = 0;
           U32_t RtcTime = 0 ;

    /* if the Rx Buffer has bytes > min_Frame , get the data and parse it ,to ensure the frame was completed */
    if ( getAvailableDataCountOnUART() >= SCREEN_MIN_FRAME_LENGTH)
    {
        u16CurrentState = EF_u16_UI_ParseUartBuffer ( (U8_t *)u32ParsingData, &u8ParsingDataLength);
    }

    switch (u16CurrentState)
    {
    case ESCAPE_STATE :
        /* don't do any thing */
        break;

    case START_OPERATION_BUTTON :
        EF_v_UI_OperationTask();
        break;

    case STOP_OPERATION_BUTTON :
        EF_void_PrintMenus_DisplayPhoto ( START_OPERATION_PHOTO );
        EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID );
        gu8OperSwitchStatus  = MENU_WITHOUT_VALUES_STATE;
        u16CurrentState      = ESCAPE_STATE;
        break;

   /*----------- Analysis ------------*/
    case ANALYSIS_STATRT_BUTTON:
        gu8PatchAnalysisFlag = TRUE;
        u16CurrentState      = ANALYSIS_PATCH_BUTTON;
        break;

    case ANALYSIS_STOP_BUTTON:
        gu8PatchAnalysisFlag = FALSE;
        u16CurrentState      = ANALYSIS_PATCH_BUTTON;
        break;

    case ANALYSIS_PATCH_BUTTON :
        /* using timer to toggle  between Analysis Cat. Numbers and Cat. weights */
        if  ( EF_u8_TimerIsEnabled (ANALYSIS_PRINT_TIMER_ID) ==TRUE)
        {
            if (EF_BOOLEAN_Timer_IsTimedOut(ANALYSIS_PRINT_TIMER_ID)  == TRUE)
            {
                EF_void_Timer_TurnOff (ANALYSIS_PRINT_TIMER_ID);
            }
        }
        else
        {
            if (u8AnaylsisPrint == ANALYSIS_PRINT_NUMBERS)
            {
                u8AnaylsisPrint = ANALYSIS_PRINT_WEIGHTS;

                EF_void_PrintMenus_SendValue ( CORRECT_PACKETS_NUM_V   , AnalysisPatch.CorrectNum   , TRUE);
                EF_void_PrintMenus_SendValue ( OVERLOAD_PACKETS_NUM_V  , AnalysisPatch.OverLoadNum  , TRUE);
                EF_void_PrintMenus_SendValue ( UNDERLOAD_PACKETS_NUM_V , AnalysisPatch.UnderloadsNum , TRUE);
            }
            else
            {
                EF_void_PrintMenus_SendValue ( CORRECT_PACKETS_NUM_V   , AnalysisPatch.CorrectWeight   , TRUE);
                EF_void_PrintMenus_SendValue ( UNDERLOAD_PACKETS_NUM_V , AnalysisPatch.UnderloadsWeight , TRUE);
                EF_void_PrintMenus_SendValue ( OVERLOAD_PACKETS_NUM_V  , AnalysisPatch.OverLoadsWeight  , TRUE);

                u8AnaylsisPrint = ANALYSIS_PRINT_NUMBERS;
            }
            EF_void_PrintMenus_DisplayPhoto ( ANALYSIS_PATCH );
            EF_void_TimerStart (ANALYSIS_PRINT_TIMER_ID);
        }
        break;


    case ANALYSIS_SYSTEM_BUTTON :
        /* using timer to toggle  between Analysis Cat. Numbers and Cat. weights */
        if  ( EF_u8_TimerIsEnabled (ANALYSIS_PRINT_TIMER_ID) ==TRUE)
        {
            if (EF_BOOLEAN_Timer_IsTimedOut(ANALYSIS_PRINT_TIMER_ID)  == TRUE)
            {
                EF_void_Timer_TurnOff (ANALYSIS_PRINT_TIMER_ID);
            }
        }
        else
        {
            if (u8AnaylsisPrint == ANALYSIS_PRINT_NUMBERS)
            {
                u8AnaylsisPrint = ANALYSIS_PRINT_WEIGHTS;

                EF_void_PrintMenus_SendValue ( CORRECT_PACKETS_NUM_V   , AnalysisSystem.CorrectNum   , TRUE);
                EF_void_PrintMenus_SendValue ( OVERLOAD_PACKETS_NUM_V  , AnalysisSystem.OverLoadNum  , TRUE);
                EF_void_PrintMenus_SendValue ( UNDERLOAD_PACKETS_NUM_V , AnalysisSystem.UnderloadsNum , TRUE);

                /* print hours and minutes */
                EF_void_PrintMenus_SendValue ( ANALYSIS_HOURS_V   , SystemTime/60 , TRUE);
                EF_void_PrintMenus_SendValue ( ANALYSIS_MINUTES_V , SystemTime - (SystemTime/60)*60 , TRUE);
            }
            else
            {
                EF_void_PrintMenus_SendValue ( CORRECT_PACKETS_NUM_V   , AnalysisSystem.CorrectWeight   , TRUE);
                EF_void_PrintMenus_SendValue ( UNDERLOAD_PACKETS_NUM_V , AnalysisSystem.UnderloadsWeight , TRUE);
                EF_void_PrintMenus_SendValue ( OVERLOAD_PACKETS_NUM_V  , AnalysisSystem.OverLoadsWeight  , TRUE);

                u8AnaylsisPrint = ANALYSIS_PRINT_NUMBERS;
            }
            EF_void_PrintMenus_DisplayPhoto ( ANALYSIS_SYSTEM );

            EF_void_TimerStart (ANALYSIS_PRINT_TIMER_ID);
        }
        break;


    case SELECT_ANALYSIS_BUTTON:
        u8AnalysisFlag = TRUE;
        EF_void_PrintMenus_GetRtc ();
        u16CurrentState = ESCAPE_STATE;
        break;

    case GET_RTC_VALUES :
        /* return Array[] = {year, month , day ,00 ,hour, minutes, Seconds}*/
        if (u32FirstRelease_EEPROM != FIRST_RELEASE_FLAG_VALUE)
        {
            u32FirstRelease_EEPROM = FIRST_RELEASE_FLAG_VALUE;
            Time_StartAnalysis.Year    = u32ParsingData[YEAR_ELEMENT];
            Time_StartAnalysis.Month   = u32ParsingData[MONTH_ELEMENT];
            Time_StartAnalysis.Day     = u32ParsingData[DAY_ELEMENT];
            Time_StartAnalysis.Hour    = u32ParsingData[HOUR_ELEMENT];
            Time_StartAnalysis.Minutes = u32ParsingData[MINUTE_ELEMENT];

            RtcData = (Time_StartAnalysis.Year << 16) + (Time_StartAnalysis.Month << 8) + (Time_StartAnalysis.Day);
            RtcTime = (Time_StartAnalysis.Hour << 8)  + (Time_StartAnalysis.Minutes);

            EF_BOOLEAN_EEPROM_WriteNBytes  ( &RtcData , START_DATE_ADD ,  4); /* 0, Year, Month, Day */
            EF_BOOLEAN_EEPROM_WriteNBytes  ( &RtcTime , START_TIME_ADD ,  4);/* 0,    0, hour , Minutes */
            u16CurrentState = ESCAPE_STATE;
        }
        else
        {
            Time_TillNowAnalysis.Year  = u32ParsingData[YEAR_ELEMENT];
            Time_TillNowAnalysis.Month = u32ParsingData[MONTH_ELEMENT];
            Time_TillNowAnalysis.Day   = u32ParsingData[DAY_ELEMENT];
            Time_TillNowAnalysis.Hour  = u32ParsingData[HOUR_ELEMENT];
            Time_TillNowAnalysis.Minutes = u32ParsingData[MINUTE_ELEMENT];

            SystemTime = EF_v_ConvertRTC(Time_TillNowAnalysis ) - EF_v_ConvertRTC(Time_StartAnalysis);
            EF_void_PrintMenus_DisplayPhoto  ( ENTER_PASSWORD_PHOTO );
            u16CurrentState = ESCAPE_STATE;
        }
        break;

    case CLEAR_ANALYSIS_BUTTON:
        AnalysisPatch.CorrectNum       = 0;
        AnalysisPatch.CorrectWeight    = 0;
        AnalysisPatch.OverLoadNum      = 0;
        AnalysisPatch.OverLoadsWeight  = 0;
        AnalysisPatch.UnderloadsNum    = 0;
        AnalysisPatch.UnderloadsWeight = 0;

        EF_void_PrintMenus_SendValue    ( CORRECT_PACKETS_NUM_V   , 0   , TRUE);
        EF_void_PrintMenus_SendValue    ( OVERLOAD_PACKETS_NUM_V  , 0  , TRUE);
        EF_void_PrintMenus_SendValue    ( UNDERLOAD_PACKETS_NUM_V , 0 , TRUE);

        /* rewrite the Package Numbers in their places in EEPROM with Value = 0 to clear */
        EF_BOOLEAN_EEPROM_WriteNBytes   ( &AnalysisPatch.CorrectNum  , CORRECT_PACKAGE_ADD_EEPROM , 4);
        EF_BOOLEAN_EEPROM_WriteNBytes   ( &AnalysisPatch.OverLoadNum , OVERLOAD_PACKAGE_ADD_EEPROM  , 4);
        EF_BOOLEAN_EEPROM_WriteNBytes   ( &AnalysisPatch.UnderloadsNum, UNDERLOAD_PACKAGE_ADD_EEPROM , 4);

        EF_BOOLEAN_EEPROM_WriteNBytes   ( &AnalysisPatch.CorrectWeight  , CORRECT_WEIGHT_ADD_EEPROM   ,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes   ( &AnalysisPatch.UnderloadsWeight, UNDERLOAD_WEIGHT_ADD_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes   ( &AnalysisPatch.OverLoadsWeight , OVERLOAD_WEIGHT_ADD_EEPROM  , 4);

        EF_void_Timer_TurnOff (ANALYSIS_PRINT_TIMER_ID);
        u16CurrentState = ANALYSIS_PATCH_BUTTON;
        u8AnaylsisPrint = ANALYSIS_PRINT_NUMBERS;
        break;

    case BACK_FROM_ANALYSIS_BUTTON :
        EF_void_Timer_TurnOff (ANALYSIS_PRINT_TIMER_ID);
        u8AnaylsisPrint = ANALYSIS_PRINT_NUMBERS;
        EF_void_PrintMenus_DisplayPhoto  ( ANALYSIS_CHOOSE );
        u16CurrentState = ESCAPE_STATE;
        break;

        /*----------- Tolerance ------------*/
    case SELECT_EDIT_BUTTON:
        /* if time is out , Stop Timer, timer is used for ensuring printitng the save Photo for it's time */
        if (EF_u8_TimerIsEnabled (SAVING_TOL_TIMER_ID) == FALSE )
        {
            EF_void_PrintMenus_DisplayPhoto  ( ENTER_EDIT_CAT_NUM_PHOTO );
            EF_void_PrintMenus_SendValue     ( ENTER_EDIT_CAT_NUM_V   , 0 , TRUE);
            u16CurrentState = ESCAPE_STATE;
        }
        else if ( EF_BOOLEAN_TimerCheck (SAVING_TOL_TIMER_ID) ==TRUE)
        {
            EF_void_Timer_TurnOff  ( SAVING_TOL_TIMER_ID);
        }
        break;

    case SELECT_SHOW_BUTTON:
        EF_void_PrintMenus_DisplayPhoto  ( ENTER_SHOW_CAT_NUM_PHOTO );
        EF_void_PrintMenus_SendValue     ( ENTER_SHOW_CAT_NUM_V   , 0 , TRUE);

        u16CurrentState = ESCAPE_STATE;
        break;

    case SHOW_LIST_HIGH_BUTTON:
        if (u32TolerCatNumber < MAX_CATEGORY_NUMBER)
        {
            u32TolerCatNumber ++;
        }
        * (U32_t*)u32ParsingData  = u32TolerCatNumber;
        u16CurrentState = ENTER_SHOW_CAT_NUM_V;
        break;

    case SHOW_LIST_LOW_BUTTON:
        if (u32TolerCatNumber > MIN_CATEGORY_NUMBER)
        {
            u32TolerCatNumber --;
        }
        * (U32_t*)u32ParsingData   = u32TolerCatNumber;
        u16CurrentState = ENTER_SHOW_CAT_NUM_V;
        break;

    case EDIT_LIST_HIGH_BUTTON:
        u32TolerCatNumber ++;
        * (U32_t*)u32ParsingData   = u32TolerCatNumber;
        u16CurrentState = ENTER_EDIT_CAT_NUM_V;
        break;

    case EDIT_LIST_LOW_BUTTON:
        u32TolerCatNumber --;
        * (U32_t*)u32ParsingData   = u32TolerCatNumber;
        u16CurrentState = ENTER_EDIT_CAT_NUM_V;
        break;

    case CONFIRM_EDIT_BUTTON:
        /* if pressed confirm Edit button , get the Entered category Number */
        EF_void_PrintMenus_RequestValue ( ENTER_EDIT_CAT_NUM_V, TRUE);
        u32TolerCatNumber  = 0;
        u8EditToleranceFlag = PREPARE_TO_EDIT;
        u16CurrentState     = ESCAPE_STATE;
        break;

    case CONFIRM_SHOW_BUTTON:
        /* if pressed ENTER_SHOW_CAT_NUM_V button , get the Entered category Number */
        EF_void_PrintMenus_RequestValue (ENTER_SHOW_CAT_NUM_V, TRUE);
        u16CurrentState = ESCAPE_STATE;
        break;


    case SAVE_TOLERANACE_BUTTON:
        /* check Category Number  - if error : print Error for 1 sec  then goto the same state again
         *                        - if ok    : Print Data saved , Return to SELECT_EDIT_BUTTON
         * */
           if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
           {
               if (u8EditToleranceFlag == SECOND_EDITING)
               {
                   u8EditToleranceFlag = PREPARE_TO_EDIT;

                   if ( (u32PositiveValue > ParametersStr.Tolerance_Max) || (u32NegativeValue > ParametersStr.Tolerance_Max) )
                   {
                       EF_void_PrintMenus_DisplayPhoto  ( ERROR_MAX_TOLERANCE_PHOTO );
                       EF_void_PrintMenus_SendValue     ( ERROR_MAX_TOLERANCE_V , ParametersStr.Tolerance_Max, TRUE );   //todo : add anther max tol if there is condition

                       EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
                   }
                   else
                   {
                       /* save in Array to get tolerance fastly in Operation mode */
                       CategoryStruct[u32TolerCatNumber].CategoryTol_Neg = u32NegativeValue;
                       CategoryStruct[u32TolerCatNumber].CategoryTol_Pos = u32PositiveValue;

                       /* save in EEPROM to Premenatly saving it */
                       EF_BOOLEAN_EEPROM_WriteNBytes ( &u32PositiveValue  ,  CATEGORIES_ADD_EEPROM + 4*4*(u32TolerCatNumber-1) + 1*4 , 4);
                       EF_BOOLEAN_EEPROM_WriteNBytes ( &u32NegativeValue  ,  CATEGORIES_ADD_EEPROM + 4*4*(u32TolerCatNumber-1) + 2*4 , 4);

                       EF_void_PrintMenus_DisplayPhoto  ( DATA_IS_SAVED_PHOTO );
                       /* start timer to print save photo */
                       EF_void_TimerStart (SAVING_TOL_TIMER_ID);

                       * (U32_t*)u32ParsingData   = u32TolerCatNumber;
                       u16CurrentState = ENTER_EDIT_CAT_NUM_V;
                   }
               }
               else
               {
                   /* if Saved Button is pressed,  request the tolerance Values to check then saveing them if vaild*/
                   EF_void_PrintMenus_RequestValue (EDIT_POSITIVE_TOLERANCE_V, TRUE);
                   EF_void_PrintMenus_RequestValue (EDIT_NEGATIVE_TOLERANCE_V, TRUE);
                   u16CurrentState     = ESCAPE_STATE;
               }
           }
           else
           {
               /* if time is out , Stop Timer */
               if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
               {
                   EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID);
                   u32CategoryWeighT  = CategoryStruct[u32TolerCatNumber].CategoryWeight;
                   /* After Error Photo is finished, Print Edit tolerance Menu again */
                   EF_void_PrintMenus_CalEditPosNeg (u32TolerCatNumber , u32CategoryWeighT);
                   u32NegativeValue = 0;
                   u32PositiveValue = 0;

                   /* Return to Pasre State, The Expected : Save (after_adding _Values_or_not) and Back */
                   u16CurrentState     = ESCAPE_STATE;
               }
           }
        break;

    case BACK_FROM_EDIT_BUTTON:
        u32NegativeValue = 0;
        u32PositiveValue = 0;

        EF_void_PrintMenus_DisplayPhoto  ( ERROR_UNSAVED_DATA_PHOTO );
        EF_void_TimerStart (SAVING_TOL_TIMER_ID);

        /* Return to Pasre State */
        u16CurrentState     = SELECT_EDIT_BUTTON;
        break;

    case BACK_FROM_SHOW_BUTTON:
        u16CurrentState     = SELECT_SHOW_BUTTON;
        break;

    case ENTER_SHOW_CAT_NUM_V:
        /* check Category Number  - if error : print Error for 1 sec  then Goto SELECT_SHOW_BUTTON again
          *                       - if ok    : Print Show Menu, Return to Main Parsing State
          * */
         if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
         {
             if ( (* (U32_t*)u32ParsingData  > gU32MaxCatogriesNumber) || (* (U32_t*)u32ParsingData  < MIN_CATEGORY_NUMBER))
             {
                 EF_void_PrintMenus_DisplayPhoto  ( ERROR_ENTER_CAT_NUM_PHOTO );
                 EF_void_PrintMenus_SendValue     ( ERROR_CAT_MAX_NUMBER_V, gU32MaxCatogriesNumber, TRUE );
                 EF_void_PrintMenus_SendValue     ( ERROR_CAT_MIN_NUMBER_V, MIN_CATEGORY_NUMBER, TRUE );

                 EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
             }
             else
             {
                 /* the Parsing Data from frame has Category Number */
                 u32TolerCatNumber = * (U32_t*)u32ParsingData ;
                 u32CategoryWeighT  = CategoryStruct[u32TolerCatNumber].CategoryWeight;
                 u32NegativeValue   = CategoryStruct[u32TolerCatNumber].CategoryTol_Neg;
                 u32PositiveValue   = CategoryStruct[u32TolerCatNumber].CategoryTol_Pos;

                 /* print Show Menu */
                 EF_void_PrintMenus_CalWeightShow (u32TolerCatNumber ,u32CategoryWeighT, u32PositiveValue , u32NegativeValue );
                 /* Return to Pasre Stat */
                 u16CurrentState     = ESCAPE_STATE;
             }
         }
         else
         {
             /* if time is out , Stop Timer */
             if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
             {
                 EF_void_Timer_TurnOff  ( ERROR_PHOTO_TIMER_ID);
                 u16CurrentState  = SELECT_SHOW_BUTTON;
             }
         }
        break;



    case ENTER_EDIT_CAT_NUM_V:
        /* check Category Number  - if error : print Error for 1 sec, zeroing flag then Goto Enter Category Number_ Edit again
         *                        - if ok    : Print Edit+- Menu, Send Zeros, Set Flag  and Return to Main Parsing State
         * */
        if (EF_u8_TimerIsEnabled (SAVING_TOL_TIMER_ID) == TRUE)
        {
            if (EF_BOOLEAN_TimerCheck (SAVING_TOL_TIMER_ID) == TRUE)
            {
                EF_void_Timer_TurnOff  ( SAVING_TOL_TIMER_ID);
            }
        }
        else
        {
            if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
            {
                if ( ( * (U32_t*)u32ParsingData  > gU32MaxCatogriesNumber ) || ( * (U32_t*)u32ParsingData  < MIN_CATEGORY_NUMBER ))
                {
                    EF_void_PrintMenus_SendValue     ( ERROR_CAT_MAX_NUMBER_V, gU32MaxCatogriesNumber, TRUE );
                    EF_void_PrintMenus_DisplayPhoto  ( ERROR_ENTER_CAT_NUM_PHOTO );
                    EF_void_PrintMenus_SendValue     ( ERROR_CAT_MIN_NUMBER_V, MIN_CATEGORY_NUMBER, TRUE );

                    EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
                }
                else
                {
                    /* the Parsing Data from frame has Category Number */
                    u32TolerCatNumber = * (U32_t*)u32ParsingData ;
                    u32CategoryWeighT  = CategoryStruct[u32TolerCatNumber].CategoryWeight;
                    /* Print Edit +,- Menu */
                    EF_void_PrintMenus_CalEditPosNeg (u32TolerCatNumber , u32CategoryWeighT);
                    /* initialize Pos. and Neg. Tolerance */
                    u32NegativeValue = 0;
                    u32PositiveValue = 0;

                    /* Return to Pasre State, The Expected : Save(after_adding _Values_or_not) or Back */
                    u16CurrentState     = ESCAPE_STATE;
                }
            }
            else
            {
                /* if time is out , Stop Timer */
                if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
                {
                    EF_void_Timer_TurnOff  ( ERROR_PHOTO_TIMER_ID);
                    u16CurrentState  = SELECT_EDIT_BUTTON;
                }
            }
        }
        break;


    case EDIT_POSITIVE_TOLERANCE_V:
        /* the Parsing Data from frame has the  EDIT_POSITIVE_TOLERANCE_VAlue*/
        u32PositiveValue = * (U32_t*)u32ParsingData ;

        if ( u8EditToleranceFlag == PREPARE_TO_EDIT )
        {
            u8EditToleranceFlag = FIRST_EDITING;
            u16CurrentState     = ESCAPE_STATE;

        }
        else if ( u8EditToleranceFlag == FIRST_EDITING )
        {
            u8EditToleranceFlag = SECOND_EDITING;
            u16CurrentState     = SAVE_TOLERANACE_BUTTON;
        }
        else
        {
            //for any unexpected errors
            u8EditToleranceFlag = PREPARE_TO_EDIT;
            u16CurrentState     = ESCAPE_STATE;
        }
        break;


    case EDIT_NEGATIVE_TOLERANCE_V:
        /* the Parsing Data from frame has the  EDIT_NEG_TOLERANCE_Value*/
        u32NegativeValue = * (U32_t*)u32ParsingData ;

        if ( u8EditToleranceFlag == PREPARE_TO_EDIT )
        {
            u8EditToleranceFlag = FIRST_EDITING;
            u16CurrentState     = ESCAPE_STATE;
        }
        else if ( u8EditToleranceFlag == FIRST_EDITING )
        {
            u8EditToleranceFlag = SECOND_EDITING;
            u16CurrentState     = SAVE_TOLERANACE_BUTTON;
        }
        else
        {
            //for any unexpected errors
            u8EditToleranceFlag = PREPARE_TO_EDIT;
            u16CurrentState     = ESCAPE_STATE;
        }
        break;


   /*----------- Password ------------*/
    case CONFIRM_PASSWORD_BUTTON:
        EF_void_PrintMenus_RequestValue(ENTER_PASSWORD_V, TRUE);
        u16CurrentState = ESCAPE_STATE;
        break;

    case CALIBRATION_CHOOSE_BUTTON:
        u8AnalysisFlag = FALSE;
        EF_void_PrintMenus_DisplayPhoto ( ENTER_PASSWORD_PHOTO );
        u16CurrentState = ESCAPE_STATE;
        break;

    case ENTER_PASSWORD_V:
        if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
        {
            if ( * (U32_t*)u32ParsingData  != ParametersStr.Password )
            {
                EF_void_PrintMenus_DisplayPhoto ( WRONG_PASSWORD_PHOTO );
                EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
            }
            else
            {
                if (u8AnalysisFlag == FALSE)
                {
                    EF_void_PrintMenus_DisplayPhoto ( CALIBRATION_CHOOSE_PHOTO );
                }
                else
                {
                    u8AnalysisFlag = FALSE;
                    EF_void_PrintMenus_DisplayPhoto ( ANALYSIS_CHOOSE );
                }
                u16CurrentState = ESCAPE_STATE;
            }
        }
        else
        {
            /* if time is out , Stop Timer */
            if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
            {
                EF_void_Timer_TurnOff  ( ERROR_PHOTO_TIMER_ID);
                EF_void_PrintMenus_DisplayPhoto ( ENTER_PASSWORD_PHOTO );
                u16CurrentState = ESCAPE_STATE;
            }
        }
        break;

    case CHANGE_PASSWORD_BUTTON:
        if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
        {
            EF_void_PrintMenus_DisplayPhoto ( CHANGE_PASSWORD_PHOTO );
            u16CurrentState = ESCAPE_STATE;
        }
        else
        {
            if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
            {
                EF_void_Timer_TurnOff  ( ERROR_PHOTO_TIMER_ID);
            }
        }
        break;

    case SAVE_PASSWORD_BUTTON:
        if (EF_u8_TimerIsEnabled (SAVING_TOL_TIMER_ID) == FALSE)
          {
              if (u8ChangePassFlag == SECOND_EDITING)
              {
                  u8ChangePassFlag = PREPARE_TO_EDIT;

                  if ( u32OldPassword != ParametersStr.Password )
                  {
                      EF_void_PrintMenus_DisplayPhoto ( WRONG_PASSWORD_PHOTO );
                      EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
                      u16CurrentState     = CHANGE_PASSWORD_BUTTON;

                  }
                  else
                  {
                      ParametersStr.Password = u32NewPassword;
                      /* save in EEPROM to Premenatly saving it */
                      EF_BOOLEAN_EEPROM_WriteNBytes ( &u32NewPassword  ,  PASSWORD_EEPROM, 4);

                      EF_void_PrintMenus_DisplayPhoto  ( DATA_IS_SAVED_PHOTO );
                      /* start timer to print save photo */
                      EF_void_TimerStart (SAVING_TOL_TIMER_ID);
                  }
              }
              else
              {
                  EF_void_PrintMenus_RequestValue(OLD_PASSWORD_V, TRUE);
                  EF_void_PrintMenus_RequestValue(NEW_PASSWORD_V, TRUE);
                  u16CurrentState = ESCAPE_STATE;
              }
          }
          else
          {
              /* if time is out , Stop Timer */
              if ( EF_BOOLEAN_TimerCheck ( SAVING_TOL_TIMER_ID ) == TRUE )
              {
                  EF_void_Timer_TurnOff ( SAVING_TOL_TIMER_ID);
                  if (u8AnalysisFlag == FALSE)
                  {
                      EF_void_PrintMenus_DisplayPhoto ( CALIBRATION_CHOOSE_PHOTO );
                  }
                  else
                  {
                      u8AnalysisFlag = FALSE;
                      EF_void_PrintMenus_DisplayPhoto ( ANALYSIS_CHOOSE );
                  }
                  /* Return to Pasre State, The Expected : Save (after_adding _Values_or_not) and Back */
                  u16CurrentState     = ESCAPE_STATE;
              }
          }
        break;

    case OLD_PASSWORD_V:
        u32OldPassword  = * (U32_t*)u32ParsingData ;

        if ( u8ChangePassFlag == PREPARE_TO_EDIT )
        {
            u8ChangePassFlag = FIRST_EDITING;
            u16CurrentState  = ESCAPE_STATE;
        }
        else if ( u8ChangePassFlag == FIRST_EDITING )
        {
            u8ChangePassFlag = SECOND_EDITING;
            u16CurrentState  = SAVE_PASSWORD_BUTTON;
        }
        else
        {
            u8ChangePassFlag = PREPARE_TO_EDIT;
            u16CurrentState  = ESCAPE_STATE;
        }
        break;

    case NEW_PASSWORD_V:
        u32NewPassword = * (U32_t*)u32ParsingData ;

        if ( u8ChangePassFlag == PREPARE_TO_EDIT )
        {
            u8ChangePassFlag = FIRST_EDITING;
            u16CurrentState  = ESCAPE_STATE;
        }
        else if ( u8ChangePassFlag == FIRST_EDITING )
        {
            u8ChangePassFlag = SECOND_EDITING;
            u16CurrentState  = SAVE_PASSWORD_BUTTON;
        }
        else
        {
            u8ChangePassFlag = PREPARE_TO_EDIT;
            u16CurrentState  = ESCAPE_STATE;
        }
        break;


   /*----------- Parameters ------------*/
    case PRAMETER_MAX_TOL_BUTTON:
        EF_void_PrintMenus_SendValue     ( PARAMETER_TOL_MAX_V , 0, TRUE );
        EF_void_PrintMenus_DisplayPhoto  ( PARAMETER_TOLERANCE_PHOTO );
        u16CurrentState = ESCAPE_STATE;
        break;

    case PRAM_BARCODE_LIMIT_BUTTON:
        EF_void_PrintMenus_SendValue     ( PARAMETER_BARCODE_MAX_V , 0, TRUE );
        EF_void_PrintMenus_DisplayPhoto  ( PARAMETER_BARCODE_PHOTO );
        u16CurrentState = ESCAPE_STATE;
        break;

    case PRAMETER_SCALE_LIMIT_BUTTON:
        EF_void_PrintMenus_SendValue     ( PARAMETER_SCALE_MAX_V , 0, TRUE );
        EF_void_PrintMenus_DisplayPhoto  ( PARAMETER_SCALE_PHOTO );
        u16CurrentState = ESCAPE_STATE;
        break;

    case PRAMETER_CAT_WEIGHT_BUTTON:
        if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID ) == TRUE)
        {
            if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
            {
                EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID );
            }
        }
        else
        {
            if (EF_u8_TimerIsEnabled (SAVING_TOL_TIMER_ID ) == FALSE)
            {
                if ( (u32PramCatNumber > MAX_CATEGORY_NUMBER) || ( u32PramCatNumber < MIN_CATEGORY_NUMBER ) )
                {
                    EF_void_PrintMenus_DisplayPhoto ( ERROR_ENTER_CAT_NUM_PHOTO );
                    EF_void_PrintMenus_SendValue    ( ERROR_CAT_MAX_NUMBER_V , MAX_CATEGORY_NUMBER, TRUE );
                    EF_void_PrintMenus_SendValue    ( ERROR_CAT_MIN_NUMBER_V , MIN_CATEGORY_NUMBER, TRUE );

                    EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
                    u32PramCatNumber = MIN_CATEGORY_NUMBER;
                }
                else
                {
                    EF_void_PrintMenus_SendValue     ( PARAMETER_CAT_WEIGHT_V , 0, TRUE );
                    EF_void_PrintMenus_SendValue     ( PARAMETER_CAT_NUM_V    , u32PramCatNumber, TRUE );
                    EF_void_PrintMenus_DisplayPhoto  ( PARAMETER_CAT_WEIGHT_PHOTO );
                    u8CatWeightFlag = PREPARE_TO_EDIT;
                    u16CurrentState = ESCAPE_STATE;
                }
            }
            else if ( EF_BOOLEAN_TimerCheck ( SAVING_TOL_TIMER_ID ) == TRUE )
            {
                EF_void_Timer_TurnOff ( SAVING_TOL_TIMER_ID );
            }
        }
        break;

    case SAVE_BARCODE_LIMIT_BUTTON:
        /* if Saved Button is pressed,  request the  Value to check then saveing them if vaild*/
        EF_void_PrintMenus_RequestValue (PARAMETER_BARCODE_MAX_V, TRUE);
        u16CurrentState     = ESCAPE_STATE;
        break;

    case SAVE_MAX_TOL_BUTTON:
        /* if Saved Button is pressed,  request the  Value to check then saveing them if vaild*/
        EF_void_PrintMenus_RequestValue (PARAMETER_TOL_MAX_V, TRUE);
        u16CurrentState     = ESCAPE_STATE;
        break;

    case SAVE_SCALE_LIMIT_BUTTON:
        /* if Saved Button is pressed,  request the  Value to check then saveing them if vaild*/
        EF_void_PrintMenus_RequestValue (PARAMETER_SCALE_MAX_V, TRUE);
        u16CurrentState = ESCAPE_STATE;
        break;

    case PRAM_CAT_HIGH_BUTTON:
        u32PramCatNumber ++;
        * (U32_t*)u32ParsingData   = u32PramCatNumber;
        u16CurrentState = PRAMETER_CAT_WEIGHT_BUTTON;
        break;

    case PRAM_CAT_LOW_BUTTON:
        u32PramCatNumber --;
        * (U32_t*)u32ParsingData   = u32PramCatNumber;
        u16CurrentState = PRAMETER_CAT_WEIGHT_BUTTON;
        break;


    case SAVE_CAT_WEIGHT_BUTTON:
        if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
          {
              if (u8CatWeightFlag == SECOND_EDITING)
              {
                  u8CatWeightFlag = PREPARE_TO_EDIT;

                  if (u32CategoryWeighT > MAX_CATEGORY_WEIGTH)
                  {
                      EF_void_PrintMenus_DisplayPhoto ( ERROR_CAT_WEIGHT_PHOTO );
                      EF_void_PrintMenus_SendValue    ( ERROR_MAX_CAT_WEIGHT_V , MAX_CATEGORY_WEIGTH, TRUE );

                      EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
                  }
                  else
                  {
                      /* save in Array to get tolerance fastly in Operation mode */
                      CategoryStruct[u32PramCatNumber].CategoryWeight = u32CategoryWeighT;
                      CategoryStruct[u32PramCatNumber].CategoryStatus = TRUE;
                      u32CatStatus = TRUE;
                      /* save in EEPROM to Premenatly saving it */
                      EF_BOOLEAN_EEPROM_WriteNBytes ( &u32CategoryWeighT  ,  CATEGORIES_ADD_EEPROM + 4*4*(u32PramCatNumber-1) , 4);
                      EF_BOOLEAN_EEPROM_WriteNBytes ( &u32CatStatus , CATEGORIES_ADD_EEPROM + 4*4*(u32PramCatNumber-1) + 3*4 , 4);

                      EF_void_PrintMenus_DisplayPhoto  ( DATA_IS_SAVED_PHOTO );
                      /* start timer to print save photo */
                      EF_void_TimerStart (SAVING_TOL_TIMER_ID);

                      u16CurrentState     = PRAMETER_CAT_WEIGHT_BUTTON;
                  }
              }
              else
              {
                  /* if Saved Button is pressed,  request the tolerance Values to check then saveing them if vaild*/
                  EF_void_PrintMenus_RequestValue ( PARAMETER_CAT_NUM_V, TRUE);
                  EF_void_PrintMenus_RequestValue ( PARAMETER_CAT_WEIGHT_V, TRUE);
                  u16CurrentState     = ESCAPE_STATE;
              }
          }
          else
          {
              /* if time is out , Stop Timer */
              if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
              {
                  EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID);

                  /* Return to Pasre State, The Expected : Save (after_adding _Values_or_not) and Back */
                  u16CurrentState     = PRAMETER_CAT_WEIGHT_BUTTON;
              }
          }
        break;

    /*------------------------------------------------------------------------------------------------------------*/
    /*-------- these four switch cases will be received as a response for System request to get defined Data -----*/
    /*------------------------------------------------------------------------------------------------------------*/


    /*___ Barcode Saving ____*/
    case PARAMETER_BARCODE_MAX_V:
        /* check Barcode Limits   - if error : print Error for 1 sec  then goto the same state again
         *                        - if ok    : Print Data saved , Return to Parameters Main Menu after timer saving is finished
         * */
        if  (EF_u8_TimerIsEnabled (SAVING_TOL_TIMER_ID) == TRUE)
        {
            if ( EF_BOOLEAN_TimerCheck ( SAVING_TOL_TIMER_ID ) == TRUE )
            {
                u16CurrentState     = ESCAPE_STATE;
                EF_void_Timer_TurnOff (SAVING_TOL_TIMER_ID);
                EF_void_PrintMenus_DisplayPhoto  ( PARAMETERS_CHOOSE_PHOTO );
            }
        }
        else
        {
            if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
            {
                u32BarCode_Max = * (U32_t*)u32ParsingData ;
                if ( (u32BarCode_Max > MAX_CATEGORY_WEIGTH) || (u32BarCode_Max < MIN_WEIGTH))
                {
                    EF_void_PrintMenus_SendValue    ( ERROR_BARCODE_MAX_WEIGHT_V , MAX_CATEGORY_WEIGTH, TRUE );
                    EF_void_PrintMenus_SendValue    ( ERROR_BARCODE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );
                    EF_void_PrintMenus_DisplayPhoto ( ERROR_BARCODE_WEIGHT_PHOTO );
                    EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
                }
                else
                {
                    ParametersStr.Barcode_Max = u32BarCode_Max;  //todo read them in init function
                    /* save in EEPROM to Premenatly saving it */
                    EF_BOOLEAN_EEPROM_WriteNBytes ( &u32BarCode_Max , BARCODE_MAX_EEPROM , 4);
                    EF_void_PrintMenus_DisplayPhoto  ( DATA_IS_SAVED_PHOTO );
                    /* start timer to print save photo */
                    EF_void_TimerStart (SAVING_TOL_TIMER_ID);
                }
            }
            else
            {
                /* if time is out , Stop Timer */
                if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
                {
                    EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID);
                    u16CurrentState     = PRAM_BARCODE_LIMIT_BUTTON;
                }
            }
        }
        break;


    case PARAMETER_SCALE_MAX_V :
        if  (EF_u8_TimerIsEnabled (SAVING_TOL_TIMER_ID) == TRUE)
        {
            if ( EF_BOOLEAN_TimerCheck ( SAVING_TOL_TIMER_ID ) == TRUE )
            {
                u16CurrentState     = ESCAPE_STATE;
                EF_void_Timer_TurnOff (SAVING_TOL_TIMER_ID);
                EF_void_PrintMenus_DisplayPhoto  ( PARAMETERS_CHOOSE_PHOTO );
            }
        }
        else
        {
            if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
            {
                u32Scale_Max = * (U32_t*)u32ParsingData ;
                if (u32Scale_Max > MAX_CATEGORY_WEIGTH)
                {
                    EF_void_PrintMenus_SendValue    ( ERROR_SCALE_MAX_WEIGHT_V , MAX_CATEGORY_WEIGTH, TRUE );
                    EF_void_PrintMenus_SendValue    ( ERROR_SCALE_MIN_WEIGHT_V , MIN_CATEGORY_WEIGTH, TRUE );

                    EF_void_PrintMenus_DisplayPhoto ( ERROR_SCALE_WEIGHT_PHOTO );
                    EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
                }
                else
                {
                    ParametersStr.Scale_Max = u32Scale_Max;  //todo read them in init function
                    /* save in EEPROM to Premenatly saving it */
                    EF_BOOLEAN_EEPROM_WriteNBytes ( &u32Scale_Max , SCALE_MAX_EEPROM , 4);
                    EF_void_PrintMenus_DisplayPhoto  ( DATA_IS_SAVED_PHOTO );
                    /* start timer to print save photo */
                    EF_void_TimerStart (SAVING_TOL_TIMER_ID);
                }
            }
            else
            {
                /* if time is out , Stop Timer */
                if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
                {
                    EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID);
                    u16CurrentState     = PRAMETER_SCALE_LIMIT_BUTTON;
                }
            }
        }
        break;

    case PARAMETER_TOL_MAX_V :
        if  (EF_u8_TimerIsEnabled (SAVING_TOL_TIMER_ID) == TRUE)
        {
            if ( EF_BOOLEAN_TimerCheck ( SAVING_TOL_TIMER_ID ) == TRUE )
            {
                u16CurrentState     = ESCAPE_STATE;
                EF_void_Timer_TurnOff (SAVING_TOL_TIMER_ID);
                EF_void_PrintMenus_DisplayPhoto  ( PARAMETERS_CHOOSE_PHOTO );
            }
        }
        else
        {
            if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
            {
                u32Tolerance_Max = * (U32_t*)u32ParsingData ;
                if (u32Tolerance_Max > MAX_TOL_VALUE)
                {
                    EF_void_PrintMenus_SendValue    ( ERROR_MAX_TOLERANCE_V , MAX_TOL_VALUE, TRUE );
                    EF_void_PrintMenus_DisplayPhoto ( ERROR_MAX_TOLERANCE_PHOTO );
                    EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
                }
                else
                {
                    ParametersStr.Tolerance_Max = u32Tolerance_Max;  //todo read them in init function
                    /* save in EEPROM to Premenatly saving it */
                    EF_BOOLEAN_EEPROM_WriteNBytes ( &u32Tolerance_Max , TOL_MAX_EEPROM , 4);
                    EF_void_PrintMenus_DisplayPhoto  ( DATA_IS_SAVED_PHOTO );
                    /* start timer to print save photo */
                    EF_void_TimerStart (SAVING_TOL_TIMER_ID);
                }
            }
            else
            {
                /* if time is out , Stop Timer */
                if ( EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE )
                {
                    EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID);
                    u16CurrentState     = PRAMETER_MAX_TOL_BUTTON;
                }
            }
        }
        break;

    case PARAMETER_CAT_NUM_V:
        u32PramCatNumber = * (U32_t*)u32ParsingData ;

        if ( u8CatWeightFlag == PREPARE_TO_EDIT )
        {
            u8CatWeightFlag = FIRST_EDITING;
            u16CurrentState = ESCAPE_STATE;

        }
        else if ( u8CatWeightFlag == FIRST_EDITING )
        {
            u8CatWeightFlag = SECOND_EDITING;
            u16CurrentState = SAVE_CAT_WEIGHT_BUTTON;
        }
        else
        {
            //for any unexpected errors
            u8CatWeightFlag = PREPARE_TO_EDIT;
            u16CurrentState = ESCAPE_STATE;
        }
        break;


    case PARAMETER_CAT_WEIGHT_V:
        u32CategoryWeighT = * (U32_t*)u32ParsingData ;

        if ( u8CatWeightFlag == PREPARE_TO_EDIT )
        {
            u8CatWeightFlag = FIRST_EDITING;
            u16CurrentState = ESCAPE_STATE;

        }
        else if ( u8CatWeightFlag == FIRST_EDITING )
        {
            u8CatWeightFlag = SECOND_EDITING;
            u16CurrentState = SAVE_CAT_WEIGHT_BUTTON;
        }
        else
        {
            //for any unexpected errors
            u8CatWeightFlag = PREPARE_TO_EDIT;
            u16CurrentState = ESCAPE_STATE;
        }
        break;


    default :
        break;
    }

}


/*********************************************************************
 * Function    : EF_u16_UI_ParseUartBuffer (U8_t * DataPtr, U8_t * DataLength )
 *
 * DESCRIPTION : Get the Received Frame and Pasrsing it to get Data, Address, DataLength and Frame Command.
 *
 * PARAMETERS  : DataPtr: Pointer to get Parsing Data.
 *               DataLength : Length of receiving Length.
 *
 * Return Value: the Frame Address .
 ***********************************************************************/
U16_t EF_u16_UI_ParseUartBuffer (U8_t * DataPtr, U8_t * DataLength )
{
    U8_t  u8RxByte        = 0;              /* used to receive any byte */
    U8_t  u8Command       = 0;              /* used to save the frame Command */
    U8_t  u8FrameLength   = 0;              /* used to save the frame data length */
    U8_t  u8NumOfWords    = 0;              /* used to save the frame Number of lenght : (2) for 4 byte and (1) for 2 byte*/
    U8_t  u8Iterator      = 0;              /* used as counter */
    U16_t u16ReturnStatus = ESCAPE_STATE;   /* used as return value of this Fumction : represent the Command of the Frame if vaild */
    U8_t  u8AddressHigh   = 0;              /* used TO Save the High Byte of the Received Frame Prameter (Button or Value ) */
    U16_t u16Address      = 0;              /* used TO Save the Low  Byte of the Received Frame Prameter (Button or Value ) */


    /* read the receive buffer until finding the frame header or until Buffer finished */
    do
    {
        u8RxByte = (U8_t)readFromUART();

    }while ( (getAvailableDataCountOnUART() > 0) && (u8RxByte != SCREEN_FRAME_HEADER_H) );


    /* - if Button is received the frame will be : [5A   A5   06   83   ADD_high   ADD_Low   01  Value_High   Value_Low]
     * - if Value request , the response will be : [5A   A5   08   83   ADD_high   ADD_Low   02  Byte_4       Byte_3      Byte_2   Byte_1]
     * - if rTC   request , the response will be : [5A   A5   0A   81   20         07        year    month   day     00  hour    minutes    Seconds]
     * , 02 : two word , Byte_4 Byte_3 Byte_2 Byte_1 : (value 4 byte)
     * - check button : check on 06  with 83 then get Address to define the state
     * - check Value  : check on 08  with 83 then get Address to define the state and get the Value
     * * */

    if ( (U8_t)readFromUART() == SCREEN_FRAME_HEADER_L)
    {
       if ( getAvailableDataCountOnUART() > 0 )
       {
           u8FrameLength = (U8_t)readFromUART();

           /* if the Received is Value , there is 2 byte may be not received so looping to get the remaiing bytes */
           if  (u8FrameLength >= getAvailableDataCountOnUART() )
           {
               EF_void_TimerStart (GET_ALL_FRAME_TIMER_ID);
               while ( ( EF_BOOLEAN_TimerCheck ( GET_ALL_FRAME_TIMER_ID ) == FALSE ) &&  (getAvailableDataCountOnUART() < u8FrameLength) );

               EF_void_Timer_TurnOff ( GET_ALL_FRAME_TIMER_ID);
           }


           if (getAvailableDataCountOnUART() >= u8FrameLength)
           {
               /* Length = command + Data (for ex.: [Add + byte to defien Num of words + Value] )*/
               u8Command     = (U8_t)readFromUART();
               u8AddressHigh = (U8_t)readFromUART();
               u16Address    = (u8AddressHigh << 8) | ((U8_t)readFromUART());
               if (u8FrameLength == GET_RTC_LENGTH )
               {
                   for (u8Iterator = 0; u8Iterator < u8FrameLength-3; u8Iterator++)
                   {
                       DataPtr [u8Iterator] = (U8_t)readFromUART(); /* return Array[] = {year, month , day ,00 ,hour, minutes, Seconds}*/
                   }
               }
               else
               {
                   u8NumOfWords  = (U8_t)readFromUART();

                   for (u8Iterator = 0; u8Iterator < u8FrameLength-4; u8Iterator++)
                   {
                       /* u8FrameLength-3 : 3 as Data was [Add_2Bytes + 1byte to defien Num of words + Value]  */
                       /* the first receiving byte is the higher so make it the last place in the return integer */
                       DataPtr [u8FrameLength - 4 -1 -u8Iterator] = (U8_t)readFromUART();
                   }
               }
                if ( u8Command == READ_DATA_COMMAND )
                {
                    if (u8FrameLength == RX_BUTTON_FRAME_LENGTH)
                    {
                       /* give the address to the return argument */
                        *DataLength     = BUTTON_VALUE_LENGTH;
                        u16ReturnStatus = u16Address;
                    }
                    else if  (u8FrameLength == GET_VALUE_FRAME_LENGTH)
                    {
                        *DataLength     = DISPLAY_DATA_VALUE_LENGTH;
                        u16ReturnStatus = u16Address;
                    }
                    else
                    {
                        //don't do anything as our system will receive length 6 or 8 only in this project
                    }
                }
                else if ( u8Command == READ_REGISTER_COMMAND )
                {
                    u16ReturnStatus = u16Address;
                    *DataLength     = GET_RTC_VALUE_LENGTH;
                }
                else
                {
                    //don't do anything as our system will receive 0x83 command only in this project
                }
           }
       }
    }

    return u16ReturnStatus ;
}


