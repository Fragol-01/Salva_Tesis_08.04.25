################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Applications/SD/audio_recorder.c \
../Applications/SD/fatfs_sd.c \
../Applications/SD/write_sector_0.c 

OBJS += \
./Applications/SD/audio_recorder.o \
./Applications/SD/fatfs_sd.o \
./Applications/SD/write_sector_0.o 

C_DEPS += \
./Applications/SD/audio_recorder.d \
./Applications/SD/fatfs_sd.d \
./Applications/SD/write_sector_0.d 


# Each subdirectory must supply rules for building sources it contributes
Applications/SD/%.o Applications/SD/%.su Applications/SD/%.cyclo: ../Applications/SD/%.c Applications/SD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32L432xx -DUSE_HAL_DRIVER -c -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Peripherals/Inc" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Core/Inc" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Applications/SD" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/STM32L4xx_HAL_Driver/Inc" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/CMSIS/Include" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Applications-2f-SD

clean-Applications-2f-SD:
	-$(RM) ./Applications/SD/audio_recorder.cyclo ./Applications/SD/audio_recorder.d ./Applications/SD/audio_recorder.o ./Applications/SD/audio_recorder.su ./Applications/SD/fatfs_sd.cyclo ./Applications/SD/fatfs_sd.d ./Applications/SD/fatfs_sd.o ./Applications/SD/fatfs_sd.su ./Applications/SD/write_sector_0.cyclo ./Applications/SD/write_sector_0.d ./Applications/SD/write_sector_0.o ./Applications/SD/write_sector_0.su

.PHONY: clean-Applications-2f-SD

