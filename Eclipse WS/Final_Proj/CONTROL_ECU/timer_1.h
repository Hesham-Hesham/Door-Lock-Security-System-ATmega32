/*
 * timer_1.h
 *
 *  Created on: Jul 8, 2023
 *      Author: Hesham
 */

#ifndef TIMER_1_H_
#define TIMER_1_H_

#include "std_types.h"


/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

typedef enum {
	CLOCK_0, F_CPU_1, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024, EXTERNAL_FALLING, EXTERNAL_RISING
}Timer1_Prescaler;

typedef enum {
	NORMAL, CTC_OCR = 4
}Timer1_Mode;

typedef struct {
	uint16 initial_value;
	uint16 compare_value; // it will be used in compare mode only.
	Timer1_Prescaler prescaler;
	Timer1_Mode mode;
}Timer1_ConfigType;


/*******************************************************************************
 *                     			 Functions Prototypes                          *
 *******************************************************************************/



/*
 * Description : Initialize timer1 based on a given config structure
 *
 */
void Timer_1_init(const Timer1_ConfigType * Config_Ptr);

/*
 * Description : Deinitialize timer1
 *
 */
void Timer_1_deInit(void);

/*
 * Description : Set the callback function for timer1
 *
 */
void Timer_1_setCallBack(void(*a_ptr)(void));



#endif /* TIMER_1_H_ */
