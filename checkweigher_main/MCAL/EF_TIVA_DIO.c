/***************************************************************
 *  Source File: EF_DIO.c
 *
 *  Description: DIO driver For TIVAC
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
 * Notes: This driver can use with DIO module only.
 *
 **************************************************************/

#include "EF_TIVA_DIO.h"


/*************************************************************
 * Global Definitions

**************************************************************/
/*********************************************************************
* Function    : DIO_InitPort(U8_t PortName, U8_t Value);
*
* DESCRIPTION : This function used to Initialize port.
*
* PARAMETERS  :  
*            
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*             U8_t Value   : User write the value in range (0 -> 255).
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or return 1 if OK.  
* Note        : 1- To configure PIN as output you should set the bit mask "1" 
*               and to set PIN input you should set bit mask "0".
*				2- PORTE is only 6 pins and PORTF is only 5 pin.
*				3- there are 6 pins needs to be unlocked to be changeable 
*				such as: PC[3:0] (for Debugging) , PD[7] and PF[0].
***********************************************************************/
S8_t  EF_S8_DIO_InitPort (U8_t PortName, U8_t Value)
{
    S8_t Status;
    
    Status = 1;
	switch(PortName)
	{
	     case 'a':
		 case 'A':
		 	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
			GPIOPinTypeGPIOOutput (GPIO_PORTA_BASE, (0xFF) & Value);
            GPIOPinTypeGPIOInput (GPIO_PORTA_BASE,  (0xFF) ^ Value);
		 break;
		 case 'b':
		 case 'B':
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
			GPIOPinTypeGPIOOutput (GPIO_PORTB_BASE, (0xFF) & Value);
            GPIOPinTypeGPIOInput (GPIO_PORTB_BASE,  (0xFF) ^ Value);
		 break;
		 case 'c':
		 case 'C':
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
			GPIOPinTypeGPIOOutput (GPIO_PORTC_BASE, (0xFF) & Value);
            GPIOPinTypeGPIOInput (GPIO_PORTC_BASE,  (0xFF) ^ Value);
		 break;
		 case 'd':
		 case 'D':
			HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
			HWREG(GPIO_PORTD_BASE + GPIO_O_CR ) = 0x80;
			HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
			GPIOPinTypeGPIOOutput (GPIO_PORTD_BASE, (0xFF) & Value);
            GPIOPinTypeGPIOInput (GPIO_PORTD_BASE,  (0xFF) ^ Value);
		 break;
		 case 'e':
		 case 'E':
			if (Value > 0x3F)
			{
				Status = -1;
				break;
			}
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
			GPIOPinTypeGPIOOutput (GPIO_PORTE_BASE, (0xFF) & Value);
            GPIOPinTypeGPIOInput (GPIO_PORTE_BASE,  (0xFF) ^ Value);
		 break;
		 case 'f':
		 case 'F':
			if (Value > 0x1F)
			{
				Status = -1;
				break;
			}
			HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
			HWREG(GPIO_PORTF_BASE + GPIO_O_CR ) = 0x01;
			HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
			GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, (0xFF) & Value);
            GPIOPinTypeGPIOInput (GPIO_PORTF_BASE,  (0xFF) ^ Value);
		 break;
		 default:
             Status = -1;

    }

    return(Status);
}

