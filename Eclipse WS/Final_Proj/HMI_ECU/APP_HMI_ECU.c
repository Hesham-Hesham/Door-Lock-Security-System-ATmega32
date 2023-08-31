/*
 * APP_HMI_ECU.c
 *
 *  Created on: Jun 22, 2023
 *      Author: Hesham
 */


#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "std_types.h"
#include "timer_1.h"


#include <util/delay.h> /* For the delay functions */
#include <avr/io.h> /* To use the IO Ports Registers */


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PASSWORD_SIZE 5



/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

void send_password(uint8 *password_arr,uint8 size);
void enter_password(void);
void enter_doorOrChange_choice(void);
void operate_choice(void);
void Timer_1_CallBack(void);
void Timer_1_delay(uint32 delay);



/*******************************************************************************
 *                                Variable Definitions                         *
 *******************************************************************************/

// To store the pressed key retrieved from the keypad
uint8 pressedKey = 0;

// A counter for the password array
uint8 i=0;

// To store at which stage the code operates in
uint8 stage=0;

// To count how many seconds have elapsed
uint8  volatile seconds = 0;

// To store the received password from the user
uint8 entered_password[PASSWORD_SIZE]={0};

// To store the return value of the user choice function
uint8 enter_doorOrChange_choice_return=0;
uint8 dummy_1=0;

// To store the received data from the UART
uint8 UART_recievedData=0;

//Initialization structure for UART
UART_ConfigType UART_HMI={UART_8_Bits,UART_PARITY_EVEN,UART_STOPBIT_1_BIT,9600,UART_ASYNCHRONOUS};

//Initialization structure for TIMER1
Timer1_ConfigType Timer1_ConfigStruct = {0,31250,F_CPU_256,CTC_OCR};

int main(){

	//initializing drivers
	LCD_init();
	UART_init(&UART_HMI);

	//Show the first LCD message
	LCD_displayString("Pls enter pass:");
	LCD_moveCursor(1,0); /* Move the cursor to the second row */



	///////////////////////////////////***///////////////////////////////////
	///////////////////////////////////***///////////////////////////////////
	///////////////////////////////////***///////////////////////////////////
	//Enable global interrupt	Enable global interrupt	Enable global interrupt
	SREG |= (1<<7);
	//Enable global interrupt	Enable global interrupt	Enable global interrupt
	///////////////////////////////////***///////////////////////////////////
	///////////////////////////////////***///////////////////////////////////
	///////////////////////////////////***///////////////////////////////////



	for(;;){

		//the main switch that determines in which state the program runs
		switch(stage){

		case 0:				//creating password 1st time
			enter_password();
			break;


		case 1:				//creating password 2st time
			enter_password();
			break;

		case 2:				//waiting for order from CONTROL_ECU
			UART_recievedData=UART_recieveByte();
			if(UART_recievedData==0){		//passwords don't match
				LCD_clearScreen();
				LCD_displayString("passwords don't");
				LCD_displayStringRowColumn(1, 0, "match");
				Timer_1_delay(1);

				LCD_clearScreen();
				LCD_displayString("Pls enter pass:");
				LCD_moveCursor(1,0); /* Move the cursor to the second row */

				//Reset the stage so the user can enter the password again
				stage=0;


			}else if(UART_recievedData==1){	//passwords match
				LCD_displayString("passwords match");
				Timer_1_delay(1);

				//The two passwords match and password creation is complete so move to the user choice stage
				stage=3;
			}
			break;

		case 3:				//Check user choice (open door or change password)
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "+ : Open Door");
			LCD_displayStringRowColumn(1, 0, "- : Change Pass");

			//Reset the password array counter
			i=0;

			//Take the input from user
			enter_doorOrChange_choice();
			dummy_1=enter_doorOrChange_choice_return;

			//User selected a choice so move to the next stage and make him enter the password
			stage = 4;
			break;

		case 4:				//Checking password to either open door or change password
			enter_password();

			break;

		case 5:				//waiting for order from CONTROL_ECU

			UART_recievedData=UART_recieveByte();

			if(UART_recievedData==0){		//passwords don't match

				LCD_clearScreen();
				LCD_displayString("Pls enter pass:");
				LCD_moveCursor(1,0); /* Move the cursor to the second row */

				//User entered a wrong password so ask him again
				stage=6;

			}else if(UART_recievedData==1){	//passwords match
				LCD_clearScreen();
				LCD_displayString("passwords match");

				Timer_1_delay(1);

				//Tell the control ECU that the choice is ready to be operated
				UART_sendByte(dummy_1);
				operate_choice();

			}
			break;


		case 6:			//wrong password

			enter_password();

			break;

		case 7:			//confirm if password is wrong or right
			UART_recievedData=UART_recieveByte();


			if(UART_recievedData==0){		//passwords don't match

				LCD_clearScreen();
				LCD_displayString("Pls enter pass:");
				LCD_moveCursor(1,0); /* Move the cursor to the second row */

				//Password is wrong again so ask the user to enter it again
				stage=6;

			}else if(UART_recievedData==1){		//passwords finally match

				//Password is right so operate the user's choice
				operate_choice();

				//Go back to the main menu
				stage = 3;
			}else{

				//Password is wrong for the third time so move to the alarm state
				stage = 8;
			}
			break;

		case 8:		//locked + alarm
			LCD_clearScreen();
			LCD_displayString("Error");
			Timer_1_delay(1);

			//Wait for an order from the control ECU
			UART_recievedData=UART_recieveByte();
			if(UART_recievedData==1){		//locked state and alarm time have elapsed

				//Go back to the main choice menu
				stage = 3;
				LCD_clearScreen();

			}else{							//Alarm hasn't elapsed
				//keep waiting
			}
			break;
		}
	}
}


