################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP_CONTROL_ECU.c \
../buzzer.c \
../dc_motor.c \
../external_eeprom.c \
../gpio.c \
../lcd.c \
../timer0_pwm.c \
../timer_1.c \
../twi.c \
../uart.c 

OBJS += \
./APP_CONTROL_ECU.o \
./buzzer.o \
./dc_motor.o \
./external_eeprom.o \
./gpio.o \
./lcd.o \
./timer0_pwm.o \
./timer_1.o \
./twi.o \
./uart.o 

C_DEPS += \
./APP_CONTROL_ECU.d \
./buzzer.d \
./dc_motor.d \
./external_eeprom.d \
./gpio.d \
./lcd.d \
./timer0_pwm.d \
./timer_1.d \
./twi.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


