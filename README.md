# Door Lock Security System - ATmega32

The Door Lock Security System project aims to create a robust and secure mechanism for unlocking a door using a password. The project leverages two ATmega32 Microcontrollers, each operating at 8MHz, and is designed based on a layered architecture model for efficient and modular functionality.

## Table of Contents

- [System Overview](#system-overview)
- [Requirements](#requirements)
- [Components](#components)
- [Circuit Diagram](#circuit-diagram)
- [Usage](#usage)


## System Overview

The project consists of two main components:

1. **HMI_ECU (Human Machine Interface):**
   - Equipped with a 2x16 LCD and a 4x4 keypad.
   - Responsible for user interaction, taking inputs through the keypad, and displaying messages on the LCD.

2. **Control_ECU:**
   - Incorporates an EEPROM for data storage, a Buzzer for alarms, and a DC-Motor for door control.
   - Manages all processing and decision-making tasks, such as password checking, door opening, and system alarm activation.

## Requirements

For the full detailed system sequence and components requirements, check [The Project requirements file](https://github.com/Hesham-Hesham/Door-Lock-Security-System-ATmega32/blob/main/System%20Requirements.pdf)

### Drivers' Requirements

- GPIO Driver
- LCD Driver
- Keypad Driver: Use 4x4 keypad.
- DC_Motor Driver
- EEPROM Driver: Use external EEPROM controlled via I2C.
- I2C Driver
- UART Driver
- Timer Driver: Implement Timer1 driver with callbacks for time-based operations.
- Buzzer Driver: Implement Buzzer driver for controlling the buzzer.


### Layered Architecture
- CONTROL_ECU

![image](https://github.com/Hesham-Hesham/Door-Lock-Security-System-ATmega32/assets/91581641/4affa2a5-1305-4d8b-b6e4-93323a2e3eb5)

---
- HMI_ECU

![image](https://github.com/Hesham-Hesham/Door-Lock-Security-System-ATmega32/assets/91581641/8bba2e28-059d-4176-b0e0-827b07959e0d)


## Components

- HMI_ECU: 2x16 LCD, 4x4 keypad.
- Control_ECU: EEPROM, Buzzer, DC-Motor.
- Two ATmega32 Microcontrollers.

## Circuit Diagram
![circuit](https://github.com/Hesham-Hesham/Door-Lock-Security-System-ATmega32/assets/91581641/2cfc658d-31db-4d8d-9026-3ac78f1d3aa8)


## Usage
- ### Hardware target
    In order to run the code on a real hardware target (ARmega32), follow these steps:
1. **Setup**: Connect the components as per the circuit diagram.
2. **Compile**: Use an appropriate compiler (e.g., AVR GCC) to compile the provided source code.
3. **Program**: Program the ATmega32 Microcontroller with the compiled binary using a suitable programmer (e.g., AVR ISP).
4. **Run**: Power up the circuit. The Stopwatch will start counting immediately.
5. **User Interaction**: Start by creating a password of 5 digits, then make a choice of either opening the door or changing the password



- ### Simulation
1.  **Setup**: Install proteus 8 professional v10.0 or higher
2. **Install Add-ons**: Add the necessary arduino and avr add-ons (Arduino_Proteus_Libraries)
3. **Run**: Run the .pdsprj file in the /Proteus Simulation folder in any of the versions 

<br/><br/>

---

This project was made during the Full Embedded Systems Diploma under the supervision of Engineer Mohamed Tarek.
