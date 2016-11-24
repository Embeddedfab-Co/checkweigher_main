/*
******************************************************
this file is to configure the data types for the system 
It defines the data types (compiler specific)
*********************************************
*/

#ifndef STD_TYPES_H
#define STD_TYPES_H
//#define UART_TERMINAL_DEBUG
//#define TRAILS_3_MONTHS                            /* Definition to enable the 3Month peroid to Close Operation Task , Comment it if not needed */
#define USING_VARIABLE_CAT_WEIGHT                  /* using Variable Category Number */
//#define BARCODE_STUP                               /* if want to use fixed compare with the scale */
//#define STUB_SCALE                                 /* if want to use make the scale as stup function as it works */
#define DWIN_SCREEN                                  /* to work with touch screen expect of LCD */
#define ENABLE_ANALYSIS_MODE                         /* Enable Analysis Selection */
//#define ENABLE_SCALE_DEBUG

typedef unsigned char BOOLEAN;

typedef unsigned char U8_t;
typedef signed char S8_t;

typedef unsigned short int U16_t;
typedef signed int S16_t;

typedef unsigned int U32_t;
typedef signed long S32_t;

typedef float FP32;
typedef double FP64;

typedef int MAINRETURN_t;

#define BYTE S8_t
#define UBYTE U8_t

#define WORD S16_t
#define UWORD U16_t

#define LONG S32_t
#define ULONG U32_t

#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL 0
#endif

extern void _delay_ms (U32_t MiliSeconds);

extern void _delay_us (U32_t MicroSeconds);

#endif
