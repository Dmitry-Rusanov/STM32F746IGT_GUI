################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include porting/subdir.mk
-include lvgl/src/widgets/subdir.mk
-include lvgl/src/misc/subdir.mk
-include lvgl/src/hal/subdir.mk
-include lvgl/src/font/subdir.mk
-include lvgl/src/extra/widgets/win/subdir.mk
-include lvgl/src/extra/widgets/tileview/subdir.mk
-include lvgl/src/extra/widgets/tabview/subdir.mk
-include lvgl/src/extra/widgets/spinner/subdir.mk
-include lvgl/src/extra/widgets/spinbox/subdir.mk
-include lvgl/src/extra/widgets/span/subdir.mk
-include lvgl/src/extra/widgets/msgbox/subdir.mk
-include lvgl/src/extra/widgets/meter/subdir.mk
-include lvgl/src/extra/widgets/menu/subdir.mk
-include lvgl/src/extra/widgets/list/subdir.mk
-include lvgl/src/extra/widgets/led/subdir.mk
-include lvgl/src/extra/widgets/keyboard/subdir.mk
-include lvgl/src/extra/widgets/imgbtn/subdir.mk
-include lvgl/src/extra/widgets/colorwheel/subdir.mk
-include lvgl/src/extra/widgets/chart/subdir.mk
-include lvgl/src/extra/widgets/calendar/subdir.mk
-include lvgl/src/extra/widgets/animimg/subdir.mk
-include lvgl/src/extra/themes/mono/subdir.mk
-include lvgl/src/extra/themes/default/subdir.mk
-include lvgl/src/extra/themes/basic/subdir.mk
-include lvgl/src/extra/others/snapshot/subdir.mk
-include lvgl/src/extra/others/msg/subdir.mk
-include lvgl/src/extra/others/monkey/subdir.mk
-include lvgl/src/extra/others/imgfont/subdir.mk
-include lvgl/src/extra/others/ime/subdir.mk
-include lvgl/src/extra/others/gridnav/subdir.mk
-include lvgl/src/extra/others/fragment/subdir.mk
-include lvgl/src/extra/libs/sjpg/subdir.mk
-include lvgl/src/extra/libs/rlottie/subdir.mk
-include lvgl/src/extra/libs/qrcode/subdir.mk
-include lvgl/src/extra/libs/png/subdir.mk
-include lvgl/src/extra/libs/gif/subdir.mk
-include lvgl/src/extra/libs/fsdrv/subdir.mk
-include lvgl/src/extra/libs/freetype/subdir.mk
-include lvgl/src/extra/libs/ffmpeg/subdir.mk
-include lvgl/src/extra/libs/bmp/subdir.mk
-include lvgl/src/extra/layouts/grid/subdir.mk
-include lvgl/src/extra/layouts/flex/subdir.mk
-include lvgl/src/extra/subdir.mk
-include lvgl/src/draw/swm341_dma2d/subdir.mk
-include lvgl/src/draw/sw/subdir.mk
-include lvgl/src/draw/stm32_dma2d/subdir.mk
-include lvgl/src/draw/sdl/subdir.mk
-include lvgl/src/draw/nxp/vglite/subdir.mk
-include lvgl/src/draw/nxp/pxp/subdir.mk
-include lvgl/src/draw/nxp/subdir.mk
-include lvgl/src/draw/arm2d/subdir.mk
-include lvgl/src/draw/subdir.mk
-include lvgl/src/core/subdir.mk
-include lvgl/demos/widgets/assets/subdir.mk
-include lvgl/demos/widgets/subdir.mk
-include lvgl/demos/stress/subdir.mk
-include lvgl/demos/music/assets/subdir.mk
-include lvgl/demos/music/subdir.mk
-include lvgl/demos/keypad_encoder/subdir.mk
-include lvgl/demos/benchmark/assets/subdir.mk
-include lvgl/demos/benchmark/subdir.mk
-include Drivers/STM32F7xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include BSP/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := STM32F746IGT_GUI
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STM32F746IGT_GUI.elf \

MAP_FILES += \
STM32F746IGT_GUI.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STM32F746IGT_GUI.list \


# All Target
all: main-build

# Main-build Target
main-build: STM32F746IGT_GUI.elf secondary-outputs

# Tool invocations
STM32F746IGT_GUI.elf STM32F746IGT_GUI.map: $(OBJS) $(USER_OBJS) C:\Users\user\Documents\GIT\STM32F746IGT_GUI\STM32F746IGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "STM32F746IGT_GUI.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"C:\Users\user\Documents\GIT\STM32F746IGT_GUI\STM32F746IGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="STM32F746IGT_GUI.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STM32F746IGT_GUI.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STM32F746IGT_GUI.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STM32F746IGT_GUI.elf STM32F746IGT_GUI.list STM32F746IGT_GUI.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
