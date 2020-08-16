################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LEAF/leaf/Externals/d_fft_mayer.c 

OBJS += \
./LEAF/leaf/Externals/d_fft_mayer.o 

C_DEPS += \
./LEAF/leaf/Externals/d_fft_mayer.d 


# Each subdirectory must supply rules for building sources it contributes
LEAF/leaf/Externals/d_fft_mayer.o: ../LEAF/leaf/Externals/d_fft_mayer.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -Ofast -ffunction-sections -fdata-sections -fno-strict-aliasing -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Externals/d_fft_mayer.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

