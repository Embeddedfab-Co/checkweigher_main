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

/* The Avalible Tolerance for every weight Category from number 1 to 21 category
 * Used it with void_WritingToleranceInEEPROM (commented function used one time only)*/
U32_t ToleranceWeight [22][2] =
{       {0   , 0},     /* represent no thing as Category number starts from (1) */
        {50  , 50},   /* start from 1 to MAX (21 here) as there is no 0 key */
        {50  , 50},
        {50  , 50},
        {50  , 50},
        {50  , 50},
        {50  , 50},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
        {100 , 100},
};


        U8_t  gu8OperSwitchStatus          = 0;         /* for Switch status */
        U8_t  bScaleError                  = 0;         /* use with Scale Function Argument */
        U32_t gU32NumOfCorrectPackages     = 0;         /* Save Correct Packge Number in it */
        U32_t gU32NumOfWrongPackages_Under = 0;         /* Save UnderLoad Packge Number in it */
        U32_t gU32NumOfWrongPackages_Over  = 0;         /* Save OverLoad Packge Number in it */
        U8_t  gU8BarCodeUartInit_FirstTime = 0;         /* used to make UARTinit in the First trial to get the BarCode */
        U8_t  gU8StopOperations_Flag       = FALSE;     /* Flag to check if 3Months passed--> Diable Operation if Enabled TRAILS_3_MONTHS */
extern  U32_t gU32CounterExpired_3Months;
static  U32_t gU32CategoryNumber           = 0;         /* Save wanted Category Number in it */
static  UART_cfg_str  uart_init_cfg        = { UART_TOUCH_SCREEN , SCREEN_UART_BAUDRATE, NUMBER_OF_BITS_8, ONE_STOP_BIT,
                                               NO_PARITY         , TRUE                , FALSE           , TRUE        , TRUE};

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
 * Function    :void_WritingToleranceInEEPROM  ()
 *
 * DESCRIPTION : execute one time only (in the first Burning time ) to save the Weight Tolerance Array in EEPROM.
 *
 * PARAMETERS  : None.
 *
 * Return Value: None.
 ***********************************************************************/
