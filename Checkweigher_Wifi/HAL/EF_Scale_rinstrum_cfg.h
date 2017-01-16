/*
 * EF_Scale_cfg.h
 *
 *  Created on: 13/01/2015
 *      Author: Mohamed Samy
 */

#ifndef EF_SCALE_CFG_H_
#define EF_SCALE_CFG_H_

#include "../MCAL/EF_TIVA_uart.h"
#include "utils/uartstdio.h"
#include <string.h>
/*************************************************
 * 	Macro Definitions
 *************************************************/
#define SCALE_UART 						   UART4

/* Error bit in the first byte of the respone frame */
#define ERROR_BIT                           6
#define RETURN_ERROR_REPONSE                0  //TODO make it any number except 0, made it 0 now because UserInter.c(operation func) in Scale depend on it

/* how many times repeat zeroing to ensure , zeroing happend*/
#define ZEROING_TRAILS    3


#endif /* EF_SCALE_CFG_H_ */