/*********************************************************************
* Function    : EF_B_DIO_InitPin (U8_t PortName, U8_t PinNumber,U8_t Direction)
*
* DESCRIPTION : This function used to Initialize port.
*
* PARAMETERS  :
*
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*             U8_t Value   : Enter pin direction wither input or output
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or return 1 if OK.
* Note        : 1- To configure PIN as output you should set the bit mask "1" 
*               and to set PIN input you should set bit mask "0".
*				2- PORTE is only 6 pins and PORTF is only 5 pin.
*				3- there are 6 pins needs to be unlocked to be changeable 
*				such as: PC[3:0] (for Debugging) , PD[7] and PF[0].
***********************************************************************/
S8_t  EF_S8_t_DIO_InitPin  (U8_t PortName, U8_t PinNumber,U8_t Direction)
{
	S8_t bStatus;
	bStatus = 1;
	if ( Direction > OUTPUT )
	{
		bStatus = -1;
	}
	else
	{
		switch(PortName)
			{
			     case 'a':
				 case 'A':
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTA_BASE,  1<<PinNumber);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTA_BASE, 1<<PinNumber);
					}
				 break;
				 case 'b':
				 case 'B':
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTB_BASE,  1<<PinNumber);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTB_BASE,1<<PinNumber);
					}
				 break;
				 case 'c':
				 case 'C':
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTC_BASE, 1<<PinNumber);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTC_BASE, 1<<PinNumber);
					}
				 break;
				 case 'd':
				 case 'D':
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
					HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
					HWREG(GPIO_PORTD_BASE + GPIO_O_CR ) = 0x80;
					HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTD_BASE, 1<<PinNumber);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTD_BASE, 1<<PinNumber);
					}
				 break;
				 case 'e':
				 case 'E':
					if (PinNumber > 5)
					{
						bStatus = -1;
						break;
					}
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTE_BASE,  1<<PinNumber);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTE_BASE, 1<<PinNumber);
					}
				 break;
				 case 'f':
				 case 'F':
					if (PinNumber > 4)
					{
						bStatus = -1;
						break;
					}
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
					HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
					HWREG(GPIO_PORTF_BASE + GPIO_O_CR ) = 0x01;
					HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTF_BASE,  1<<PinNumber);
					}

					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, 1<<PinNumber);
					}
				 break;
				 default:
		             bStatus = -1;

		}
	}


    return(bStatus);
}


/* PinNumbers : ex. (1<<2)|(1<<5). if you choosed pin 2,5*/
S8_t  EF_S8_t_DIO_InitSomePins (U8_t PortName, U8_t PinNumbers, U8_t Direction)
{
	S8_t bStatus;
	bStatus = 1;
	if ( Direction > OUTPUT )
	{
		bStatus = -1;
	}
	else
	{
		switch(PortName)
			{
			     case 'a':
				 case 'A':
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, PinNumbers);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTA_BASE, PinNumbers);
					}
				 break;
				 case 'b':
				 case 'B':
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTB_BASE,  PinNumbers);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTB_BASE,  PinNumbers);
					}
				 break;
				 case 'c':
				 case 'C':
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTC_BASE, PinNumbers);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTC_BASE, PinNumbers);
					}
				 break;
				 case 'd':
				 case 'D':
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
					HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
					HWREG(GPIO_PORTD_BASE + GPIO_O_CR ) = 0x80;
					HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTD_BASE, PinNumbers);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTD_BASE, PinNumbers);
					}
				 break;
				 case 'e':
				 case 'E':
					if (PinNumbers > (1<<5))
					{
						bStatus = -1;
						break;
					}
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTE_BASE,  PinNumbers);
					}
					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTE_BASE, PinNumbers);
					}
				 break;
				 case 'f':
				 case 'F':
					if (PinNumbers > (1<<4))
					{
						bStatus = -1;
						break;
					}
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
					HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
					HWREG(GPIO_PORTF_BASE + GPIO_O_CR ) = 0x01;
					HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
					if (Direction == INPUT)
					{
						GPIOPinTypeGPIOInput (GPIO_PORTF_BASE,  PinNumbers);
					}

					else if (Direction == OUTPUT)
					{
						GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, PinNumbers);
					}
				 break;
				 default:
		             bStatus = -1;

		}
	}


    return(bStatus);
}



/*********************************************************************
* Function    : DIO_WritePort(U8_t PortName, U8_t Value);
*
* DESCRIPTION : This function used to write value to any port.
*
* PARAMETERS  :  
*            
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*             U8_t Value   : User write the value in range (0 -> 255).
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or return 1 if OK.  
***********************************************************************/
S8_t  EF_S8_DIO_WritePort(U8_t PortName, U8_t Value)
{
    S8_t Status;
    
    Status = 1;
	switch(PortName)
	{
	     case 'a':
		 case 'A':
			GPIOPinWrite(GPIO_PORTA_BASE, 0xff, Value);
		 break;
		 case 'b':
		 case 'B':
             GPIOPinWrite(GPIO_PORTB_BASE, 0xff, Value);
		 break;
		 case 'c':
		 case 'C':
            GPIOPinWrite(GPIO_PORTC_BASE, 0xff, Value);
		 break;
		 case 'd':
		 case 'D':
            GPIOPinWrite(GPIO_PORTD_BASE, 0xff, Value);
		 break;
		 case 'e':
		 case 'E':
			 GPIOPinWrite(GPIO_PORTE_BASE, 0xff, Value);
		 break;
		 case 'f':
		 case 'F':
			GPIOPinWrite(GPIO_PORTF_BASE, 0xff, Value);
		 break;
		 default:
             Status = -1;
    }

    return(Status);
}

