################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.c 

OBJS += \
./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.o 

C_DEPS += \
./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.d 


# Each subdirectory must supply rules for building sources it contributes
Core/.metadata/.plugins/org.eclipse.cdt.make.core/%.o Core/.metadata/.plugins/org.eclipse.cdt.make.core/%.su Core/.metadata/.plugins/org.eclipse.cdt.make.core/%.cyclo: ../Core/.metadata/.plugins/org.eclipse.cdt.make.core/%.c Core/.metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_DIRECT_SMPS_SUPPLY -DUSE_NUCLEO_64 -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/BSP/STM32H7xx_Nucleo -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I/home/icart/Git_Clone/Bipedal_Robot_Software/NUCLEOH755ZI-Q_Test/test_CAN/CM7/Core/ODrive -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f--2e-metadata-2f--2e-plugins-2f-org-2e-eclipse-2e-cdt-2e-make-2e-core

clean-Core-2f--2e-metadata-2f--2e-plugins-2f-org-2e-eclipse-2e-cdt-2e-make-2e-core:
	-$(RM) ./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.cyclo ./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.d ./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.o ./Core/.metadata/.plugins/org.eclipse.cdt.make.core/specs.su

.PHONY: clean-Core-2f--2e-metadata-2f--2e-plugins-2f-org-2e-eclipse-2e-cdt-2e-make-2e-core

