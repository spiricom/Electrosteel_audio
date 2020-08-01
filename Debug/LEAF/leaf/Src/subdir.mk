################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LEAF/leaf/Src/leaf-analysis.c \
../LEAF/leaf/Src/leaf-delay.c \
../LEAF/leaf/Src/leaf-distortion.c \
../LEAF/leaf/Src/leaf-dynamics.c \
../LEAF/leaf/Src/leaf-effects.c \
../LEAF/leaf/Src/leaf-electrical.c \
../LEAF/leaf/Src/leaf-envelopes.c \
../LEAF/leaf/Src/leaf-filters.c \
../LEAF/leaf/Src/leaf-instruments.c \
../LEAF/leaf/Src/leaf-math.c \
../LEAF/leaf/Src/leaf-mempool.c \
../LEAF/leaf/Src/leaf-midi.c \
../LEAF/leaf/Src/leaf-oscillators.c \
../LEAF/leaf/Src/leaf-physical.c \
../LEAF/leaf/Src/leaf-reverb.c \
../LEAF/leaf/Src/leaf-sampling.c \
../LEAF/leaf/Src/leaf-tables.c \
../LEAF/leaf/Src/leaf.c 

OBJS += \
./LEAF/leaf/Src/leaf-analysis.o \
./LEAF/leaf/Src/leaf-delay.o \
./LEAF/leaf/Src/leaf-distortion.o \
./LEAF/leaf/Src/leaf-dynamics.o \
./LEAF/leaf/Src/leaf-effects.o \
./LEAF/leaf/Src/leaf-electrical.o \
./LEAF/leaf/Src/leaf-envelopes.o \
./LEAF/leaf/Src/leaf-filters.o \
./LEAF/leaf/Src/leaf-instruments.o \
./LEAF/leaf/Src/leaf-math.o \
./LEAF/leaf/Src/leaf-mempool.o \
./LEAF/leaf/Src/leaf-midi.o \
./LEAF/leaf/Src/leaf-oscillators.o \
./LEAF/leaf/Src/leaf-physical.o \
./LEAF/leaf/Src/leaf-reverb.o \
./LEAF/leaf/Src/leaf-sampling.o \
./LEAF/leaf/Src/leaf-tables.o \
./LEAF/leaf/Src/leaf.o 

C_DEPS += \
./LEAF/leaf/Src/leaf-analysis.d \
./LEAF/leaf/Src/leaf-delay.d \
./LEAF/leaf/Src/leaf-distortion.d \
./LEAF/leaf/Src/leaf-dynamics.d \
./LEAF/leaf/Src/leaf-effects.d \
./LEAF/leaf/Src/leaf-electrical.d \
./LEAF/leaf/Src/leaf-envelopes.d \
./LEAF/leaf/Src/leaf-filters.d \
./LEAF/leaf/Src/leaf-instruments.d \
./LEAF/leaf/Src/leaf-math.d \
./LEAF/leaf/Src/leaf-mempool.d \
./LEAF/leaf/Src/leaf-midi.d \
./LEAF/leaf/Src/leaf-oscillators.d \
./LEAF/leaf/Src/leaf-physical.d \
./LEAF/leaf/Src/leaf-reverb.d \
./LEAF/leaf/Src/leaf-sampling.d \
./LEAF/leaf/Src/leaf-tables.d \
./LEAF/leaf/Src/leaf.d 


# Each subdirectory must supply rules for building sources it contributes
LEAF/leaf/Src/leaf-analysis.o: ../LEAF/leaf/Src/leaf-analysis.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-analysis.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-delay.o: ../LEAF/leaf/Src/leaf-delay.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-delay.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-distortion.o: ../LEAF/leaf/Src/leaf-distortion.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-distortion.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-dynamics.o: ../LEAF/leaf/Src/leaf-dynamics.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-dynamics.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-effects.o: ../LEAF/leaf/Src/leaf-effects.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-effects.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-electrical.o: ../LEAF/leaf/Src/leaf-electrical.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-electrical.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-envelopes.o: ../LEAF/leaf/Src/leaf-envelopes.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-envelopes.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-filters.o: ../LEAF/leaf/Src/leaf-filters.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-filters.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-instruments.o: ../LEAF/leaf/Src/leaf-instruments.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-instruments.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-math.o: ../LEAF/leaf/Src/leaf-math.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-math.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-mempool.o: ../LEAF/leaf/Src/leaf-mempool.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-mempool.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-midi.o: ../LEAF/leaf/Src/leaf-midi.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-midi.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-oscillators.o: ../LEAF/leaf/Src/leaf-oscillators.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-oscillators.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-physical.o: ../LEAF/leaf/Src/leaf-physical.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-physical.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-reverb.o: ../LEAF/leaf/Src/leaf-reverb.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-reverb.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-sampling.o: ../LEAF/leaf/Src/leaf-sampling.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-sampling.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf-tables.o: ../LEAF/leaf/Src/leaf-tables.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf-tables.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
LEAF/leaf/Src/leaf.o: ../LEAF/leaf/Src/leaf.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H743xx -DDEBUG -c -I../Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Middlewares/Third_Party/FatFs/src -I../Drivers/STM32H7xx_HAL_Driver/Inc -I"/Users/josnyder/dev/Electrosteel_audio/LEAF/leaf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"LEAF/leaf/Src/leaf.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