/*********************************************************************
* Function    : S16_t DIO_ReadPort (U8_t PortName);
*
* DESCRIPTION : This function used to read value from any port.
*
* PARAMETERS  :  
*            
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or the value.
***********************************************************************/
S16_t EF_S16_DIO_ReadPort (U8_t PortName)
{
    volatile S8_t  Status;

    switch(PortName)
	{
	     case 'a':
		 case 'A':
			Status = GPIOPinRead(GPIO_PORTA_BASE, 0xff);
		 break;
		 case 'b':
		 case 'B':
            Status = GPIOPinRead(GPIO_PORTB_BASE, 0xff);
		 break;
		 case 'c':
		 case 'C':
            Status = GPIOPinRead(GPIO_PORTC_BASE, 0xff);
		 break;
		 case 'd':
		 case 'D':
            Status = GPIOPinRead(GPIO_PORTD_BASE, 0xff);
		 break;
		 case 'e':
		 case 'E':
			 Status = GPIOPinRead(GPIO_PORTE_BASE, 0xff);
		 break;
		 case 'f':
		 case 'F':
			Status = GPIOPinRead(GPIO_PORTF_BASE, 0xff);
		 break;
		 default:
             Status = -1;
    }
    return(Status);
}

/*********************************************************************
* Function    : S8_t DIO_SetPin (U8_t PortName, U8_t PinNumber);
*
* DESCRIPTION : This function used to set pin any port.
*
* PARAMETERS  :  
*            
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*             U8_t PinNumber: User write the value in range (0 -> 7).
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or return 1 if OK.  
* Note        : This function set output PIN.
***********************************************************************/
S8_t  EF_S8_DIO_SetPin   (U8_t PortName, U8_t PinNumber)
{
    S8_t Status;

    if(PinNumber > 7)
	{
	    Status = -1;
    }
	else
	{    
		Status = 1;
	    switch(PortName)
		{
		 case 'a':
		 case 'A':
			GPIOPinWrite(GPIO_PORTA_BASE, 1<<PinNumber , 1<<PinNumber );
		 break;
		 case 'b':
		 case 'B':
             GPIOPinWrite(GPIO_PORTB_BASE,  1<<PinNumber , 1<<PinNumber );
		 break;
		 case 'c':
		 case 'C':
            GPIOPinWrite(GPIO_PORTC_BASE,  1<<PinNumber , 1<<PinNumber );
		 break;
		 case 'd':
		 case 'D':
            GPIOPinWrite(GPIO_PORTD_BASE,  1<<PinNumber , 1<<PinNumber );
		 break;
		 case 'e':
		 case 'E':
			 GPIOPinWrite(GPIO_PORTE_BASE,  1<<PinNumber , 1<<PinNumber );
		 break;
		 case 'f':
		 case 'F':
			GPIOPinWrite(GPIO_PORTF_BASE,  1<<PinNumber , 1<<PinNumber );
		 break;
			 default:
	             Status = -1;
	    }
    }
    return(Status);

}

