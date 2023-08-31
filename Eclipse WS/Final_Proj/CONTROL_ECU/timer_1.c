/*
 * timer_1.c
 *
 *  Created on: Jul 8, 2023
 *      Author: Hesham
 */


#include <avr/io.h> /* To use the IO Ports Registers */
#include <avr/interrupt.h>


#include "timer_1.h"
#include "common_macros.h"




static volatile void (*g_CallBackPtr)(void) = NULL_PTR;	//global pointer to function to be used for the cb


ISR(TIMER1_OVF_vect){
	g_CallBackPtr();
}

ISR(TIMER1_COMPA_vect){
	g_CallBackPtr();
}



/*
 * Description : Initialize timer1 based on a given config structure
 *
 */
void Timer_1_init(const Timer1_ConfigType * Config_Ptr){

	//in normal mode WGM10 WGM11 WGM12 WGM13 are all 0 so we can set them all to 0 by default
	CLEAR_BIT(TCCR1A,WGM10);
	CLEAR_BIT(TCCR1A,WGM11);
	CLEAR_BIT(TCCR1B,WGM12);
	CLEAR_BIT(TCCR1B,WGM13);


	if(Config_Ptr->mode == CTC_OCR){		//CTC mode was selected

		//in CTC mode only WGM12 is 1
		SET_BIT(TCCR1B,WGM12);

		//assign the compare value to OCR1A
		OCR1A = Config_Ptr->compare_value;

		//enable interrupt for OCIE1A
		SET_BIT(TIMSK,OCIE1A);

	}else if(Config_Ptr->mode == NORMAL){	//Normal mode was selected

		//enable interrupt for TOIE1
		SET_BIT(TIMSK,TOIE1);

	}

	//only active when using non-PWM mode
	SET_BIT(TCCR1A,FOC1A);
	SET_BIT(TCCR1A,FOC1B);

	//assign initial value
	TCNT1 = Config_Ptr->initial_value;


//	TCCR1B 0 to 2 clock select
//	clock bits are 0 1 2    0000 0111 = f7
//							1111 1000 = ~f7
	TCCR1B = (TCCR1B & ~0xf7) | ( ( (Config_Ptr->prescaler) <<0) & 0xf7 );


}



/*
 * Description : Deinitialize timer1
 *
 */
void Timer_1_deInit(void){
	g_CallBackPtr = NULL_PTR;
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;
	TIMSK &= 0x3c;
}



/*
 * Description : Set the callback function for timer1
 *
 */
void Timer_1_setCallBack(void(*a_ptr)(void)){
	g_CallBackPtr=a_ptr;
}
