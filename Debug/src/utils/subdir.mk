################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/utils/utils.c 

OBJS += \
./src/utils/utils.o 

C_DEPS += \
./src/utils/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/utils/%.o: ../src/utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DSTM32F40XX -DARM_MATH_CM4 -DUSE_USB_OTG_FS -DUSE_STDPERIPH_DRIVER -I"D:\WORK\ARM\ARM C\SuperDumperFW\src" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\CMSIS\Include" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32F4xx_StdPeriph_Driver\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_OTG_Driver\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Core\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Class\hid\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Class\cdc\inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


