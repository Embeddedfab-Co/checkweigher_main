/***************************************************************
 *  Source File: EF_PrintMenus.h
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

#ifndef _EF_PRINT_MENUS_H_
#define _EF_PRINT_MENUS_H_

#include "../Utilities/std_types.h"
#include "EF_UserInterface_cfg.h"




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
void EF_void_PrintMenus_CalWeightShow (U32_t u32CatNumber,U32_t u32CatWeight , U32_t u32PositiveValue  , U32_t u32NegativeValue );

void EF_void_PrintMenus_CalEditPosNeg (U32_t u32CatNumber , U32_t u32CatWeight );



/*********************************************************************
 * Function    : void EF_void_PrintOperation ()
 *
 * DESCRIPTION : to print the weights and differences between Scale and Parcode weights
 *
 * PARAMETERS  : Void
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_PrintOperation (U32_t ParCodeWeigth, U32_t ScaleWeigth, BOOLEAN b_isAcceptPacket );


/*********************************************************************
 * Function    :EF_void_PrintMenus_DisplayPicture (U16_t u16ValueAddress)
 *
 * DESCRIPTION : send a picture to the touch screen and displayed it in it's address
 *
 * PARAMETERS  : u16ValueAddress
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_PrintMenus_DisplayPhoto (U16_t u16FrameAddress);

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
void EF_void_PrintMenus_SendValue (U16_t u16ValueAddress, U32_t u32Value, BOOLEAN b_isValue32bit);


void EF_void_PrintMenus_GetRtc ();

void EF_void_PrintMenus_RequestValue (U16_t u16ValueAddress, BOOLEAN b_isValue32bit);

/**************** to remove after that *****************************/

void EF_void_PrintMenus_OperationWithZeroing ();

#endif
