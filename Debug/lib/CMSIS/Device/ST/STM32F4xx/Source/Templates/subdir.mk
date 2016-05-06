################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c 

OBJS += \
./lib/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.o 

C_DEPS += \
./lib/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
lib/CMSIS/Device/ST/STM32F4xx/Source/Templates/%.o: ../lib/CMSIS/Device/ST/STM32F4xx/Source/Templates/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DSTM32F40XX -DUSE_USB_OTG_FS -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -I"D:\WORK\ARM\ARM C\SuperDumperFW\src" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\CMSIS\Include" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32F4xx_StdPeriph_Driver\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_OTG_Driver\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Core\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Class\hid\inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


