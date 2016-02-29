################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ds1820/crc8.c \
../src/ds1820/delay.c \
../src/ds1820/ds18x20.c \
../src/ds1820/onewire.c 

OBJS += \
./src/ds1820/crc8.o \
./src/ds1820/delay.o \
./src/ds1820/ds18x20.o \
./src/ds1820/onewire.o 

C_DEPS += \
./src/ds1820/crc8.d \
./src/ds1820/delay.d \
./src/ds1820/ds18x20.d \
./src/ds1820/onewire.d 


# Each subdirectory must supply rules for building sources it contributes
src/ds1820/%.o: ../src/ds1820/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega168 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


