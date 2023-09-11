################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/HardwareLib/FlashWriteRead.c \
../Sources/HardwareLib/LPUART.c \
../Sources/HardwareLib/LP_IT_timer.c \
../Sources/HardwareLib/RGB_LED.c \
../Sources/HardwareLib/Ymodem.c \
../Sources/HardwareLib/latency.c 

OBJS += \
./Sources/HardwareLib/FlashWriteRead.o \
./Sources/HardwareLib/LPUART.o \
./Sources/HardwareLib/LP_IT_timer.o \
./Sources/HardwareLib/RGB_LED.o \
./Sources/HardwareLib/Ymodem.o \
./Sources/HardwareLib/latency.o 

C_DEPS += \
./Sources/HardwareLib/FlashWriteRead.d \
./Sources/HardwareLib/LPUART.d \
./Sources/HardwareLib/LP_IT_timer.d \
./Sources/HardwareLib/RGB_LED.d \
./Sources/HardwareLib/Ymodem.d \
./Sources/HardwareLib/latency.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/HardwareLib/%.o: ../Sources/HardwareLib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/HardwareLib/FlashWriteRead.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


