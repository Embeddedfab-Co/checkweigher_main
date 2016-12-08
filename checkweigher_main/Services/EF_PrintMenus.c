/***************************************************************
 *  Source File: EF_PrintMenus.c
 *
 *  Description: Using LCD driver to Print User Interface Menus.
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
#include "EF_PrintMenus.h"
#include "EF_PrintMenus_cfg.h"

#include "EF_UserInterface_cfg.h"
#include "../MCAL/EF_TIVA_uart.h"

#include <string.h>
#include "EF_UserInterface.h"

#define UART_TOUCH_SCREEN   UART3
#ifndef DWIN_SCREEN
U8_t * CateogryWeight [22] =
{
        {"----"},
        {"800g"},
        {"1000g"},
        {"2000g"},
        {"3000g"},
        {"4000g"},
        {"5000g"},
        {"6000g"},
        {"7000g"},
        {"8000g"},
        {"9000g"},
        {"10000g"},
        {"11000g"},
        {"12000g"},
        {"13000g"},
        {"14000g"},
        {"15000g"},
        {"16000g"},
        {"17000g"},
        {"18000g"},
        {"19000g"},
        {"20000g"},
};
#endif

/**************************API FuncTions ****************************************/






/*********************************************************************
 * Function    : EF_void_PrintMenus_CalWeightShow (U8_t u8CatNumber)
 *
 * DESCRIPTION : Just to Show the Wanted Category Weight and Category Number,
 *               Enable to return to Main Menu
 *
 * PARAMETERS  : CatNumber: defien Category Number
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_PrintMenus_CalWeightShow (U32_t u32CatNumber,U32_t u32CatWeight , U32_t u32PositiveValue  , U32_t u32NegativeValue )
{

    EF_void_PrintMenus_SendValue     ( SHOW_CATEGORY_NUM_V       , u32CatNumber     , TRUE );
    EF_void_PrintMenus_SendValue     ( SHOW_CATEGORY_WEIGHT_V    , u32CatWeight     , TRUE );
    EF_void_PrintMenus_SendValue     ( SHOW_POSITIVE_TOLERANCE_V , u32PositiveValue , TRUE );
    EF_void_PrintMenus_SendValue     ( SHOW_NEGATIVE_TOLERANCE_V , u32NegativeValue , TRUE );
    EF_void_PrintMenus_DisplayPhoto  ( SHOW_TOLERANCE_PHOTO );
}


void EF_void_PrintMenus_CalEditPosNeg (U32_t u32CatNumber , U32_t u32CatWeight )
{

    EF_void_PrintMenus_SendValue     ( EDIT_CATEGORY_NUM_V       , u32CatNumber , TRUE );
    EF_void_PrintMenus_SendValue     ( EDIT_CATEGORY_WEIGHT_V    , u32CatWeight , TRUE );
    EF_void_PrintMenus_SendValue     ( EDIT_POSITIVE_TOLERANCE_V , 0            , TRUE );
    EF_void_PrintMenus_SendValue     ( EDIT_NEGATIVE_TOLERANCE_V , 0            , TRUE );
    EF_void_PrintMenus_DisplayPhoto  ( EDIT_POSITIVE_NEG_PHOTO );

}

/*********************************************************************
 * Function    : void EF_void_PrintOperation ()
 *
 * DESCRIPTION : to print the weights and differences between Scale and BarCode weights
 *
 * PARAMETERS  : Void
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_PrintOperation (U32_t u32BarCodeWeigth, U32_t u32ScaleWeigth, BOOLEAN b_isAcceptPacket )
{
    S32_t s32Difference = 0;


    s32Difference = u32BarCodeWeigth - u32ScaleWeigth;

    EF_void_PrintMenus_SendValue ( OPERATION_BARCODE_WEIGHT_V, u32BarCodeWeigth, TRUE );
    EF_void_PrintMenus_SendValue ( OPERATION_SCALE_WEIGHT_V  , u32ScaleWeigth, TRUE );

    if (b_isAcceptPacket == ACCEPT_PACKET)
    {
        EF_void_PrintMenus_DisplayPhoto  ( ACCEPT_OPERATION_PHOTO );
    }
    else if  (b_isAcceptPacket == OVERLOAD_PACKET)
    {
        EF_void_PrintMenus_DisplayPhoto  ( OVERLOAD_OPERATION_PHOTO );
    }
    else if  (b_isAcceptPacket == UNDERLOAD_PACKET)
    {
        EF_void_PrintMenus_DisplayPhoto  ( UNDERLOAD_OPERATION_PHOTO );
    }

    EF_void_PrintMenus_SendValue ( OPERATION_DIFFERENCE_V    , abs(s32Difference) , TRUE);

}

/*********************************************************************
 * Function    :EF_void_PrintMenus_DisplayPicture (U16_t u16ValueAddress)
 *
 * DESCRIPTION : send a picture to the touch screen and displayed it in it's address
 *
 * PARAMETERS  : u16ValueAddress
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_PrintMenus_DisplayPhoto (U16_t u16FrameAddress)
{
    /*   frame : [header_High   Header_Low   Length   Data{CommandAddress,Values,....} ]
     *  -frame : [5A            A5           04       80   03 ADD_high   ADD_Low] */

    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, SCREEN_FRAME_HEADER_H);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, SCREEN_FRAME_HEADER_L);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, 0x04);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, SEND_PHOTO_COMMAND);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, 0x03);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, (U8_t) (u16FrameAddress>>8));
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, (U8_t) (u16FrameAddress));
}

