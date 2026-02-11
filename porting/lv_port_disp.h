/**
 * @file lv_port_disp.h
 * Порт дисплея для LVGL на STM32F746 + LTDC + SDRAM
 */

#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES     1024
#define MY_DISP_VER_RES     600

// Тип буферизации: 3 = двойная буферизация полного экрана (рекомендуется)
#define LV_BUF_TYPE         3

// Размер одного кадра в байтах (RGB565 = 2 байта на пиксель)
#define LCD_FB_SIZE_BYTES   ((uint32_t)(MY_DISP_HOR_RES * MY_DISP_VER_RES * 2))

// Начальный адрес фреймбуфера в SDRAM
#define LCD_FB_START_ADDRESS  ((uint32_t)0xD0000000)

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_port_disp_init(void);

/**
 * Включает обновление экрана (вызов disp_flush)
 */
void disp_enable_update(void);

/**
 * Отключает обновление экрана
 */
void disp_disable_update(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_PORT_DISP_H */
