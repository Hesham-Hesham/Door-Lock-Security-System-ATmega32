/*
 * dc_motor.c
 *
 *  Created on: May 8, 2023
 *      Author: Hesham
 */

#include "gpio.h"
#include "dc_motor.h"
#include "timer0_pwm.h"



/*
 * Description: initialize the motor driver
 */
void DCMotor_Init(void){

	//set the enable pin as output
	GPIO_setupPinDirection(DC_MOTOR_1_ENABLE_PORT, DC_MOTOR_1_ENABLE_PIN, PIN_OUTPUT);

	//set the IN pins as output
	GPIO_setupPinDirection(DC_MOTOR_1_IN_PORT, DC_MOTOR_1_IN_FIRST_PIN+0, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_1_IN_PORT, DC_MOTOR_1_IN_FIRST_PIN+1, PIN_OUTPUT);

	//Stop the motor
	GPIO_writePin(DC_MOTOR_1_ENABLE_PORT, DC_MOTOR_1_ENABLE_PIN, DC_MOTOR_STOP);
}



/*
 * Description: set the motor direction and rotation speed
 */
void DCMotor_Rotate(DcMotor_State state,uint8 speed){

	switch(state){
		case DC_MOTOR_CW:
			//set the direction as CW by the IN pins
			GPIO_writePin(DC_MOTOR_1_IN_PORT, DC_MOTOR_1_IN_FIRST_PIN+0, LOGIC_HIGH);
			GPIO_writePin(DC_MOTOR_1_IN_PORT, DC_MOTOR_1_IN_FIRST_PIN+1, LOGIC_LOW);
			break;

		case DC_MOTOR_ACW:
			//set the direction as ACW by the IN pins
			GPIO_writePin(DC_MOTOR_1_IN_PORT, DC_MOTOR_1_IN_FIRST_PIN+0, LOGIC_LOW);
			GPIO_writePin(DC_MOTOR_1_IN_PORT, DC_MOTOR_1_IN_FIRST_PIN+1, LOGIC_HIGH);
			break;

		case DC_MOTOR_STOP:
			//Stop the motor by the IN pins
			GPIO_writePin(DC_MOTOR_1_IN_PORT, DC_MOTOR_1_IN_FIRST_PIN+0, LOGIC_LOW);
			GPIO_writePin(DC_MOTOR_1_IN_PORT, DC_MOTOR_1_IN_FIRST_PIN+1, LOGIC_LOW);
			break;
	}

	//set the motor speed by passing the PWM value to the function
	PWM_Timer0_Start(speed);

}