void void_WritingToleranceInEEPROM  ()
{
    U8_t u8Iterator       = 0;
    U8_t u8MemoryLocation = 0;


    for (u8Iterator = MIN_TOL_CATEGORIES; u8Iterator <= MAX_TOL_CATEGORIES ; u8Iterator++)
    {
        EF_BOOLEAN_EEPROM_WriteNBytes ((U32_t *)&ToleranceWeight[u8Iterator][POSITIVE_TOL], TOLERANCE_ARRAY_ADD_EEPROM +  u8MemoryLocation*4     , 4);
        EF_BOOLEAN_EEPROM_WriteNBytes ((U32_t *)&ToleranceWeight[u8Iterator][NEGATIVE_TOL], TOLERANCE_ARRAY_ADD_EEPROM + (u8MemoryLocation+1)*4  , 4);
        u8MemoryLocation = u8MemoryLocation + 2;
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
           U8_t  u8ExpectedCatNum          = 0;                /* To get Expected Category Number */
           U32_t u32Tol_PositiveValue      = 0;                /* To save Positive Tolerance from EEPROM in it */
           U32_t u32Tol_NegativeValue      = 0;                /* To get Negative Tolerance  from EEPROM in it */
           S32_t s32Difference             = 0;                /* To save the Difference */
#ifdef USING_VARIABLE_CAT_WEIGHT
           U32_t CategoryWeight            = 0;
           U8_t  u8Counter                 = 0;
           U32_t u32TimerCompareValue      = GET_OPER_VALUES_TIMEOUT/2 ;
#endif

    switch (gu8OperSwitchStatus)
    {
    /* this state is used to initlize the Variables */
    case MENU_WITHOUT_VALUES_STATE :
        u8BarCode_Status          = BARCODE_FALSE;
        u8ScaleStableStatus       = SCALE_NOT_STABLE;
        /* if BARCODEWeigth and ScaleWeigth is less than MinWeigth , it will give Error , So if it's Declaration Value is Less
         * than Min and No Value Come from Barcode/Scale then it will give Error, So MinWeigth in Declaration is needed */
        u32BARCODEWeigth          = MIN_WEIGTH;
        u32ScaleWeigth            = MIN_WEIGTH;
        u32TimerCompareValue      = GET_OPER_VALUES_TIMEOUT/2 ;
        gu8OperSwitchStatus       = GET_ALL_DATA_STATE;
        EF_void_Timer_TurnOff ( GET_OPER_VALUES_TIMER_ID );
        EF_void_Timer_TurnOff ( NEW_OPERATION_TIMER_ID );

        break;


    /* this state helps me to start the Get Value timer in the first Entery of any new operation */
    case GET_ALL_DATA_STATE :
        /* Print Operation Menu if Not Error Found, else after Error ,Print Operation Menu */
        if ( (EF_u8_TimerIsEnabled ( ERROR_PHOTO_TIMER_ID ) == TRUE) )
        {
            if (EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE)
            {
                EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID );
                EF_void_PrintMenus_OperationWithZeroing ();
            }
        }
        else
        {
            EF_void_PrintMenus_OperationWithZeroing ();
        }

        /* Get Barcode and scale if it was possible */
#ifdef BARCODE_SCALE
        u8BarCode_Status  = TRUE ;
        u32BARCODEWeigth  = 3000 ;
#else
         u8BarCode_Status   = EF_BOOLEAN_BarCodeScanner_ReadData ( &u32BARCODEWeigth ) ;
#endif

#ifdef USING_PROXIMITY
        if (EF_BOOLEAN_IOCard_ReadPhotoCell() == FOUND_PACKAGE)
        {
            u8ScaleStableStatus = EF_BOOLEAN_Scale_Get_Stable_Weight ( &u32ScaleWeigth , &bScaleError );
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
        //todo : if all is ok what happend ?

        /* if Scale stable : print it, start Get Barcode timer and goto GET_BARCODE_STATE */
        if ( (u8BarCode_Status == BARCODE_FALSE) && (u8ScaleStableStatus == SCALE_STABLE) )
        {
            /* check for Weight limits. if Error found ,Print Weight Error and start Error Timer */
            if ( (u32ScaleWeigth > MAX_WEIGTH) || (u32ScaleWeigth < MIN_WEIGTH))
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_SCALE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MAX_WEIGHT_V , MAX_WEIGTH, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );

                EF_void_TimerStart    ( ERROR_PHOTO_TIMER_ID );
            }
            else
            {
                EF_void_Timer_TurnOff (ERROR_PHOTO_TIMER_ID);
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
            if ( (u32BARCODEWeigth > MAX_WEIGTH) || (u32BARCODEWeigth < MIN_WEIGTH))
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_BARCODE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MAX_WEIGHT_V , MAX_WEIGTH, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );

                EF_void_TimerStart ( ERROR_PHOTO_TIMER_ID );
            }
            else
            {
                EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID );
                EF_void_PrintMenus_OperationWithZeroing ();
                EF_void_PrintMenus_SendValue ( OPERATION_BARCODE_WEIGHT_V , u32BARCODEWeigth, TRUE );
                gu8OperSwitchStatus     = GET_SCALE_STATE;
                EF_void_TimerStart ( GET_OPER_VALUES_TIMER_ID );
            }
        }
        break;

    case GET_BARCODE_STATE :
        /* if timer get barcode is out,print error for 1 sec and stop Start Timer */
        if ( EF_u32_TimerGetCounterValue(GET_OPER_VALUES_TIMER_ID) >= u32TimerCompareValue )
        {
            u32TimerCompareValue      = GET_OPER_VALUES_TIMEOUT*4 ;

            EF_void_PrintMenus_DisplayPhoto (ERROR_WATING_BARCODE_PHOTO);
        }

        /* if timer error is out , return to MENU_WITHOUT_VALUES_STATE */
        else if ( EF_BOOLEAN_TimerCheck (GET_OPER_VALUES_TIMER_ID) == TRUE)
        {
            gu8OperSwitchStatus = MENU_WITHOUT_VALUES_STATE;
        }

        u8BarCode_Status  = EF_BOOLEAN_BarCodeScanner_ReadData ( &u32BARCODEWeigth ) ;
        if ( u8BarCode_Status == BARCODE_OK )
        {
            if (( (u32BARCODEWeigth > MAX_WEIGTH) || (u32BARCODEWeigth < MIN_WEIGTH) ) )
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_BARCODE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MAX_WEIGHT_V , MAX_WEIGTH, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_BARCODE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );

                EF_void_TimerStart    ( ERROR_PHOTO_TIMER_ID     );
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
            EF_void_TimerStart    ( ERROR_PHOTO_TIMER_ID     );
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

            if ( (EF_u8_TimerIsEnabled ( ERROR_PHOTO_TIMER_ID ) == FALSE) )
            {
#ifdef USING_PROXIMITY

            if (EF_BOOLEAN_IOCard_ReadPhotoCell() == FOUND_PACKAGE)
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
                if (EF_BOOLEAN_TimerCheck ( ERROR_PHOTO_TIMER_ID ) == TRUE)
                {
                    EF_void_Timer_TurnOff ( ERROR_PHOTO_TIMER_ID );
                }
            }
        }


