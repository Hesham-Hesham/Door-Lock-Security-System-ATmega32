/*
 * APP_CONTROL_ECU.c
 *
 *  Created on: Jul 6, 2023
 *      Author: Hesham
 */

#include <avr/io.h> /* To use the IO Ports Registers */
#include <util/delay.h> /* For the delay functions */


#include "lcd.h"
#include "uart.h"
#include "std_types.h"
#include "external_eeprom.h"
#include "buzzer.h"
#include "dc_motor.h"
#include "timer_1.h"
#include "twi.h"



/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/


#define OFF 0
#define ON 1


//Debugging mode enables/disables the second debugging LCD connected to the control ECU
#define DEBUGGING_MODE ON
//#define DEBUGGING_MODE OFF


#define PASSWORD_SIZE 5
#define address_of_EEPROM  0x0001



/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

uint8 compare_passwords(uint8 *compared_password_1, uint8 *compared_password_2);
void saveToEEPROM(uint16 first_address , uint8 *arr,uint8 size);
void readFromEEPROM(uint16 first_address , uint8 *arr,uint8 size);
void Timer_1_CallBack(void);
void Timer_1_delay(uint32 delay);




/*******************************************************************************
 *                                Variable Definitions                         *
 *******************************************************************************/

uint8 control_stage=0;

uint8  volatile seconds = 0;

//To store the received data from the UART
uint8 UART_recievedData=0;

//To store the number of times a wrong password was entered
uint8 worng_password_counter=0;

//To store the received password_1 from the user
uint8 recived_password_1[PASSWORD_SIZE]={0};

//To store the received password_2 from the user
uint8 recived_password_2[PASSWORD_SIZE]={0};

//To store the password that was set by the user on startup
uint8 EEPROM_password[PASSWORD_SIZE]={0};

//Initialization structure for UART
UART_ConfigType UART_HMI={UART_8_Bits,UART_PARITY_EVEN,UART_STOPBIT_1_BIT,9600,UART_ASYNCHRONOUS};

//Initialization structure for TIMER1
Timer1_ConfigType Timer1_ConfigStruct = {0,31250,F_CPU_256,CTC_OCR};

//Initialization structure for TWI
TWI_ConfigType TWI_ConfigStruct = {0b00000010,0x02};

int main(){


	//initializing drivers
	Buzzer_init();
	DCMotor_Init();
	UART_init(&UART_HMI);
	TWI_init(&TWI_ConfigStruct);

	//to store the return value of the comparison function
	uint8 compare_passwords_return;



	///////////////////////////////////***///////////////////////////////////
	///////////////////////////////////***///////////////////////////////////
	///////////////////////////////////***///////////////////////////////////
	//Enable global interrupt	Enable global interrupt	Enable global interrupt
	SREG |= (1<<7);
	//Enable global interrupt	Enable global interrupt	Enable global interrupt
	///////////////////////////////////***///////////////////////////////////
	///////////////////////////////////***///////////////////////////////////
	///////////////////////////////////***///////////////////////////////////



#if (DEBUGGING_MODE == ON)

	//Initialize the LCD driver in case of the debugging mode being used
	LCD_init();
	LCD_displayString("Debugging...");
#endif

	for(;;){

		//the main switch that determines in which state the program runs
		switch(control_stage){
		case 0:							//setting password
			//Receive first password
			UART_recieveArray(recived_password_1,PASSWORD_SIZE);
#if (DEBUGGING_MODE == ON)

			//Display the first password
			LCD_clearScreen();
			LCD_displayArray(recived_password_1, PASSWORD_SIZE);
			_delay_ms(500);
#endif

			//Receive second password
			UART_recieveArray(recived_password_2,PASSWORD_SIZE);
#if (DEBUGGING_MODE == ON)

			//Display the second password
			LCD_clearScreen();
			LCD_displayArray(recived_password_2, PASSWORD_SIZE);
			_delay_ms(500);
#endif

			//compare passwords
			LCD_clearScreen();
			compare_passwords_return=compare_passwords(recived_password_1, recived_password_2);
			_delay_ms(1000);
			LCD_clearScreen();
			if(compare_passwords_return==1){
				//since the passwords match move to the next control state
				control_stage=1;
			}
			//Send the decision to the APP ECU
			UART_sendByte(compare_passwords_return);
			break;


		case 1:							//Receiving password to open the door or change password

			//Receive third password (from + or - options)
			UART_recieveArray(recived_password_1,PASSWORD_SIZE);
#if (DEBUGGING_MODE == ON)

			//Display the password
			LCD_clearScreen();
			LCD_displayArray(recived_password_1, PASSWORD_SIZE);
			_delay_ms(500);
#endif

			//Read the password saved in the EEPROM
			readFromEEPROM(address_of_EEPROM, EEPROM_password, PASSWORD_SIZE);
			//Compare the password saved in the EEPROM to the password entered by the user
			compare_passwords_return=compare_passwords(recived_password_1, EEPROM_password);

			//Send the decision to the APP ECU
			UART_sendByte(compare_passwords_return);


			if(compare_passwords_return==1){	//passwords match
				//receive the choice from user
				UART_recievedData=UART_recieveByte();


				if(UART_recievedData==2){		//motor choice was selected
#if (DEBUGGING_MODE == ON)

					LCD_clearScreen();
					LCD_displayString("do motor sequence");
#endif

					//Do the motor sequence
					DCMotor_Rotate(DC_MOTOR_CW, 100);
					Timer_1_delay(15);
					DCMotor_Rotate(DC_MOTOR_STOP,0);
					Timer_1_delay(3);
					DCMotor_Rotate(DC_MOTOR_ACW, 100);
					Timer_1_delay(15);
					DCMotor_Rotate(DC_MOTOR_STOP,0);
					LCD_clearScreen();

				}else if(UART_recievedData==1){	//change password choice was selected

#if (DEBUGGING_MODE == ON)

					LCD_clearScreen();
					LCD_displayString("changing password");
					Timer_1_delay(1);
#endif

					//Do the changing password sequence (go back to the starting stage "stage 0")
					control_stage=0;
				}

			}else{							//passwords dont match

				//Increment the wrong password counter
				worng_password_counter++;

				//The maximum number of wrong passwords is 3
				if(worng_password_counter==3){	//maximum no. of wrong passwords was reached

					//Move to the alarm state "state 2"
					control_stage=2;
					Timer_1_delay(1);
					break;
				}


#if (DEBUGGING_MODE == ON)

				LCD_clearScreen();
				LCD_displayString("wrong");
				LCD_moveCursor(1, 0);
				LCD_intgerToString(worng_password_counter);
#endif

				_delay_ms(1000);
			}
			break;

		case 2:							//Alarm state
#if (DEBUGGING_MODE == ON)

			LCD_clearScreen();
			LCD_displayString("Alarm");
			_delay_ms(1000);
#endif

			//Do the alarm sequence since the max no of passwords was reached
			Buzzer_on();
			//timer
			Timer_1_delay(6);
			Buzzer_off();
			//Reset the stage to (the receiving password stage "stage 1")
			control_stage = 1;

			//Reset the wrong password counter
			worng_password_counter=0;
			LCD_clearScreen();

			//Send that the alarm has elapsed to the HMI ECU
			UART_sendByte(1); //alarm has elapsed
			break;

		}
	}

}



