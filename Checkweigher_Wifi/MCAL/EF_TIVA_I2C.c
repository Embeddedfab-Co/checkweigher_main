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
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#include "../Utilities/std_types.h"

#define I2C_MASTER_BASE  I2C1_BASE



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
void EF_void_I2C_Init(void)
{
    //enable I2C module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);

    //enable GPIO peripheral that contains I2C 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Configure the pin muxing for I2C1 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    // Enable and initialize the I2C1 master module.  Use the system clock for
    // the I2C1 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C_MASTER_BASE, SysCtlClockGet(), false);

}

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
void EF_BOOLEAN_I2C_Write(U8_t DeviceAddress, U8_t Address, U8_t* DataPtr, U8_t NoOfBytes )
{
    U8_t DataCnt = 0;

    /* define the Device address with read flag =false */
    I2CMasterSlaveAddrSet(I2C_MASTER_BASE, DeviceAddress, false);

    /* Define the wanted Address in the Device to write in */
    I2CMasterDataPut(I2C_MASTER_BASE, Address);

    /* start sending */
    I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    /* waiting for Sending */
    while(!I2CMasterBusy(I2C_MASTER_BASE));
    while(I2CMasterBusy(I2C_MASTER_BASE));

    /* send data */
    for(DataCnt = NoOfBytes; DataCnt > 1; DataCnt--) // data
    {
        I2CMasterDataPut(I2C_MASTER_BASE, *DataPtr++);

        /* define it is sending continuously */
        I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

        /* waiting for Sending */
        while(!I2CMasterBusy(I2C_MASTER_BASE));
        while(I2CMasterBusy(I2C_MASTER_BASE));
    }

    /* send the last byte and definig it was the last one */
    I2CMasterDataPut(I2C_MASTER_BASE, *DataPtr);
    I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    /* waiting for Sending */
    while(!I2CMasterBusy(I2C_MASTER_BASE));
    while(I2CMasterBusy(I2C_MASTER_BASE));

}



/*********************************************************************
 * Function    : EF_void_I2C_Read
 *
 * DESCRIPTION : This function used to read byte/bytes from one I2C data bus
 *               from defined Address
 *
 * PARAMETERS  : DeviceAddress: Slave Address with shift right 1 as Tiva function takes 7bit for Add
 *               Address      : Register Address in device
 *               DataPtr      : pointer to Data which is wanted to be read from this Address
 *               NoOfBytes    : Number of bytes to be read
 *
 * Return Value: Void
 *
 * Notes:
 ***********************************************************************/
void EF_void_I2C_Read(U8_t DeviceAddress, U8_t Address, U8_t* DataPtr, U8_t NoOfBytes )
{
    U8_t DataCnt = 0;

    /* define the Device address with read flag =false */
    I2CMasterSlaveAddrSet(I2C_MASTER_BASE, DeviceAddress, false); // device address

    /* define the wanted Address in device to read from */
    I2CMasterDataPut(I2C_MASTER_BASE, (U8_t)Address);

    I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    /* waiting for Sending the Add.*/
    while(!I2CMasterBusy(I2C_MASTER_BASE));
    while(I2CMasterBusy(I2C_MASTER_BASE));

    /* define the Device address with read flag =true */
    I2CMasterSlaveAddrSet(I2C_MASTER_BASE, DeviceAddress, true);

    /* receive/read */
    if(NoOfBytes == 1)
    {
        /* define it is 1 byte */
        I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        /* waiting for receiving the Data */
        while(!I2CMasterBusy(I2C_MASTER_BASE));
        while(I2CMasterBusy(I2C_MASTER_BASE));

        *DataPtr = (U8_t)I2CMasterDataGet(I2C_MASTER_BASE);
    }
    else
    {
        /* define they are many bytes */
        I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
        /* waiting for receiving the Data */
        while(!I2CMasterBusy(I2C_MASTER_BASE));
        while(I2CMasterBusy(I2C_MASTER_BASE));

        *DataPtr++ = (U8_t)I2CMasterDataGet(I2C_MASTER_BASE);

        for(DataCnt = NoOfBytes; DataCnt > 2; DataCnt--)
        {
            I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

            /* waiting for receiving the Data */
            while(!I2CMasterBusy(I2C_MASTER_BASE));
            while(I2CMasterBusy(I2C_MASTER_BASE));

            *DataPtr++ = (U8_t)I2CMasterDataGet(I2C_MASTER_BASE);
        }
        /* read the last byte and definig it was the last one */
        I2CMasterControl(I2C_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

        while(!I2CMasterBusy(I2C_MASTER_BASE));
        while(I2CMasterBusy(I2C_MASTER_BASE));

        *DataPtr = (U8_t)I2CMasterDataGet(I2C_MASTER_BASE);
    }
}
