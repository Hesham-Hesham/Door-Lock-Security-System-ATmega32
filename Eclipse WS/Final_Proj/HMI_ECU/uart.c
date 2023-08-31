 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 *
 *******************************************************************************/

#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */




/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device
 */
void UART_init(UART_ConfigType *UART_CONFIG_PTR)
{
	uint16 ubrr_value = 0;

//	for double transmission speed
	UCSRA = (1<<U2X);


//	 Receiver Enable + Transmitter Enable
	UCSRB = (1<<RXEN) | (1<<TXEN);
	

//	 * URSEL   The URSEL must be one while writing the UCSRC
	SET_BIT(UCSRC,URSEL);


//	 * UMSEL   Operation mode
//	 * UPM1:0  parity bit
//	 * USBS    No of stop bits
//	 * UCSZ1:0 For data mode
//	 * UCPOL   Used with the Synchronous operation only
	UCSRC =( (UCSRC & ~0x40) | ( ( (UART_CONFIG_PTR->synchronization)<<UMSEL) & 0x40 ) );
	UCSRC =( (UCSRC & ~0x30) | ( ( (UART_CONFIG_PTR->parity)<<UPM0) & 0x30) );
	UCSRC =( (UCSRC & ~0x08) | ( ( (UART_CONFIG_PTR->stop_bit)<<USBS) & 0x08) );
	UCSRC =( (UCSRC & ~0x06) | ( ( (UART_CONFIG_PTR->bit_data)<<UCSZ0) & 0x06) );


	
	/* Calculate the UBRR register value */
	ubrr_value = (uint16)(((F_CPU / (UART_CONFIG_PTR->baud_rate * 8UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value>>8;
	UBRRL = ubrr_value;
}

/*
 * Description :
 * Function responsible for sending a byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{
	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}

	/*
	 * Put the required data in the UDR register
	 */
	UDR = data;

}

/*
 * Description :
 * Functional responsible for receiving a byte from another UART device.
 */
uint8 UART_recieveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC)){}

    return UDR;		
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_recieveByte();

	/* Receive the whole string until the '#' */
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}
