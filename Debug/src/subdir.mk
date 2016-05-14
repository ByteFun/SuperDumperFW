################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/stm32f4xx_it.c 

S_UPPER_SRCS += \
../src/startup_stm32f4xx.S 

OBJS += \
./src/main.o \
./src/startup_stm32f4xx.o \
./src/stm32f4xx_it.o 

S_UPPER_DEPS += \
./src/startup_stm32f4xx.d 

C_DEPS += \
./src/main.d \
./src/stm32f4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DSTM32F40XX -DUSE_USB_OTG_FS -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -I"D:\WORK\ARM\ARM C\SuperDumperFW\src" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\CMSIS\Include" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32F4xx_StdPeriph_Driver\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_OTG_Driver\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Core\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Class\hid\inc" -I"D:\WORK\ARM\ARM C\SuperDumperFW\lib\STM32_USB_Device_Library\Class\cdc\inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -DUSE_USB_OTG_FS -DARM_MATH_CM4 -DSTM32F40XX -DUSE_STDPERIPH_DRIVER -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


