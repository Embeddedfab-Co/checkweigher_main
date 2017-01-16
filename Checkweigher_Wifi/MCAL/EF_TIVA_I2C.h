/***************************************************************
 *  Source File: EF_TIVA_I2C.c
 *
 *  Description: I2C driver for TIVAC
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
#ifndef _EF_TIVA_I2C_H_
#define _EF_TIVA_I2C_H_

#include "../Utilities/std_types.h"
/*********************************************************************
 * Function    : void EF_void_I2C_Init(void);
 *
 * DESCRIPTION : This function used to Send Init I2C Module,there are many modules
 *               ,I2C1 is selected with PA6, PA7 , I2C is master here
 *
 * PARAMETERS  : Void
 *
 * Return Value: Void
 ***********************************************************************/
void EF_void_I2C_Init(void);
/*********************************************************************
 * Function    : void EF_void_I2C_Write
 *
 * DESCRIPTION : This function used to Write byte/bytes to one I2C data bus
 *               to defined Address
 *
 * PARAMETERS  : DeviceAddress: Slave Address with shift right 1 as Tiva function takes 7bit for Add
 *               Address      : Register Address in device
 *               DataPtr      : pointer to Data which is wantes to be written in this Address
 *               NoOfBytes    : Number of bytes to be written
 *
 * Return Value: Void
 *
 * Notes:
 ***********************************************************************/
void  EF_BOOLEAN_I2C_Write(U8_t DeviceAddress, U8_t Address, U8_t* DataPtr, U8_t NoOfBytes );
/*********************************************************************
 * Function    : void EF_void_I2C_Write
 *
 * DESCRIPTION : This function used to Write byte/bytes to one I2C data bus
 *               to defined Address
 *
 * PARAMETERS  : DeviceAddress: Slave Address with shift right 1 as Tiva function takes 7bit for Add
 *               Address      : Register Address in device
 *               DataPtr      : pointer to Data which is wantes to be written in this Address
 *               NoOfBytes    : Number of bytes to be written
 *
 * Return Value: Void
 *
 * Notes:
 ***********************************************************************/
void EF_void_I2C_Read(U8_t DeviceAddress, U8_t Address, U8_t* DataPtr, U8_t NoOfBytes );


#endif