/*********************************************************************
* Function    : S8_t DIO_ClearPin (U8_t PortName, U8_t PinNumber)
*
* DESCRIPTION : This function used to clear Pin in port.
*
* PARAMETERS  :  
*            
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*             U8_t PinNumber: User write the value in range (0 -> 7).
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or return 1 if OK.  
***********************************************************************/
S8_t  EF_S8_DIO_ClearPin (U8_t PortName, U8_t PinNumber)
{
    S8_t Status;

    if(PinNumber > 7)
	{
	    Status = -1;
    }
	else
	{
	    Status = 1;
	    switch(PortName)
		{
		 case 'a':
		 case 'A':
			GPIOPinWrite(GPIO_PORTA_BASE, 1<<PinNumber , 0 );
		 break;
		 case 'b':
		 case 'B':
             GPIOPinWrite(GPIO_PORTB_BASE,  1<<PinNumber , 0 );
		 break;
		 case 'c':
		 case 'C':
            GPIOPinWrite(GPIO_PORTC_BASE,  1<<PinNumber , 0 );
		 break;
		 case 'd':
		 case 'D':
            GPIOPinWrite(GPIO_PORTD_BASE,  1<<PinNumber , 0 );
		 break;
		 case 'e':
		 case 'E':
			 GPIOPinWrite(GPIO_PORTE_BASE,  1<<PinNumber , 0 );
		 break;
		 case 'f':
		 case 'F':
			GPIOPinWrite(GPIO_PORTF_BASE,  1<<PinNumber , 0 );
		 break;
		default:
	             Status = -1;
	    }
	}
    return(Status);

}
/*********************************************************************
* Function    : S8_t  DIO_SetPort  (U8_t PortName);
*
* DESCRIPTION : This function used to set port.
*
* PARAMETERS  :  
*            
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*             U8_t Value   : User write the value in range (0 -> 255).
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or return 1 if OK.  
***********************************************************************/
S8_t  EF_S8_DIO_SetPort  (U8_t PortName)
{
    S8_t Status;
    
    Status = 1;
	switch(PortName)
	{
	    case 'a':
		 case 'A':
			GPIOPinWrite(GPIO_PORTA_BASE, 0xff, 0xff);
		 break;
		 case 'b':
		 case 'B':
             GPIOPinWrite(GPIO_PORTB_BASE, 0xff, 0xff);
		 break;
		 case 'c':
		 case 'C':
            GPIOPinWrite(GPIO_PORTC_BASE, 0xff, 0xff);
		 break;
		 case 'd':
		 case 'D':
            GPIOPinWrite(GPIO_PORTD_BASE, 0xff, 0xff);
		 break;
		 case 'e':
		 case 'E':
			 GPIOPinWrite(GPIO_PORTE_BASE, 0xff, 0xff);
		 break;
		 case 'f':
		 case 'F':
			GPIOPinWrite(GPIO_PORTF_BASE, 0xff, 0xff);
		 break;
		 default:
             Status = -1;
    }

    return(Status);
}

/*********************************************************************
* Function    : S8_t  DIO_ClearPort(U8_t PortName);
*
* DESCRIPTION : This function used to clear port.
*
* PARAMETERS  :  
*            
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or return 1 if OK.  
***********************************************************************/
S8_t  EF_S8_DIO_ClearPort(U8_t PortName)
{
    S8_t Status;
    
    Status = 1;
	switch(PortName)
	{
	    	    case 'a':
		 case 'A':
			GPIOPinWrite(GPIO_PORTA_BASE, 0xff, 0);
		 break;
		 case 'b':
		 case 'B':
             GPIOPinWrite(GPIO_PORTB_BASE, 0xff, 0);
		 break;
		 case 'c':
		 case 'C':
            GPIOPinWrite(GPIO_PORTC_BASE, 0xff, 0);
		 break;
		 case 'd':
		 case 'D':
            GPIOPinWrite(GPIO_PORTD_BASE, 0xff, 0);
		 break;
		 case 'e':
		 case 'E':
			 GPIOPinWrite(GPIO_PORTE_BASE, 0xff, 0);
		 break;
		 case 'f':
		 case 'F':
			GPIOPinWrite(GPIO_PORTF_BASE, 0xff, 0);
		 break;
		 default:
             Status = -1;
    }

    return(Status);
}

