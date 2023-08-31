/*
 * timer0_pwm.h
 *
 *  Created on: May 8, 2023
 *      Author: Hesham
 */

#ifndef TIMER0_PWM_H_
#define TIMER0_PWM_H_


#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PWM_ENABLE_PORT_ID   PORTB_ID
#define PWM_ENABLE_PIN_ID    PIN3_ID


/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description: start the pwm output with a certain duty cycle value
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* TIMER0_PWM_H_ */
