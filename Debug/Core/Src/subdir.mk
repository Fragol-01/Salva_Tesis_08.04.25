################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Recording_WAV_files.c \
../Core/Src/delete_sectors.c \
../Core/Src/main.c \
../Core/Src/printf_redirect.c \
../Core/Src/raw_test.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/write_sd_test.c 

OBJS += \
./Core/Src/Recording_WAV_files.o \
./Core/Src/delete_sectors.o \
./Core/Src/main.o \
./Core/Src/printf_redirect.o \
./Core/Src/raw_test.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/write_sd_test.o 

C_DEPS += \
./Core/Src/Recording_WAV_files.d \
./Core/Src/delete_sectors.d \
./Core/Src/main.d \
./Core/Src/printf_redirect.d \
./Core/Src/raw_test.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/write_sd_test.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32L432xx -DUSE_HAL_DRIVER -c -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Peripherals/Inc" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Core/Inc" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Applications/SD" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/STM32L4xx_HAL_Driver/Inc" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/CMSIS/Include" -I"C:/Users/DANNY/STM32CubeIDE/workspace_1.12.0/AVANCE_Tesis040425/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Recording_WAV_files.cyclo ./Core/Src/Recording_WAV_files.d ./Core/Src/Recording_WAV_files.o ./Core/Src/Recording_WAV_files.su ./Core/Src/delete_sectors.cyclo ./Core/Src/delete_sectors.d ./Core/Src/delete_sectors.o ./Core/Src/delete_sectors.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/printf_redirect.cyclo ./Core/Src/printf_redirect.d ./Core/Src/printf_redirect.o ./Core/Src/printf_redirect.su ./Core/Src/raw_test.cyclo ./Core/Src/raw_test.d ./Core/Src/raw_test.o ./Core/Src/raw_test.su ./Core/Src/stm32l4xx_hal_msp.cyclo ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_hal_msp.su ./Core/Src/stm32l4xx_it.cyclo ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.cyclo ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su ./Core/Src/write_sd_test.cyclo ./Core/Src/write_sd_test.d ./Core/Src/write_sd_test.o ./Core/Src/write_sd_test.su

.PHONY: clean-Core-2f-Src

