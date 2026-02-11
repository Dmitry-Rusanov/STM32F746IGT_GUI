################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/GT811.c \
../BSP/GT911_user.c \
../BSP/TS_I2C.c \
../BSP/gt911.c \
../BSP/stm32746g_lcd.c \
../BSP/stm32746g_sdram.c 

OBJS += \
./BSP/GT811.o \
./BSP/GT911_user.o \
./BSP/TS_I2C.o \
./BSP/gt911.o \
./BSP/stm32746g_lcd.o \
./BSP/stm32746g_sdram.o 

C_DEPS += \
./BSP/GT811.d \
./BSP/GT911_user.d \
./BSP/TS_I2C.d \
./BSP/gt911.d \
./BSP/stm32746g_lcd.d \
./BSP/stm32746g_sdram.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/%.o BSP/%.su BSP/%.cyclo: ../BSP/%.c BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/user/Documents/GIT/STM32F746IGT_GUI/lvgl" -I"C:/Users/user/Documents/GIT/STM32F746IGT_GUI/porting" -I"C:/Users/user/Documents/GIT/STM32F746IGT_GUI/BSP" -I"C:/Users/user/Documents/GIT/STM32F746IGT_GUI/Fonts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP

clean-BSP:
	-$(RM) ./BSP/GT811.cyclo ./BSP/GT811.d ./BSP/GT811.o ./BSP/GT811.su ./BSP/GT911_user.cyclo ./BSP/GT911_user.d ./BSP/GT911_user.o ./BSP/GT911_user.su ./BSP/TS_I2C.cyclo ./BSP/TS_I2C.d ./BSP/TS_I2C.o ./BSP/TS_I2C.su ./BSP/gt911.cyclo ./BSP/gt911.d ./BSP/gt911.o ./BSP/gt911.su ./BSP/stm32746g_lcd.cyclo ./BSP/stm32746g_lcd.d ./BSP/stm32746g_lcd.o ./BSP/stm32746g_lcd.su ./BSP/stm32746g_sdram.cyclo ./BSP/stm32746g_sdram.d ./BSP/stm32746g_sdram.o ./BSP/stm32746g_sdram.su

.PHONY: clean-BSP

