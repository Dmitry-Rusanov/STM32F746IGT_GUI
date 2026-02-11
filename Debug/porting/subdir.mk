################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../porting/lv_port_disp.c \
../porting/lv_port_fs_template.c \
../porting/lv_port_indev.c 

OBJS += \
./porting/lv_port_disp.o \
./porting/lv_port_fs_template.o \
./porting/lv_port_indev.o 

C_DEPS += \
./porting/lv_port_disp.d \
./porting/lv_port_fs_template.d \
./porting/lv_port_indev.d 


# Each subdirectory must supply rules for building sources it contributes
porting/%.o porting/%.su porting/%.cyclo: ../porting/%.c porting/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/user/Documents/GIT/STM32F746IGT_GUI/lvgl" -I"C:/Users/user/Documents/GIT/STM32F746IGT_GUI/porting" -I"C:/Users/user/Documents/GIT/STM32F746IGT_GUI/BSP" -I"C:/Users/user/Documents/GIT/STM32F746IGT_GUI/Fonts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-porting

clean-porting:
	-$(RM) ./porting/lv_port_disp.cyclo ./porting/lv_port_disp.d ./porting/lv_port_disp.o ./porting/lv_port_disp.su ./porting/lv_port_fs_template.cyclo ./porting/lv_port_fs_template.d ./porting/lv_port_fs_template.o ./porting/lv_port_fs_template.su ./porting/lv_port_indev.cyclo ./porting/lv_port_indev.d ./porting/lv_port_indev.o ./porting/lv_port_indev.su

.PHONY: clean-porting