/*
 * Description : Take the input password from the user and store it in an array
 *
 */
void enter_password(void){

	//Take input from the keypad
	pressedKey = KEYPAD_getPressedKey();

	//Store the input character taken from keypad in the array in index "i"
	entered_password[i]=pressedKey;
	i++;

	//Make sure when the user enters numbers that they're hidden for privacy
	if((pressedKey <= 9) && (pressedKey >= 0))
	{
		LCD_displayString("*");   /* display the pressed keypad switch */


	// "13" is the On key on the keypad and here it represents the (enter) key
	}else if(pressedKey == 13){

		//make sure that the user enters 5 numbers only which is the required password length
		if( i - 1 == 5){
			if(stage==0){
				/*if we're in stage 0 then send the entered password to the control ECU and
				ask the user to enter the password again*/
				LCD_clearScreen();
				send_password(entered_password, PASSWORD_SIZE);
				LCD_displayString("plz re-enter the");
				LCD_displayStringRowColumn(1, 0, "same pass: ");

				//move to the next stage (enter the password for a second time)
				stage = 1;

				//reset the array counter
				i=0;
			}else if(stage==1){
				/* if we're in stage 1 then  send the second entered password to the control ECU
				and move to the next stage */
				LCD_clearScreen();
				send_password(entered_password, PASSWORD_SIZE);

				//move to the next stage (wait for an order from the control ECU)
				stage = 2;

				//reset the array counter
				i=0;
			}
			else if(stage==3){
				LCD_clearScreen();
				send_password(entered_password, PASSWORD_SIZE);
			}
			else if(stage==4){
				/* if we're in stage 4 then  send the entered password to the control ECU
				and move to the next stage */
				LCD_clearScreen();
				send_password(entered_password, PASSWORD_SIZE);

				//move to the next stage (wait for an order from the control ECU)
				stage = 5;

				//reset the array counter
				i = 0;
			}else if(stage==6){
				/* if we're in stage 6 then  send the entered password to the control ECU
				and move to the next stage */
				LCD_clearScreen();
				send_password(entered_password, PASSWORD_SIZE);

				//move to the next stage (confirm if password is wrong or right)
				stage = 7;

				//reset the array counter
				i=0;
			}

		}else{	//number of entered characters is not 5 digits long
			LCD_clearScreen();
			LCD_displayString("Err:Password has to");
			LCD_displayStringRowColumn(1, 0, "be 5 digits long");
		}

	}
	//	else
	//	{
	//		LCD_clearScreen();
	//		LCD_displayString("Err:Enter numbers");
	//		LCD_displayStringRowColumn(1, 0, "only");
	//
	//	}
	_delay_ms(500); /* Press time */


}



/*
 * Description : Send the password by UART to the control ECU
 *
 */
void send_password(uint8 *password_arr,uint8 size){
	uint8 i;
	for(i=0;i<PASSWORD_SIZE;i++){
		UART_sendByte(password_arr[i]);
	}
}


/*
 * Description : Take the input choice from user
 *
 */
void enter_doorOrChange_choice(void){
	pressedKey = KEYPAD_getPressedKey();
	_delay_ms(500); /* Press time */

	if(pressedKey == '+'){
		LCD_clearScreen();
		LCD_displayString("Pls enter pass:");
		LCD_moveCursor(1,0); /* Move the cursor to the second row */
		enter_doorOrChange_choice_return = 2;
	}else if(pressedKey == '-'){
		LCD_clearScreen();
		LCD_displayString("Pls enter pass:");
		LCD_moveCursor(1,0); /* Move the cursor to the second row */
		enter_doorOrChange_choice_return = 1;
	}else{
		LCD_clearScreen();
		LCD_displayString("Err:Enter + or -");
		LCD_displayStringRowColumn(1, 0, "only");
		enter_doorOrChange_choice_return = 0;
	}
}



/*
 * Description : Operate the choice that came from the control ECU
 *
 */
void operate_choice(void){
	if(dummy_1==2){		//+ : Open Door
		LCD_clearScreen();
		LCD_displayString("Door is Unlocking");
		Timer_1_delay(15);
		LCD_clearScreen();

		LCD_displayString("holding motor");
		Timer_1_delay(3);
		LCD_clearScreen();

		LCD_displayString("Door is locking");
		Timer_1_delay(15);
		LCD_clearScreen();

		//reset the array counter
		i=0;

		//move back to the main choice menu
		stage = 3;
	}
	else if(dummy_1==1){	//- : Change Pass

		LCD_clearScreen();
		LCD_displayString("Changing password");
		Timer_1_delay(1);

		LCD_clearScreen();
		LCD_displayString("Pls enter pass:");
		LCD_moveCursor(1,0); /* Move the cursor to the second row */

		//reset the array counter
		i=0;

		//move back to the main password creation stage
		stage=0;
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

