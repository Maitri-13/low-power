################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include Graphics/subdir.mk
-include Drivers/subdir.mk
-include CMSIS/EFM32PG12B/subdir.mk
-include BSP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Panoptinode.axf

# Tool invocations
Panoptinode.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "Panoptinode.ld" -u _printf_float -Xlinker --gc-sections -Xlinker -Map="Panoptinode.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o Panoptinode.axf "./BSP/bsp_bcc.o" "./BSP/bsp_stk.o" "./CMSIS/EFM32PG12B/startup_efm32pg12b.o" "./CMSIS/EFM32PG12B/system_efm32pg12b.o" "./Drivers/display.o" "./Drivers/displayls013b7dh03.o" "./Drivers/displaypalemlib.o" "./Drivers/rtcdriver.o" "./Drivers/udelay.o" "./Graphics/dmd_display.o" "./Graphics/glib.o" "./Graphics/glib_bitmap.o" "./Graphics/glib_circle.o" "./Graphics/glib_font_narrow_6x8.o" "./Graphics/glib_font_normal_8x8.o" "./Graphics/glib_line.o" "./Graphics/glib_rectangle.o" "./Graphics/glib_string.o" "./emlib/em_adc.o" "./emlib/em_assert.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_rtcc.o" "./emlib/em_system.o" "./emlib/em_usart.o" "./src/graphics.o" "./src/mic.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: Panoptinode.hex'
	arm-none-eabi-objcopy -O ihex "Panoptinode.axf" "Panoptinode.hex"
	@echo ' '

	@echo 'Building bin file: Panoptinode.bin'
	arm-none-eabi-objcopy -O binary "Panoptinode.axf" "Panoptinode.bin"
	@echo ' '

	@echo 'Building s37 file: Panoptinode.s37'
	arm-none-eabi-objcopy -O srec "Panoptinode.axf" "Panoptinode.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Panoptinode.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Panoptinode.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
