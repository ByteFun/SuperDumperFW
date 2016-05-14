################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/utils/subdir.mk
-include src/usb-cdc/subdir.mk
-include src/dumper/subdir.mk
-include src/subdir.mk
-include lib/STM32_USB_OTG_Driver/src/subdir.mk
-include lib/STM32_USB_Device_Library/Core/src/subdir.mk
-include lib/STM32_USB_Device_Library/Class/cdc/src/subdir.mk
-include lib/STM32F4xx_StdPeriph_Driver/src/subdir.mk
-include lib/CMSIS/Device/ST/STM32F4xx/Source/Templates/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
SuperDumperFW.bin \

SECONDARY_SIZE += \
SuperDumperFW.siz \


# All Target
all: SuperDumperFW.elf secondary-outputs

# Tool invocations
SuperDumperFW.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "D:\WORK\ARM\ARM C\SuperDumperFW\startup\stm32_flash.ld" -Xlinker --gc-sections -Wl,-Map,"SuperDumperFW.map" -o "SuperDumperFW.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

SuperDumperFW.bin: SuperDumperFW.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary "SuperDumperFW.elf"  "SuperDumperFW.bin"
	@echo 'Finished building: $@'
	@echo ' '

SuperDumperFW.siz: SuperDumperFW.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "SuperDumperFW.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) SuperDumperFW.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
