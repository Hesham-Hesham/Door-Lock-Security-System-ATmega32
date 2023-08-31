/*
 * dc_motor.h
 *
 *  Created on: May 8, 2023
 *      Author: Hesham
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"
#include "gpio.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define DC_MOTOR_1_ENABLE_PORT		PORTB_ID
#define DC_MOTOR_1_ENABLE_PIN		PIN3_ID

#define DC_MOTOR_1_IN_PORT			PORTB_ID
#define DC_MOTOR_1_IN_FIRST_PIN		PIN1_ID


/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

//To determine the rotation direction
typedef enum
{

	DC_MOTOR_STOP,DC_MOTOR_CW,DC_MOTOR_ACW
}DcMotor_State;



/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description: initialize the motor driver
 */
void DCMotor_Init(void);



/*
 * Description: set the motor direction and rotation speed
 */
void DCMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DC_MOTOR_H_ */
