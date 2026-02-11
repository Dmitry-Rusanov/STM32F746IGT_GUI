/**
 * @file lv_port_disp.c
 * Порт дисплея LVGL для STM32F746IGT + LTDC + SDRAM (двойная буферизация)
 */

#include "lv_port_disp.h"
#include "main.h"
#include "stm32746g_lcd.h"
#include "ltdc.h"
#include "gpio.h"
#include "stm32f7xx_hal.h"   // для SCB_...

/*-----------------------------------------------------------------
 * Внешние переменные из CubeMX / HAL
 *----------------------------------------------------------------*/
extern LTDC_HandleTypeDef hltdc;
extern DMA2D_HandleTypeDef hdma2d;  // используется LVGL при LV_USE_GPU_STM32_DMA2D

/*-----------------------------------------------------------------
 * Константы
 *----------------------------------------------------------------*/
#define ACTIVE_LAYER        0

/* Два фреймбуфера в SDRAM */
static __IO uint16_t *framebuffer_1 = (__IO uint16_t *)LCD_FB_START_ADDRESS;
static __IO uint16_t *framebuffer_2 = (__IO uint16_t *)(LCD_FB_START_ADDRESS + LCD_FB_SIZE_BYTES);

/*-----------------------------------------------------------------
 * Глобальные переменные
 *----------------------------------------------------------------*/
static lv_disp_drv_t disp_drv;

static volatile bool disp_update_enabled = true;

/*-----------------------------------------------------------------
 * Прототипы
 *----------------------------------------------------------------*/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

/*-----------------------------------------------------------------
 * Публичные функции
 *----------------------------------------------------------------*/

/**
 * Инициализация порта дисплея LVGL
 */
void lv_port_disp_init(void)
{
    disp_init();

#if LV_BUF_TYPE == 3
    static lv_disp_draw_buf_t draw_buf;

    lv_disp_draw_buf_init(&draw_buf,
                          (void *)framebuffer_1,
                          (void *)framebuffer_2,
                          MY_DISP_HOR_RES * MY_DISP_VER_RES);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res      = MY_DISP_HOR_RES;
    disp_drv.ver_res      = MY_DISP_VER_RES;
    disp_drv.flush_cb     = disp_flush;
    disp_drv.draw_buf     = &draw_buf;
    disp_drv.full_refresh = 1;          // обязательно для двойной буферизации

    lv_disp_drv_register(&disp_drv);
#else
    #error "Поддерживается только LV_BUF_TYPE == 3 (двойная буферизация полного экрана)"
#endif
}

/**
 * Включить обновление экрана
 */
void disp_enable_update(void)
{
    disp_update_enabled = true;
}

/**
 * Отключить обновление экрана
 */
void disp_disable_update(void)
{
    disp_update_enabled = false;
}

/*-----------------------------------------------------------------
 * Статические функции
 *----------------------------------------------------------------*/

/**
 * Инициализация низкоуровневых ресурсов дисплея
 */
static void disp_init(void)
{
    // Здесь можно добавить дополнительную инициализацию, если требуется
    // (например, проверка hdma2d или другие настройки)
}

/**
 * Функция передачи данных на дисплей (flush callback)
 * При использовании двойной буферизации + full_refresh:
 *   - копирование не требуется
 *   - просто меняем адрес фреймбуфера в LTDC
 */
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    if (!disp_update_enabled)
    {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    // Очистка кэша данных (обязательно!)
    SCB_CleanDCache_by_Addr((uint32_t *)color_p, LCD_FB_SIZE_BYTES);

    // Переключаем активный фреймбуфер в LTDC
    HAL_LTDC_SetAddress(&hltdc, (uint32_t)color_p, ACTIVE_LAYER);

    // Сообщаем LVGL, что буфер готов
    lv_disp_flush_ready(disp_drv);
}