/*
 * Description: Compare between two given passwords (arrays) and return the comparison result
 */
uint8 compare_passwords(uint8 *compared_password_1, uint8 *compared_password_2){

	uint8 i;	//Counter for the "for" loop

#if (DEBUGGING_MODE == ON)

	LCD_clearScreen();
	LCD_displayString("comparing");
	LCD_displayStringRowColumn(1, 0, "passwords");
	_delay_ms(1000);
	LCD_clearScreen();

#endif

	//Loop through all the password array elements
	for(i=0;i<PASSWORD_SIZE;i++){

		//Compare between the element in array1 and the element in array2 in the same place
		if(compared_password_1[i]!=compared_password_2[i]){		//The passwords don't match

#if (DEBUGGING_MODE == ON)
			LCD_clearScreen();
			LCD_displayString("passwords don't");
			LCD_displayStringRowColumn(1, 0, "match");
#endif

			//The passwords don't match so return 0 and exit
			return 0;
		}else{													//The passwords match
			//continue and do nothing
		}
	}


#if (DEBUGGING_MODE == ON)
	LCD_displayString("passwords match");
#endif


	if(control_stage==0){	//if we're in stage 0 (the password creation stage)

		//save one of them to EEPROM since: 1)they match  2)we're in stage 0
		saveToEEPROM(address_of_EEPROM,compared_password_1,PASSWORD_SIZE);

		//		for(i=0;i<PASSWORD_SIZE;i++){
		//			EEPROM_password[i]=compared_password_1[i];
		//		}
	}
	if(worng_password_counter==2){	//If we'll be in the last trial the next iteration (max is 3)
		return 2;
	}

	//Return 1 if we're in any other state
	return 1;
}



/*
 * Description: Save a given array to EEPROM
 */
void saveToEEPROM(uint16 first_address , uint8 *arr,uint8 size){

	uint8 i;
	for(i=0;i<size;i++){

		EEPROM_writeByte(first_address+i,arr[i]);
		_delay_ms(10);
	}

}


/*
 * Description: Read an array from EEPROM
 */
void readFromEEPROM(uint16 first_address , uint8 *arr,uint8 size){

	uint8 i=0;
	for(i=0;i<size;i++){

		EEPROM_readByte(first_address+i,&arr[i]);
		_delay_ms(10);
	}

}




/*
 * Description: Delay the code execution for a specific time interval
 */
void Timer_1_delay(uint32 delay){

	//Initialize timer1 with the configuration structure
	Timer_1_init(&Timer1_ConfigStruct);

	//Set the callback function for timer1
	Timer_1_setCallBack(&Timer_1_CallBack);

	//Stay in the loop as long as the required number of delay seconds hasn't elapsed
	while(seconds != delay);

	//Deinitialize timer1
	Timer_1_deInit();

	//Reset the seconds variable
	seconds = 0;
}


/*
 * Description: increment the seconds variable each time the function is called
 */
void Timer_1_CallBack(void){
	seconds++;
}
