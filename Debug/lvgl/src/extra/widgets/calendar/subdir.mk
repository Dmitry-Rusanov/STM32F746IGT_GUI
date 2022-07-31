################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lvgl/src/extra/widgets/calendar/lv_calendar.c \
../lvgl/src/extra/widgets/calendar/lv_calendar_header_arrow.c \
../lvgl/src/extra/widgets/calendar/lv_calendar_header_dropdown.c 

OBJS += \
./lvgl/src/extra/widgets/calendar/lv_calendar.o \
./lvgl/src/extra/widgets/calendar/lv_calendar_header_arrow.o \
./lvgl/src/extra/widgets/calendar/lv_calendar_header_dropdown.o 

C_DEPS += \
./lvgl/src/extra/widgets/calendar/lv_calendar.d \
./lvgl/src/extra/widgets/calendar/lv_calendar_header_arrow.d \
./lvgl/src/extra/widgets/calendar/lv_calendar_header_dropdown.d 


# Each subdirectory must supply rules for building sources it contributes
lvgl/src/extra/widgets/calendar/%.o: ../lvgl/src/extra/widgets/calendar/%.c lvgl/src/extra/widgets/calendar/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I"/home/rus/Yandex.Disk/Projects/stm32_linux/stms2f746/STM32F746IGT_GUI/lvgl" -I"/home/rus/Yandex.Disk/Projects/stm32_linux/stms2f746/STM32F746IGT_GUI/porting" -I"/home/rus/Yandex.Disk/Projects/stm32_linux/stms2f746/STM32F746IGT_GUI/BSP" -I"/home/rus/Yandex.Disk/Projects/stm32_linux/stms2f746/STM32F746IGT_GUI/Fonts" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

