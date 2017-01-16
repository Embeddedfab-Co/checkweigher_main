/***************************************************************
 *  Source File: EF_EEPROM.c
 *
 *  Description: Internal EEPROM (1Kbytes) driver For ATMEGA32
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
/**************************************************************
 * Notes: This driver can use with EEPROM module only.
 *
 **************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/flash.h"
#include "driverlib/eeprom.h"


#include "../Utilities/std_types.h"

/*The below value should be set depending on the controller
by referring the respective data sheet*/
#define MAX_EEPROM_SIZE           2048  //bytes


/*********************************************************************
 * Function    : EF_BOOLEAN_EEPROM_Init
 *
 * DESCRIPTION : This function is used to init the EEPROM
 *
 * PARAMETERS  : NONE
 *
 * Return Value: BOOLEAN to check for errors
 ***********************************************************************/
BOOLEAN EF_BOOLEAN_EEPROM_Init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    if (EEPROMInit() ==  EEPROM_INIT_ERROR )
    {
        return FALSE;
    }

    return TRUE;

}





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
multiple of 4.
 *
 * Return Value: BOOLEAN to check for errors
 ***********************************************************************/
BOOLEAN EF_BOOLEAN_EEPROM_WriteNBytes(U32_t *DataPtr , U16_t Address, U16_t NoOfBytes)
{

    if (EEPROMProgram(DataPtr, Address, NoOfBytes ) == 0)
    {
        return TRUE;
    }

    return FALSE;

}

/*********************************************************************
 * Function    : void EEPROM_ReadNBytes(U16_t EepromAddr,
 *                                      U8_t *RamAddr,
 *                                      S8_t NoOfBytes);
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
BOOLEAN EF_BOOLEAN_EEPROM_ReadNBytes (U32_t *DataPtr , U16_t Address, U16_t NoOfBytes)
{
    if (NoOfBytes > MAX_EEPROM_SIZE)
    {
        return FALSE;
    }
    EEPROMRead(DataPtr, Address, NoOfBytes);

    return TRUE;
}
/*********************************************************************
 * Function    : EF_BOOLEAN_EEPROM_Erase(void);
 * DESCRIPTION : This function is used to erase the entire EEprom memory.
 * PARAMETERS  : void.
 *
 * Return Value: void.
 * Note        : void.
 ***********************************************************************/
BOOLEAN EF_BOOLEAN_EEPROM_Erase(void)
{
   if ( EEPROMMassErase() == 0)
   {
       return TRUE;
   }

   return FALSE;
}
