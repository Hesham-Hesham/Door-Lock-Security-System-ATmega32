 /******************************************************************************
 *
 * Module: buzzer
 *
 * File Name: buzzer.h
 *
 * Description: Source file for the buzzer driver
 *
 * Author: Hesham
 *
 *******************************************************************************/


#ifndef BUZZER_H_
#define BUZZER_H_

#include "gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define BUZZER_PORT 		PORTC_ID
#define BUZZER_PIN			PIN2_ID



/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description: initialize the buzzer
 */
void Buzzer_init(void);


/*
 * Description: turn on the buzzer
 */
void Buzzer_on(void);


/*
 * Description: turn off the buzzer
 */
void Buzzer_off(void);


#endif /* BUZZER_H_ */
