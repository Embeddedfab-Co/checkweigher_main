/*
 * EF_BarCode_Scanner.h
 *
 *  Created on: 2 Nov 2015
 *      Author: EmbeddedFab
 */

#ifndef _EF_BARCODE_SCANNER_H_
#define _EF_BARCODE_SCANNER_H_
#include "../Utilities/std_types.h"



/*********************************************************************
* Function    : EF_BOOLEAN_BarCodeScanner_Init ()
*
* DESCRIPTION : initialize the Uart
*
* PARAMETERS  : None.
*
* Return Value: Void.
***********************************************************************/
BOOLEAN EF_BOOLEAN_BarCodeScanner_Init(void);

/*********************************************************************
* Function    : EF_BOOLEAN_BarCodeScanner_ReadData (U32_t * BarCodeMeasuredWeight)
*
* DESCRIPTION : try to Read Barcode
*
* PARAMETERS  : pU32BarCodeMeasuredWeight : pointer to the returned read Barcode.
*
* Return Value: True if get the barcode correctly or false if fail.
***********************************************************************/
BOOLEAN EF_BOOLEAN_BarCodeScanner_ReadData(U32_t * BarCodeMeasuredWeight);


#endif /* AVON_TOUCHSCREEEN_HAL_EF_BARCODE_SCANNER_H_ */