#ifdef USING_PROXIMITY
        if (EF_BOOLEAN_IOCard_ReadPhotoCell() == FOUND_PACKAGE)
        {
            u8ScaleStableStatus = EF_BOOLEAN_Scale_Get_Stable_Weight ( &u32ScaleWeigth , &bScaleError );
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
            if ( (u32ScaleWeigth > MAX_WEIGTH) || (u32ScaleWeigth < MIN_WEIGTH))
            {
                EF_void_PrintMenus_DisplayPhoto ( ERROR_SCALE_WEIGHT_PHOTO );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MAX_WEIGHT_V , MAX_WEIGTH, TRUE );
                EF_void_PrintMenus_SendValue ( ERROR_SCALE_MIN_WEIGHT_V , MIN_WEIGTH, TRUE );

                EF_void_TimerStart    ( ERROR_PHOTO_TIMER_ID     );
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


        /* get BarCode weigth Category */
#ifdef USING_VARIABLE_CAT_WEIGHT
#ifdef SECOND_METHOD_CALCULATION
        /* from 800  to 1000 gram ===> Cat#2
         * From 1000 to 2000 gram ===> Cat#3 ,and so on
         * * */
        for (u8Counter = 1 ; u8Counter <= MAX_TOL_CATEGORIES ; u8Counter++)
        {
            EF_u8_PrintMenu_GetCategoryWeight (u8Counter , &CategoryWeight );
            if (u32BARCODEWeigth < CategoryWeight)
            {
                break;
            }
        }
        u8ExpectedCatNum = u8Counter;
#else
        /* from 500  to 1000 gram ===> Cat#1
         * From 1000 to 2000 gram ===> Cat#2 ,and so on
         * * */
        for (u8Counter = 1 ; u8Counter <= MAX_TOL_CATEGORIES ; u8Counter++)
        {
            EF_u8_PrintMenu_GetCategoryWeight ( u8Counter , &CategoryWeight );
            if ( u32BARCODEWeigth > CategoryWeight )
            {
                EF_u8_PrintMenu_GetCategoryWeight ( u8Counter+1 , &CategoryWeight );
                if ( u32BARCODEWeigth < CategoryWeight )
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
        u8ExpectedCatNum = u8Counter;
#endif

#else
#ifdef SECOND_METHOD_CALCULATION
        if (u32BARCODEWeigth < 800)
        {
            u8ExpectedCatNum = 1;
        }
        else
        {
            /* from 800  to 1000 gram ===> Cat#2
             * From 1000 to 2000 gram ===> Cat#3 ,and so on
             * * */
            u8ExpectedCatNum = u32BARCODEWeigth/1000;
            u8ExpectedCatNum = u8ExpectedCatNum + 2;
        }
#else
        /* the only category with weight < 1000 is Cat#1 , Category #1 is 800g, if very less than 800, it will be Not Accept */
        u8ExpectedCatNum = u32BARCODEWeigth/1000;          /* Cat. #2 is 1000 weight   , #3 = 2000g ... and so on */
        u8ExpectedCatNum ++;                            /* ++ as Cat#2 = 1000g      , Cat#3 = 2000g  and so on */
#endif
#endif


        s32Difference = u32BARCODEWeigth - u32ScaleWeigth;     /* Get the Difference to check for Tolerance to define Accept or not */
        if (s32Difference >= 0)
        {
            /* Check the Negative Tolerance as Scale < BarCode, get the NegTol from it's Place in EEPROM
                  every place in EEPROM is 4 byte , and TolNeg is saved in one of this place  (4byte for any TolNeg) */
            EF_BOOLEAN_EEPROM_ReadNBytes((U32_t *)&u32Tol_NegativeValue, (2*u8ExpectedCatNum )*4, 4);

            if ( u32Tol_NegativeValue >= s32Difference)
            {
                /* Accept */
                EF_BOOLEAN_EEPROM_ReadNBytes( &gU32NumOfCorrectPackages , CORRECT_PACKAGE_ADD_EEPROM , 4);
                gU32NumOfCorrectPackages++;
                b_isAcceptPackage = 1;
                EF_BOOLEAN_EEPROM_WriteNBytes ( &gU32NumOfCorrectPackages    , CORRECT_PACKAGE_ADD_EEPROM , 4);
            }
            else
            {
                EF_BOOLEAN_EEPROM_ReadNBytes ( &gU32NumOfWrongPackages_Under, UNDERLOAD_PACKAGE_ADD_EEPROM, 4);
                gU32NumOfWrongPackages_Under++;
                b_isAcceptPackage = 0;
                EF_BOOLEAN_EEPROM_WriteNBytes ( &gU32NumOfWrongPackages_Under, UNDERLOAD_PACKAGE_ADD_EEPROM, 4);
            }
        }
        else
        {
            s32Difference = u32ScaleWeigth - u32BARCODEWeigth ;   /* here the s32Difference is - . So make it Positive */

            /* Check the Positive Tolerance as Scale > BarCode, get the PositiveTol from it's Place in EEPROM */
            EF_BOOLEAN_EEPROM_ReadNBytes((U32_t *)&u32Tol_PositiveValue, (2*u8ExpectedCatNum - 1)*4, 4);

            if ( u32Tol_PositiveValue >= s32Difference)
            {
                /* ACCEPT */
                EF_BOOLEAN_EEPROM_ReadNBytes( &gU32NumOfCorrectPackages , CORRECT_PACKAGE_ADD_EEPROM , 4);
                gU32NumOfCorrectPackages++;
                b_isAcceptPackage = 1;
                EF_BOOLEAN_EEPROM_WriteNBytes ( &gU32NumOfCorrectPackages    , CORRECT_PACKAGE_ADD_EEPROM , 4);
            }
            else
            {
                /* OVERLOAD */
                EF_BOOLEAN_EEPROM_ReadNBytes ( &gU32NumOfWrongPackages_Over , OVERLOAD_PACKAGE_ADD_EEPROM, 4);
                gU32NumOfWrongPackages_Over++;
                b_isAcceptPackage = 0;
                EF_BOOLEAN_EEPROM_WriteNBytes ( &gU32NumOfWrongPackages_Over , OVERLOAD_PACKAGE_ADD_EEPROM, 4);
            }
        }

        /* print them with Result */
        EF_void_PrintOperation (u32BARCODEWeigth,  u32ScaleWeigth,  b_isAcceptPackage );
        EF_void_TimerStart    ( NEW_OPERATION_TIMER_ID );

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
    U32_t u32FirstRelease_EEPROM = 0;
    U8_t  u8Counter           = 0;


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
    EF_BOOLEAN_EEPROM_ReadNBytes (&u32FirstRelease_EEPROM  , FIRST_RELEASE_FLAG_EEPROM_ADD , 4);

    if (u32FirstRelease_EEPROM != FIRST_RELEASE_FLAG_VALUE)
    {
        /* Save 100 in it noto enter here again */
        u32FirstRelease_EEPROM = FIRST_RELEASE_FLAG_VALUE;
        EF_BOOLEAN_EEPROM_WriteNBytes ( &u32FirstRelease_EEPROM  , FIRST_RELEASE_FLAG_EEPROM_ADD , 4);

        /* Write EEPROM First Release only for Weigth Tolerance */
        void_WritingToleranceInEEPROM();

        /* init the Package Numbers in their places in EEPROM with Value = 0 */
        gU32NumOfCorrectPackages     = 0;
        gU32NumOfWrongPackages_Over  = 0;
        gU32NumOfWrongPackages_Under = 0;

        EF_BOOLEAN_EEPROM_WriteNBytes ( &gU32NumOfCorrectPackages    , CORRECT_PACKAGE_ADD_EEPROM ,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &gU32NumOfWrongPackages_Over , OVERLOAD_PACKAGE_ADD_EEPROM,  4);
        EF_BOOLEAN_EEPROM_WriteNBytes ( &gU32NumOfWrongPackages_Under, UNDERLOAD_PACKAGE_ADD_EEPROM, 4);

        /* Make gCounterExpired_3Months =0 to start timeout after 3Months to disable Operation if 3Months passed */
        gU32CounterExpired_3Months = 0;
        EF_BOOLEAN_EEPROM_WriteNBytes ( &gU32CounterExpired_3Months  , TIMER_EXPIRED_3Months_EEPROM_ADD , 4);

        /* Write the Category Weight for all categories */
        for (u8Counter = 0; u8Counter <= MAX_TOL_CATEGORIES; u8Counter++)
        {
            if (u8Counter == 1)
            {
                EF_u8_WriteCategoryWeight ( u8Counter, 800);
            }
            else
            {
                EF_u8_WriteCategoryWeight ( u8Counter, 1000 + (1000*(u8Counter - 2)) );
            }
        }
    }

#ifdef USING_PROXIMITY
    EF_BOOLEAN_IOCard_Init();
#endif
    /* Bar-Code Scanner */
    EF_BOOLEAN_BarCodeScanner_Init();

    EF_void_TimerCreate ( GET_ALL_FRAME_TIMER_ID   , GET_ALL_FRAME_TIMEOUT  );
    EF_void_TimerCreate ( SAVING_TOL_TIMER_ID      , DISPLAY_ERROR_TIMEOUT  );
    EF_void_TimerCreate ( ERROR_PHOTO_TIMER_ID     , DISPLAY_ERROR_TIMEOUT  );
    EF_void_TimerCreate ( GET_OPER_VALUES_TIMER_ID , GET_OPER_VALUES_TIMEOUT);
    EF_void_TimerCreate ( NEW_OPERATION_TIMER_ID   , NEW_OPERATION_TIMEOUT  );

}

/*********************************************************************
 * Function    : EF_u8_WriteCategoryWeight (U8_t u8CategoryNumber, U32_t u32CategoryWeight)
 *
 * DESCRIPTION : write and redfine the Category Weight for the corresponding Category Number.
 *
 * PARAMETERS  : u8CategoryNumber  : category number from 1 to 21
 *               u32CategoryWeight : Category Weight by Grams.
 *
 * Return Value: False if Error in Category Number or in EEPROM
 ***********************************************************************/
U8_t EF_u8_WriteCategoryWeight (U8_t u8CategoryNumber, U32_t u32CategoryWeight)
{
    U8_t u8ReturnValue = TRUE;


    if ( (u8CategoryNumber < MIN_TOL_CATEGORIES) || (u8CategoryNumber > MAX_TOL_CATEGORIES))
    {
        u8ReturnValue = FALSE;
    }
    else
    {
        u8ReturnValue = EF_BOOLEAN_EEPROM_WriteNBytes ( &u32CategoryWeight  , CATEGORY_WEIGHT_ARRAY_ADD + 4*(u8CategoryNumber-1) , 4);
    }
    return u8ReturnValue;
}



void  EF_v_UI_SystemStates ()
{
    static U32_t u32NegativeValue    = 0;
    static U32_t u32PositiveValue    = 0;
    static U32_t u32ParsingData      = 0;
    static U16_t u16CurrentState     = 0;
    static U8_t  u8EditToleranceFlag = PREPARE_TO_EDIT;
    static U8_t  u8ParsingDataLength = 0;

    if ( getAvailableDataCountOnUART() >= SCREEN_MIN_FRAME_LENGTH)
    {
        u16CurrentState = EF_u16_UI_ParseUartBuffer ( (U8_t *)&u32ParsingData, &u8ParsingDataLength);
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



    case SELECT_ANALYSIS_BUTTON:
//        EF_BOOLEAN_EEPROM_ReadNBytes       ( &gU32NumOfCorrectPackages      , CORRECT_PACKAGE_ADD_EEPROM   , 4);
//        EF_BOOLEAN_EEPROM_ReadNBytes       ( &gU32NumOfWrongPackages_Under  , UNDERLOAD_PACKAGE_ADD_EEPROM , 4);
//        EF_BOOLEAN_EEPROM_ReadNBytes       ( &gU32NumOfWrongPackages_Over   , OVERLOAD_PACKAGE_ADD_EEPROM  , 4);
//for test
        gU32NumOfCorrectPackages     = 1500;
        gU32NumOfWrongPackages_Over  = 30780;
        gU32NumOfWrongPackages_Under = 100467;

        EF_void_PrintMenus_DisplayPhoto    ( ANALYSIS_PHOTO );

        EF_void_PrintMenus_SendValue        ( CORRECT_PACKETS_NUM_V   , gU32NumOfCorrectPackages     , TRUE);
        EF_void_PrintMenus_SendValue        ( UNDERLOAD_PACKETS_NUM_V , gU32NumOfWrongPackages_Over  , TRUE);
        EF_void_PrintMenus_SendValue        ( OVERLOAD_PACKETS_NUM_V  , gU32NumOfWrongPackages_Under , TRUE);

        u16CurrentState = ESCAPE_STATE;

        break;


    case CLEAR_ANALYSIS_BUTTON:
        gU32NumOfCorrectPackages     = 0;
        gU32NumOfWrongPackages_Over  = 0;
        gU32NumOfWrongPackages_Under = 0;

        EF_void_PrintMenus_SendValue    ( CORRECT_PACKETS_NUM_V   , gU32NumOfCorrectPackages , TRUE);
        EF_void_PrintMenus_SendValue    ( UNDERLOAD_PACKETS_NUM_V , gU32NumOfWrongPackages_Over , TRUE);
        EF_void_PrintMenus_SendValue    ( OVERLOAD_PACKETS_NUM_V  , gU32NumOfWrongPackages_Under , TRUE);

        /* rewrite the Package Numbers in their places in EEPROM with Value = 0 to clear */
        EF_BOOLEAN_EEPROM_WriteNBytes   ( &gU32NumOfCorrectPackages    , CORRECT_PACKAGE_ADD_EEPROM   , 4);
        EF_BOOLEAN_EEPROM_WriteNBytes   ( &gU32NumOfWrongPackages_Over , OVERLOAD_PACKAGE_ADD_EEPROM  , 4);
        EF_BOOLEAN_EEPROM_WriteNBytes   ( &gU32NumOfWrongPackages_Under, UNDERLOAD_PACKAGE_ADD_EEPROM , 4);

        u16CurrentState = ESCAPE_STATE;

        break;



    case SELECT_EDIT_BUTTON:
        /* if time is out , Stop Timer */
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



    case CONFIRM_EDIT_BUTTON:
        EF_void_PrintMenus_RequestValue (ENTER_EDIT_CAT_NUM_V, TRUE);
        u8EditToleranceFlag = PREPARE_TO_EDIT;

        u16CurrentState = ESCAPE_STATE;
        break;

    case CONFIRM_SHOW_BUTTON:
        EF_void_PrintMenus_RequestValue (ENTER_SHOW_CAT_NUM_V, TRUE);
        u16CurrentState = ESCAPE_STATE;
        break;


    case SAVE_TOLERANACE_BUTTON:
        /* check Category Number  - if error   : print Error for 1 sec  then goto the same state  again
            *                       - if ok    : Print Data saved , Return to SELECT_EDIT_BUTTON
            * */
           if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
           {
               if (u8EditToleranceFlag == SECOND_EDITING)
               {
                   u8EditToleranceFlag = PREPARE_TO_EDIT;

                   //todo : random max tol if there is condition
                   if ( (u32PositiveValue > MAX_TOL_VALUE) || (u32NegativeValue > MAX_TOL_VALUE) )
                   {
                       EF_void_PrintMenus_DisplayPhoto  ( ERROR_MAX_TOLERANCE_PHOTO );
                       EF_void_PrintMenus_SendValue     ( ERROR_MAX_TOLERANCE_V , MAX_TOL_VALUE, TRUE );   //todo : add anther max tol if there is condition

                       EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
                   }
                   else
                   {
                       EF_BOOLEAN_EEPROM_WriteNBytes ( (U32_t *)&u32PositiveValue, (2*gU32CategoryNumber - 1)*4, 4);
                       EF_BOOLEAN_EEPROM_WriteNBytes ( (U32_t *)&u32NegativeValue, (2*gU32CategoryNumber)*4    , 4);
                       EF_void_PrintMenus_DisplayPhoto  ( DATA_IS_SAVED_PHOTO );

                       EF_void_TimerStart (SAVING_TOL_TIMER_ID);

                       u16CurrentState     = SELECT_EDIT_BUTTON;
                   }
               }
               else
               {
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

                   EF_void_PrintMenus_CalEditPosNeg (gU32CategoryNumber);
                   u32NegativeValue = 0;
                   u32PositiveValue = 0;

                   /* Return to Pasre State, The Expected : Save(after_adding _Values_or_not) and Back */
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



    /*-------- these four switch cases will be received as a response for System request to get defined Data -----*/
    case ENTER_SHOW_CAT_NUM_V:
        /* check Category Number  - if error : print Error for 1 sec  then Goto SELECT_SHOW_BUTTON again
          *                       - if ok    : Print Show Menu, Return to Main Parsing State
          * */
         if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
         {
             if (u32ParsingData > MAX_TOL_CATEGORIES)
             {
                 EF_void_PrintMenus_DisplayPhoto  ( ERROR_ENTER_CAT_NUM_PHOTO );
                 EF_void_PrintMenus_SendValue     ( ERROR_CAT_MAX_NUMBER_V, MAX_TOL_CATEGORIES, TRUE );   //todo : test for UL and for TRUE or False
                 EF_void_PrintMenus_SendValue     ( ERROR_CAT_MIN_NUMBER_V, MIN_TOL_CATEGORIES, TRUE );   //todo : test for UL and for TRUE or False

                 EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
             }
             else
             {
                 gU32CategoryNumber = u32ParsingData;
                 /* print Show Menu */
                 EF_void_PrintMenus_CalWeightShow (gU32CategoryNumber);
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
        if (EF_u8_TimerIsEnabled (ERROR_PHOTO_TIMER_ID) == FALSE)
        {
            if (u32ParsingData > MAX_TOL_CATEGORIES)
            {
                EF_void_PrintMenus_DisplayPhoto  ( ERROR_ENTER_CAT_NUM_PHOTO );
                EF_void_PrintMenus_SendValue     ( ERROR_CAT_MAX_NUMBER_V, MAX_TOL_CATEGORIES, TRUE );   //todo : test for UL and for TRUE or False
                EF_void_PrintMenus_SendValue     ( ERROR_CAT_MIN_NUMBER_V, MIN_TOL_CATEGORIES, TRUE );   //todo : test for UL and for TRUE or False

                EF_void_TimerStart (ERROR_PHOTO_TIMER_ID);
            }
            else
            {
                gU32CategoryNumber = u32ParsingData;
                /* Print Edit +,- Menu */
                EF_void_PrintMenus_CalEditPosNeg (gU32CategoryNumber);
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

        break;


    case EDIT_POSITIVE_TOLERANCE_V:

        u32PositiveValue = u32ParsingData;

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
        u32NegativeValue = u32ParsingData;

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

    default :
        break;
    }

}

U16_t EF_u16_UI_ParseUartBuffer (U8_t * DataPtr, U8_t * DataLength )
{
    U8_t  u8RxByte        = 0;
    U8_t  u8Command       = 0;
    U8_t  u8FrameLength   = 0;
    U8_t  u8NumOfWords    = 0;
    U8_t  u8Iterator      = 0;
    U16_t u16ReturnStatus = ESCAPE_STATE;
    U8_t  u8AddressHigh   = 0;
    U16_t u16Address      = 0;


    /* read the receive buffer until finding the frame header */
    do
    {
        u8RxByte = (U8_t)readFromUART();

    }while ( (getAvailableDataCountOnUART() > 0) && (u8RxByte != SCREEN_FRAME_HEADER_H) );


    /* - if Button is received the frame will be : [5A   A5   06   83   ADD_high   ADD_Low   01  Value_High   Value_Low]
     * - if Value request , the response will be : [5A   A5   08   83   ADD_high   ADD_Low   02  Byte_4       Byte_3      Byte_2   Byte_1]
     * , 02 : two word , Byte_4 Byte_3 Byte_2 Byte_1 : (value 4 byte)
     * - check button : check on 06  with 83 then get Address to define the state
     * - check Value  : check on 08  with 83 then get Address to define the state and get the Value
     * * */

    if ( (U8_t)readFromUART() == SCREEN_FRAME_HEADER_L)
    {
       if ( getAvailableDataCountOnUART() > 0 )
       {
           u8FrameLength = (U8_t)readFromUART();

           if  (u8FrameLength == GET_VALUE_FRAME_LENGTH)
           {
               EF_void_TimerStart (GET_ALL_FRAME_TIMER_ID);
               while ( ( EF_BOOLEAN_TimerCheck ( GET_ALL_FRAME_TIMER_ID ) == FALSE ) &&  (getAvailableDataCountOnUART() < u8FrameLength) );

               EF_void_Timer_TurnOff ( GET_ALL_FRAME_TIMER_ID);
           }


           if (getAvailableDataCountOnUART() >= u8FrameLength)
           {
               if  (u8FrameLength == GET_VALUE_FRAME_LENGTH)
               {
                   u8NumOfWords = 9;
               }

               /* Length = command + Data (for ex.: [Add + byte to defien Num of words + Value] )*/
               u8Command     = (U8_t)readFromUART();
               u8AddressHigh = (U8_t)readFromUART();
               u16Address    = (u8AddressHigh << 8) | ((U8_t)readFromUART());
               u8NumOfWords  = (U8_t)readFromUART();

               for (u8Iterator = 0; u8Iterator < u8FrameLength-4; u8Iterator++)
               {
                   /* u8FrameLength-3 : 3 as Data was [Add_2Bytes + 1byte to defien Num of words + Value]  */
                   /* the first receiving byte is the higher so make it the last place in the return integer */
                   DataPtr [u8FrameLength - 4 -1 -u8Iterator] = (U8_t)readFromUART();
               }


                if ( u8Command == READ_DATA_COMMAND )
                {
                    if (u8FrameLength == RX_BUTTON_FRAME_LENGTH)
                    {
                       /* give the address to the return argument */
                        *DataLength    = BUTTON_VALUE_LENGTH;
                        u16ReturnStatus = u16Address;
                    }
                    else if  (u8FrameLength == GET_VALUE_FRAME_LENGTH)
                    {
                        *DataLength    = DISPLAY_DATA_VALUE_LENGTH;
                        u16ReturnStatus = u16Address;
                    }
                    else
                    {
                        //don't do anything as our system will receive length 6 or 8 only in this project
                    }
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


