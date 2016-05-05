################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BitBangMPL.c \
../Bluetooth_RFCOMM.c \
../LCD.c \
../MCP3002SPI.c \
../MPL3115A2.c \
../SerializeDeserialize.c \
../main.c \
../thread.c 

OBJS += \
./BitBangMPL.o \
./Bluetooth_RFCOMM.o \
./LCD.o \
./MCP3002SPI.o \
./MPL3115A2.o \
./SerializeDeserialize.o \
./main.o \
./thread.o 

C_DEPS += \
./BitBangMPL.d \
./Bluetooth_RFCOMM.d \
./LCD.d \
./MCP3002SPI.d \
./MPL3115A2.d \
./SerializeDeserialize.d \
./main.d \
./thread.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


