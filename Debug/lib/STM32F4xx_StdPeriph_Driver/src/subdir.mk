################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/STM32F4xx_StdPeriph_Driver/src/misc.c \
../lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
../lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c 

OBJS += \
./lib/STM32F4xx_StdPeriph_Driver/src/misc.o \
./lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.o \
./lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.o 

C_DEPS += \
./lib/STM32F4xx_StdPeriph_Driver/src/misc.d \
./lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.d \
./lib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.d 


# Each subdirectory must supply rules for building sources it contributes
lib/STM32F4xx_StdPeriph_Driver/src/%.o: ../lib/STM32F4xx_StdPeriph_Driver/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DSTM32F40XX -DARM_MATH_CM4 -DUSE_USB_OTG_FS -DUSE_STDPERIPH_DRIVER -I"D:\WORK\ARM\ARM C\SuperDumperFW\src" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\CMSIS\Include" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32F4xx_StdPeriph_Driver\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_OTG_Driver\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Core\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Class\hid\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Class\cdc\inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


