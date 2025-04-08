################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Applications/WAV_Recorder/wav_recorder.c 

OBJS += \
./Applications/WAV_Recorder/wav_recorder.o 

C_DEPS += \
./Applications/WAV_Recorder/wav_recorder.d 


# Each subdirectory must supply rules for building sources it contributes
Applications/WAV_Recorder/%.o Applications/WAV_Recorder/%.su Applications/WAV_Recorder/%.cyclo: ../Applications/WAV_Recorder/%.c Applications/WAV_Recorder/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32L432xx -DUSE_HAL_DRIVER -c -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Peripherals/Inc" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Core/Inc" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Applications/SD" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/STM32L4xx_HAL_Driver/Inc" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/CMSIS/Include" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Applications-2f-WAV_Recorder

clean-Applications-2f-WAV_Recorder:
	-$(RM) ./Applications/WAV_Recorder/wav_recorder.cyclo ./Applications/WAV_Recorder/wav_recorder.d ./Applications/WAV_Recorder/wav_recorder.o ./Applications/WAV_Recorder/wav_recorder.su

.PHONY: clean-Applications-2f-WAV_Recorder