/*********************************************************************
* Function    : S8_t  DIO_TogglePin(U8_t PortName, U8_t PinNumber);
*
* DESCRIPTION : This function used to toggle any PIN in any PORT.
*
* PARAMETERS  :  
*            
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*             U8_t PinNumber: User write the value in range (0 -> 7).
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or return 1 if OK.  
***********************************************************************/
S8_t  EF_S8_DIO_TogglePin(U8_t PortName, U8_t PinNumber)
{
    volatile S8_t Status;
    
    if(PinNumber > 7)
	{
	    Status = -1;
    }
	else
	{
	    Status = 1;
		switch(PortName)
		{
	     case 'a':
		 case 'A':
			Status = GPIOPinRead(GPIO_PORTA_BASE, 1<<PinNumber);
			GPIOPinWrite(GPIO_PORTA_BASE, 1<<PinNumber, Status ^ (1<<PinNumber) );
		 break;
		 case 'b':
		 case 'B':
            Status = GPIOPinRead(GPIO_PORTB_BASE,  1<<PinNumber);
			GPIOPinWrite(GPIO_PORTB_BASE, 1<<PinNumber, Status ^ (1<<PinNumber) );
		 break;
		 case 'c':
		 case 'C':
            Status = GPIOPinRead(GPIO_PORTC_BASE,  1<<PinNumber);
			GPIOPinWrite(GPIO_PORTC_BASE, 1<<PinNumber, Status ^ (1<<PinNumber) );
		 break;
		 case 'd':
		 case 'D':
            Status = GPIOPinRead(GPIO_PORTD_BASE,  1<<PinNumber);
			GPIOPinWrite(GPIO_PORTD_BASE, 1<<PinNumber, Status ^ (1<<PinNumber) );
		 break;
		 case 'e':
		 case 'E':
			 Status = GPIOPinRead(GPIO_PORTE_BASE,  1<<PinNumber);
			 GPIOPinWrite(GPIO_PORTE_BASE, 1<<PinNumber, Status ^ (1<<PinNumber) );
		 break;
		 case 'f':
		 case 'F':
			Status = GPIOPinRead(GPIO_PORTF_BASE,  1<<PinNumber);
			GPIOPinWrite(GPIO_PORTF_BASE, 1<<PinNumber, Status ^ (1<<PinNumber) );
		 break;
			 default:
	             Status = -1;
	    }
    }
	if (Status != -1)
	{
		Status = 1;
	}
    return(Status);
}

/*********************************************************************
* Function    : S8_t  DIO_CheckPin (U8_t PortName, U8_t PinNumber);
*
* DESCRIPTION : This function used to Check pin status .
*
* PARAMETERS  :  
*            
*             U8_t PortName: User write which port need to access.
*                            like 'A' or 'a'.
*             U8_t PinNumber: User write the value in range (0 -> 7).
*
* Return Value: The function will return -1 if user enter wrong inputs.
*               or return 1 if Pin ON and 0 If pin OFF.  
***********************************************************************/
S8_t  EF_S8_DIO_CheckPin (U8_t PortName, U8_t PinNumber)
{
    volatile S8_t Status      = 0;
    

    if(PinNumber > 7)
    {
        Status = -1;
    }
    else
    {
        switch(PortName)
        {
        case 'a':
        case 'A':
            Status = ( (GPIOPinRead(GPIO_PORTA_BASE, 1<<PinNumber) ) & (1<<PinNumber) )>>PinNumber;
            break;
        case 'b':
        case 'B':
            Status = ( (GPIOPinRead(GPIO_PORTB_BASE, 1<<PinNumber) )  & (1<<PinNumber) ) >>PinNumber;
            break;
        case 'c':
        case 'C':
            Status = ( (GPIOPinRead(GPIO_PORTC_BASE, 1<<PinNumber) ) & (1<<PinNumber) )>>PinNumber;
            break;
        case 'd':
        case 'D':
            Status = ( (GPIOPinRead(GPIO_PORTD_BASE, 1<<PinNumber) ) & (1<<PinNumber) )>>PinNumber;
            break;
        case 'e':
        case 'E':
            Status = ( (GPIOPinRead(GPIO_PORTE_BASE, 1<<PinNumber) ) & (1<<PinNumber) )>>PinNumber;
            break;
        case 'f':
        case 'F':
            Status = ( (GPIOPinRead(GPIO_PORTF_BASE, 1<<PinNumber) ) & (1<<PinNumber) )>>PinNumber;
            break;
        default:
            Status = -1;
	    }
    }
    return(Status);
}
