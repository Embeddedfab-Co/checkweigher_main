/***************************************************************
 *  Source File: EF_LCD.c
 *
 *  Description: This simple driver for LCD 2X16 4bit mode
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
 * Notes: This driver can use with 4 bit mode only.
 *
 **************************************************************/


#include <string.h>
#include "utils/ustdlib.h"
#include "EF_LCD.h"
#include "../MCAL/EF_TIVA_DIO.h"
#include "../MCAL/EF_TIVA_uart.h"
/**************************************************************
 * Definitions
 **************************************************************/


#define LCD_DATA0_PORT   'a'     /**< port for 4bit data bit 0 */
#define LCD_DATA0_PIN     2      /**< pin for 4bit data bit 0  */

#define LCD_DATA1_PORT   'a'     /**< port for 4bit data bit 1 */
#define LCD_DATA1_PIN     3      /**< pin for 4bit data bit 1  */

#define LCD_DATA2_PORT   'a'     /**< port for 4bit data bit 2 */
#define LCD_DATA2_PIN     4      /**< pin for 4bit data bit 2  */

#define LCD_DATA3_PORT   'b'     /**< port for 4bit data bit 3 */
#define LCD_DATA3_PIN     6      /**< pin for 4bit data bit 3  */

#define LCD_RS_PORT      'b'
#define LCD_RS_PIN        2

#define LCD_ENABLE_PORT  'e'
#define LCD_ENABLE_PIN    0


#define UART_TOUCH_SCREEN   UART3

#define COMMAND_MODE          0xCF
#define DATA_MODE             0xDF
#define DELAY_AFTER_SEND_LCD  100 //usec

/**************************************************************
 * API Functions
 **************************************************************/


/*********************************************************************
* Function    :  itoa_Convert (U16_t Number , U8_t * NumberASCII_ptr)
*
* DESCRIPTION : This function used to convert integer number to ASCII
*
* PARAMETERS  : U16_t Number : Number ,max. digits of it should be equalled  MAX_DIGIT_FOR_ITOA
*               U8_t * NumberASCII_ptr [MAX_DIGIT_FOR_ITOA] : pointer to the ASCII number after Conversion
*
* Return Value: BOOLEAN to check for Errors
**********************************************************************/
/* Enter Number of MAX_DIGIT */
void itoa_Convert (U32_t Number , U8_t * NumberASCII_ptr)
{
    U8_t  ArrayIndex = 0;
    U8_t NumberOfDigits = 0;
    U32_t Number_temp = Number;
    while ( Number == 0 )
    {
        NumberASCII_ptr[ArrayIndex] = '0';
        return;
    }
    while ( Number_temp > 0 )
    {
        Number_temp = Number_temp/10;
        NumberOfDigits++;
    }
    for ( ArrayIndex =0 ; ArrayIndex < NumberOfDigits ; ArrayIndex++)
    {
        NumberASCII_ptr[ArrayIndex] = Number %10 +'0';
        Number = Number/10;
    }
    /* rotate , using Number as a temp */
    for ( ArrayIndex =0 ; ArrayIndex< NumberOfDigits/2 ; ArrayIndex++)
    {
        Number     = NumberASCII_ptr[ArrayIndex];
        NumberASCII_ptr[ArrayIndex]   = NumberASCII_ptr[NumberOfDigits-ArrayIndex-1];
        NumberASCII_ptr[NumberOfDigits-ArrayIndex-1] = Number;
    }
}


/*********************************************************************
 * Function    : void initLCD(void);
 *
 * DESCRIPTION : This function used to Initialize LCD Ports and send suitable default
 *               CMD before start display.
 *
 * PARAMETERS  : Void.
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_LCD_init()
{
    //for atmega8
    UART_cfg_str  uart_cfg = {UART_TOUCH_SCREEN, 9600, NUMBER_OF_BITS_8, ONE_STOP_BIT, NO_PARITY, FALSE, FALSE, TRUE, TRUE};

    EF_void_UART_Init( &uart_cfg);

#ifndef DWIN_SCREEN

    /* after power up , delay 10ms should be existed */
    _delay_ms(10);
    EF_void_LCD_send_command(0x28);            /* Command ..0x28---> 4-bit mode - 2 line - 5x7 font */
//  EF_void_LCD_send_command(0x0A);            /* Command ..0x0A---> Display  cursor on*/
//  EF_void_LCD_send_command(0x0F);            /* Command ..0x0E/0F> for cursor blinking*/
    EF_void_LCD_send_command(0x0C);            /* Command ..0x0C---> Display no cursor - no blink */
    EF_void_LCD_send_command(0x06);            /* Command ..0x06---> Automatic Increment - No Display shift */
    EF_void_LCD_send_command(0x80);            /* Command ..0x80---> Address DDRAM with 0 offset 80h */
    _delay_ms(20);

    _delay_us(3400);
#endif

}


/*********************************************************************
 * Function    : void EF_EF_void_LCD_send_command(U8_t cmnd);
 *
 * DESCRIPTION : This function used to send CMD to LCD like move or control
 *               display mode.
 *
 * PARAMETERS  : U8_t cmnd:  Command User want to send.
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_LCD_send_command(U8_t cmnd)
{
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, COMMAND_MODE);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, cmnd);
//    _delay_us(DELAY_AFTER_SEND_LCD);

}

/*********************************************************************
 * Function    : void EF_void_LCD_send_data(U8_t data);
 *
 * DESCRIPTION : This function used to send data to LCD .
 *
 * PARAMETERS  : U8_t Data:  Data User want to send.
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_LCD_send_data(char data)
{
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, DATA_MODE);
    EF_BOOLEAN_UART_PutChar( UART_TOUCH_SCREEN, data);
//    _delay_us(DELAY_AFTER_SEND_LCD);
}

/***********************************************************************
 * Function    : void EF_void_LCD_goto(U8_t y, U8_t x);
 *
 * DESCRIPTION : This function moves the cursor the line y column x
 *               on the LCD module.
 *
 * PARAMETERS  : U8_t y, U8_t x.
 *
 * Return Value: Void.
 ***********************************************************************/