/*********************************************************************
 * Function    :EF_void_PrintMenus_DisplaysValue (U16_t u16ValueAddress, U16_t u16Value)
 *
 * DESCRIPTION : send a Value to the touch screen and displayed it in it's address
 *
 * PARAMETERS  : u16ValueAddress : address of value on photo
 *               u16Value        :
 *               b_is32bit       : isthis bit is configured as 32 bit at the touch screen.
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_PrintMenus_SendValue (U16_t u16ValueAddress, U32_t u32Value, BOOLEAN b_isValue32bit)
{
    /*   frame : [header_High   Header_Low   Length   Data{CommandAddress,Values,....} ]
     *  -frame : [5A            A5           05       82   ADD_high   ADD_Low    VALUE_Hight   VALUE_Low] */

    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, SCREEN_FRAME_HEADER_H);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, SCREEN_FRAME_HEADER_L);
    if ( b_isValue32bit == TRUE)
    {
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, 0x07);
    }
    else
    {
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, 0x05);
    }

    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  WRITE_DATA_COMMAND);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  (U8_t)(u16ValueAddress>>8));
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  (U8_t)(u16ValueAddress));
    if ( b_isValue32bit == TRUE)
    {
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  (U8_t)(u32Value>>24));
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  (U8_t)(u32Value>>16));
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  (U8_t)(u32Value>>8));
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  (U8_t)(u32Value));
    }
    else
    {
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  (U8_t)(u32Value>>8));
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  (U8_t)(u32Value));
    }
}


void EF_void_PrintMenus_RequestValue (U16_t u16ValueAddress, BOOLEAN b_isValue32bit)
{
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, SCREEN_FRAME_HEADER_H);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, SCREEN_FRAME_HEADER_L);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, 0x04);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, READ_DATA_COMMAND);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, (U8_t)(u16ValueAddress>>8));
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, (U8_t)(u16ValueAddress));
    if ( b_isValue32bit == TRUE)
    {
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  0x02);
    }
    else
    {
        EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN,  0x01);
    }
    /* todo receive it -- wait or by isr */
}




void EF_void_PrintMenus_OperationWithZeroing ()
{
    EF_void_PrintMenus_SendValue     ( OPERATION_BARCODE_WEIGHT_V , 0, TRUE );
    EF_void_PrintMenus_SendValue     ( OPERATION_SCALE_WEIGHT_V   , 0, TRUE );
    EF_void_PrintMenus_SendValue     ( OPERATION_DIFFERENCE_V     , 0, TRUE );
    EF_void_PrintMenus_DisplayPhoto  ( OPERATION_PHOTO );

}




