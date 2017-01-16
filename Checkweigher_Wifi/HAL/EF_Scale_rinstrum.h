/*
 * EF_Scale.h
 *
 *  Created on: 13/01/2015
 *      Author: Mohamed Samy
 */

#ifndef EF_SCALE_H_
#define EF_SCALE_H_

#include "EF_Scale_rinstrum_cfg.h"

/*************************************************
 * 	extern API functions
 *************************************************/

/*********************************************************************
 * Function    : B_Scale_Init (void);
 *
 * DESCRIPTION : Initializes scale
 *
 * PARAMETERS  : Nothing
 *
 * Return Value: The status either success (1) or failure (0)
 ***********************************************************************/
BOOLEAN EF_BOOLEAN_Scale_Init  (U8_t b_isISR);

/*********************************************************************
* Function    : B_Scale_Set_Zeroing  (void);
*
* DESCRIPTION : Sets the scale zero.
*
* PARAMETERS  : Nothing
*
* Return Value: The status either success (1) or failure (-1).
*               And returns negative values in case of failure.
***********************************************************************/
BOOLEAN EF_BOOLEAN_Scale_Set_Zeroing  ();

/*********************************************************************
* Function    : S16_Get_Gross_Weight  (void);
*
* DESCRIPTION : Gets the real value of gross weight.
*
* PARAMETERS  : Nothing
*
* Return Value: The real value of indicated weight between 0 grams and
*               50000 grams. And returns -1 to indicate under load.
***********************************************************************/
BOOLEAN EF_BOOLEAN_Scale_Get_Gross_Weight  (U32_t * u16WeightInGrams, BOOLEAN * bScaleError);

/*********************************************************************
* Function    : S16_Get_Stable_Weight  (void);
*
* DESCRIPTION : Get the real number of weight in grams between 0 and
*               50000
*
* PARAMETERS  : Nothing
*
* Return Value: The value of weight. And returns -1 to indicate that
*               print isn't available
***********************************************************************/
BOOLEAN EF_BOOLEAN_Scale_Get_Stable_Weight  (volatile U32_t * u16WeightInGrams, BOOLEAN * bScaleError);

/*********************************************************************
* Function    : F64_Get_Increased_Resolution_Weight  (void);
*
* DESCRIPTION : Gets the real value of increased resolution weight.
*
* PARAMETERS  : Nothing
*
* Return Value: The value of returned weight with increased resolution
*               between 0 grams and 50000 grams. And returns -1 to
*               indicate that error occured with increased resolution.
***********************************************************************/
BOOLEAN B_Scale_Get_Increased_Resolution_Weight  (FP64 * f64WeightInGrams, BOOLEAN * bScaleError);

/*********************************************************************
* Function    : s16_Get_Indicated_Weight_Number(S8_t *);
*
* DESCRIPTION : Gets the real value of indicated weight.
*
* PARAMETERS  : Nothing
*
* Return Value: The real value of indicated weight between 0 grams and
*               50000 grams. And returns -1 to indicate overload.
***********************************************************************/
BOOLEAN B_Scale_Get_Indicated_Weight  (U16_t * u16WeightInGrams, BOOLEAN * bScaleError);

/*********************************************************************
* Function    : B_Get_All_Weight  (S16_t *);
*
* DESCRIPTION : Get the real values of all weights in grams between
*               0 and 5000 grams. We get the net, tare and gross
*               weights
*
* PARAMETER 1 : The returned frame of containing real values of all
*               weights between 0 grams and 50000 grams. And returns
*               -1 to indicate ADC out error.
*
* Return Value: void
***********************************************************************/
BOOLEAN B_Scale_Get_All_Weight  (U16_t * u16WeightInGrams, U8_t * bScaleError);

/*********************************************************************
* Function    : S16_Get_Count_Number  (void);
*
* DESCRIPTION : Gets the real value of scale count.
*
* PARAMETERS  : void
*
* Return Value: The real value of count. And returns negative values in
*               case of failure.
***********************************************************************/
BOOLEAN B_Scale_Get_Count  (U16_t * u16NetCount, BOOLEAN * bScaleError);

/*********************************************************************
* Function    : B_Get_Status  (void);
*
* DESCRIPTION : Get the current status of the scale
*
* PARAMETERS  : Pointer to returned frame of status in ASCII.
*               Frame is in form [ADR][S][STATUS-1][STATUS-2][STATUS-3]
*               STATUS-1 : Stable or dynamic
*               STATUS-2 : Gross  or net
*               STATUS-3 : In range or out of range
*               The frame of length 15.
*
* Return Value: Returns true in case status is normal and returns -1 in
*               case the status is out of range
***********************************************************************/
BOOLEAN B_Scale_Get_Status  (BOOLEAN *bScaleError);

/*********************************************************************
* Function    : B_Apply_Tare  (void);
*
* DESCRIPTION : Apply taring for the scale
*
* PARAMETERS  : Nothing
*
* Return Value: Returns true for success, -1 for execution failure or
*               -2 for unavailability.
***********************************************************************/
S16_t s16_Scale_Apply_Tare  (BOOLEAN * bScaleError);

/*********************************************************************
* Function    : B_Clear_Tare_Memory(void);
*
* DESCRIPTION : Clears the tare memory
*
* PARAMETERS  : Nothing
*
* Return Value:  Returns true for success or -1 in case clear is
*                unavailable in count mode
***********************************************************************/
BOOLEAN B_Scale_Clear_Tare_Memory  (BOOLEAN * bScaleError);

#endif /* EF_SCALE_H_ */
