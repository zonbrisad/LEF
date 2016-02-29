################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/commandInterpreter.c \
../src/main.c \
../src/queue.c \
../src/sev_timer.c 

OBJS += \
./src/commandInterpreter.o \
./src/main.o \
./src/queue.o \
./src/sev_timer.o 

C_DEPS += \
./src/commandInterpreter.d \
./src/main.d \
./src/queue.d \
./src/sev_timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


