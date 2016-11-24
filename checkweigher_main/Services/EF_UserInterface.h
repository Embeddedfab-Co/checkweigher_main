/***************************************************************
 *  Source File: EF_UserInterface.h
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
#ifndef _EF_USER_INTERFACE_H_
#define _EF_USER_INTERFACE_H_

#include "../MCAL/EF_TIVA_uart.h"
#include "utils/uartstdio.h"
#include <string.h>

/*********************************************************************
* Function    : void EF_void_UI_Init ();
*
* DESCRIPTION : This function used to Initialize Lcd,KeyPad and EEPROM
*
* PARAMETERS  : Void.
*
* Return Value: Void.
***********************************************************************/
void EF_void_UserInterface_Init ();

U16_t EF_u16_UI_ParseUartBuffer (U8_t * DataPtr, U8_t * DataLength );

void  EF_v_UI_SystemStates ();

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
U8_t EF_u8_WriteCategoryWeight (U8_t u8CategoryNumber, U32_t u32CategoryWeight);


/*********************************************************************
 * Function    : void vSystemInsallation()
 *
 * DESCRIPTION : The First State : Print the Project Name and The Initliazation Menu
 *               then Enable User to Select Operation or Analysis or Calibration
 *
 * PARAMETERS  : Void.
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_UserInterface_SystemInsallation();



#endif
