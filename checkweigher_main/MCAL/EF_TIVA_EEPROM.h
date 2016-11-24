/***************************************************************
 *  Source File: EF_EEPROM.h
 *
 *  Description: Internal EEPROM (2Kbytes) driver For TIVA-C
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

#ifndef EF_TIVA_EEPROM_H_
#define EF_TIVA_EEPROM_H_

#include "../Utilities/std_types.h"



/*********************************************************************
 * Function    : EF_BOOLEAN_EEPROM_Init
 *
 * DESCRIPTION : This function is used to init the EEPROM
 *
 * PARAMETERS  : NONE
 *
 * Return Value: BOOLEAN to check for errors
 ***********************************************************************/
extern BOOLEAN EF_BOOLEAN_EEPROM_Init();



/*********************************************************************
 * Function    : void EF_void_EEPROM_WriteNBytes(U16_t EepromAddr,
 *                                          U8_t *RamAddr,
 *                                          S8_t NoOfBytes);
 * DESCRIPTION : This function is used to write N-bytes of data
 *               at specified EEPROM_address.
 * PARAMETERS  :
 *               U32_t *DataPtr  : points to the first word of data to write to the EEPROM.
 *               U16_t Address   : defines the byte address within the EEPROM that the data is to be written to.
 *                                This value must be a multiple of 4.
 *               U16_t NoOfBytes : defines the number of bytes of data that is to be written. This value must be a
 *                                  multiple of 4.
 *
 * Return Value: BOOLEAN to check for errors
 ***********************************************************************/
BOOLEAN EF_BOOLEAN_EEPROM_WriteNBytes(U32_t *DataPtr , U16_t Address, U16_t NoOfBytes);



/*********************************************************************
 * Function    : void EEPROM_ReadNBytes
 *
 * DESCRIPTION : This function is used to Read N-bytes of data
 *               from specified EEPROM_address.
 * PARAMETERS  :
 *               U32_t *DataPtr  : points to the first word of data to write to the EEPROM.
 *               U16_t Address   : defines the byte address within the EEPROM that the data is to be written to.
 *                                This value must be a multiple of 4.
 *               U16_t NoOfBytes : defines the number of bytes of data that is to be written. This value must be a
 *                                 multiple of 4.
 *
 *
 * Return Value: false if errors and true if ok
 ***********************************************************************/
BOOLEAN EF_BOOLEAN_EEPROM_ReadNBytes (U32_t *DataPtr , U16_t Address, U16_t NoOfBytes);

/*********************************************************************
 * Function    : void EF_BOOLEAN_EEPROM_Erase(void);
 *
 * DESCRIPTION : This function is used to erase the entire EEprom memory.
 *
 * PARAMETERS  : void.
 *
 * Return Value: void.
 ***********************************************************************/
extern BOOLEAN EF_BOOLEAN_EEPROM_Erase(void);

#endif /* EF_TIVA_EEPROM_H_ */
