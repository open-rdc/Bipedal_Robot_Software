################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/ODrive/ODrive.c 

OBJS += \
./Core/ODrive/ODrive.o 

C_DEPS += \
./Core/ODrive/ODrive.d 


# Each subdirectory must supply rules for building sources it contributes
Core/ODrive/%.o Core/ODrive/%.su Core/ODrive/%.cyclo: ../Core/ODrive/%.c Core/ODrive/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -DUSE_NUCLEO_64 -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/BSP/STM32H7xx_Nucleo -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I/home/icart/Git_Clone/Bipedal_Robot_Software/NUCLEOH755ZI-Q_Test/test_CAN/CM7/Core/ODrive -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-ODrive

clean-Core-2f-ODrive:
	-$(RM) ./Core/ODrive/ODrive.cyclo ./Core/ODrive/ODrive.d ./Core/ODrive/ODrive.o ./Core/ODrive/ODrive.su

.PHONY: clean-Core-2f-ODrive