void EF_void_LCD_goto(U8_t y, U8_t x)
{
    U8_t firstAddress[] = {0x80,0xC0,0x94,0xD4};

    EF_void_LCD_send_command(firstAddress[y-1] + x-1);
    _delay_us(100);
}

/***********************************************************************
* Function    : EF_void_LCD_print_NUM(S16_t s16Numberstring,U8_t u8ColNumber);
*
* DESCRIPTION : This function used to convert integer number to ASCII
*               then print it in LCD
*
* PARAMETERS  : s16Numberstring: integer number wanted to be displayed in LCD
*               u8RowNumber    : to define which row to display ,
*               take 1 for first row, 2: for the second row
*
* Return Value: Void.
***********************************************************************/
void EF_void_LCD_print_NUM(U32_t u16Numberstring,U8_t u8RowNumber ,U8_t u8ColNumber)
{
    U8_t ArrayIndex = 0;
    U8_t s8DisplayStr[6] = {0};
    /* convert number to ASCII */
    itoa_Convert(u16Numberstring, s8DisplayStr);
    for(ArrayIndex = 0; s8DisplayStr[ArrayIndex] != 0; ArrayIndex++)
    {
        EF_void_LCD_goto(u8RowNumber, (u8ColNumber+ArrayIndex ));
        EF_void_LCD_send_data(s8DisplayStr[ArrayIndex]);
    }
}

/***********************************************************************
* Function    : void EF_void_LCD_print(U8_t *string);
*
* DESCRIPTION : This function used to display String which user
*               wants to display .
*
* PARAMETERS  : U8_t *string:  String User want to send.
*
* Return Value: Void.
***********************************************************************/
void EF_void_LCD_print(char *string)
{
    U8_t i = 0;

    while(string[i]!=0)
    {
        EF_void_LCD_send_data(string[i]);
        i++;
    }
}

/***********************************************************************
* Function    : void EF_void_LCD_print_ByLength
*
* DESCRIPTION : This function used to display String which user
*               wants to display .
*
* PARAMETERS  : U8_t *string:  String User want to send.
*               U8_t  length:  number of characters to be displayed
*
* Return Value: Void.
***********************************************************************/
void EF_void_LCD_print_ByLength(unsigned char *string, U8_t length)
{
    U8_t i = 0;

    while( i < length)
    {
        EF_void_LCD_send_data(string[i]);
        i++;
    }
}

/***********************************************************************
* Function    : EF_void_LCD_Clear_Screen(void);
*
* DESCRIPTION : This function used to clear the LCD screen
*
* PARAMETERS  : Void.
*
* Return Value: Void.
***********************************************************************/
void EF_void_LCD_Clear_Screen(void)
{
    EF_void_LCD_send_command(1<<0);
    _delay_us(2);
    _delay_ms(20);
}

/***********************************************************************
* Function    :EF_void_uploadCustomChar(U8_t location, unsigned char *character);
*
* DESCRIPTION : This function used to upload a custom char which user wants to map
*               it using CGRAM (Character Generator Ram Address set).
*
* PARAMETERS  : location : to define which CGRAM ADD offset. It takes from (0 to 7).
*               character: pointer to 8 pointer to char as CGRAM needs 8 char to map
*
* Return Value: Void.
***********************************************************************/
void EF_void_uploadCustomChar(U8_t location, unsigned char *character)

{
    U8_t ArrayIndex;
    /* the Add offset must be 0 to 7*/
    if (location < 8)
    {
        EF_void_LCD_send_command(0x40 + (location * 8));
        for (ArrayIndex = 0; ArrayIndex < 8; ArrayIndex++)
        {
            EF_void_LCD_send_data(character[ArrayIndex]);
        }
    }
    else
    {
        /* TODO: return Error */
    }

}


/***********************************************************************
* Function    : EF_void_LCD_HextoASCII(U8_t *hex);
*
* DESCRIPTION : This function used to convert hex to ASCII, then display it
*
* PARAMETERS  : hex: pointer to hex (one byte)
*
* Return Value: Void.
***********************************************************************/
void EF_void_LCD_HextoASCII(U8_t *hex)
{

    U8_t temp_L = 0;                    // dummy variable that will hold LSBs of Hex number
    U8_t temp_H = 0;                    // dummy variable that will hold MSBs of Hex number

    temp_H = *hex & 0xF0;           // Obtain the upper 4 bits (MSBs) of hex number
    temp_H = temp_H >> 4;           //

    if(temp_H >9)                   // Check if the number is a letter
    {
        temp_H += 0x37;             // Convert the number to a letter in ASCII
    }
    else
    {
        temp_H += 0x30;             // Convert the number to ASCII number
    }

    EF_void_LCD_send_data(temp_H);           // Display the number

    temp_L = *hex & 0x0F;           // Obtain the lower 4 bits (LSBs) of hex number

    if(temp_L >9)                   // Check if the the number is a letter
    {
        temp_L += 0x37;             // Convert the number to a letter in ASCII
    }
    else
    {
        temp_L += 0x30;             // Convert the number to ASCII number
    }

    EF_void_LCD_send_data(temp_L);           // Display the number
}

