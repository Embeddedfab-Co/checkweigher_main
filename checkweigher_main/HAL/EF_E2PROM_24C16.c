/***************************************************************
 *  Source File: EF_E2PROM_24C16.c
 *
 *  Description: This simple driver for E2PROM_24C16
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

#include "../MCAL/EF_TIVA_I2C.h"
#include "../Utilities/std_types.h"
/*********************************************************************
 * Function    : EF_void_EEPROM24C16_Init
 *
 * DESCRIPTION : This function used to initalize the I2C Module
 *
 * PARAMETERS  : None
 *
 * Return Value: Void
 ***********************************************************************/
void EF_void_EEPROM24C16_Init ()
{
	EF_void_I2C_Init();
}
/*********************************************************************
 * Function    : void EF_void_EEPROM24C16_Write_Byte (U8_t memory_location,
 *                                          U8_t slave_address,
 *                                          U8_t data);
 *
 * DESCRIPTION : This function used to Write One byte
 *
 * PARAMETERS  : U8_t memory_location,
 *               U8_t slave_address,
 *               U8_t data
 *
 * Return Value: Void
 ***********************************************************************/
void EF_void_EEPROM24C16_Write_Byte (U8_t memory_location, U8_t slave_address, U8_t data )
{
    EF_BOOLEAN_I2C_Write( slave_address, memory_location, &data, 1 );

	_delay_ms(15);
}

/*********************************************************************
 * Function    : void EF_void_EEPROM24C16_Read_Byte (U8_t memory_location,
 *                                          U8_t slave_address);
 *
 * DESCRIPTION : This function used to Write One byte
 *
 * PARAMETERS  : U8_t memory_location:
 *               U8_t slave_address:
 *
 * Return Value: Void
 ***********************************************************************/
U8_t EF_void_EEPROM24C16_Read_Byte (U8_t memory_location, U8_t slave_address)
{
	U8_t data = 0;

	EF_void_I2C_Read( slave_address, memory_location, &data, 1 );

	return data;
}
