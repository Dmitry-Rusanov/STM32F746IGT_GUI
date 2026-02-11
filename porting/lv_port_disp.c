/**
 * @file lv_port_disp.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "main.h"
#include "stm32746g_lcd.h"
#include "ltdc.h"
#include "gpio.h"

/*********************
 *      DEFINES
 *********************/

#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    1024
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    600
#endif

// Активный слой LTDC (обычно 0)
#define ACTIVE_LAYER    0

// Адреса фреймбуферов в SDRAM
#define LCD_FB_START_ADDRESS       ((uint32_t)0xD0000000)
#define LCD_FB_SIZE_BYTES          ((uint32_t)(MY_DISP_HOR_RES * MY_DISP_VER_RES * 2))  // RGB565

#define BLOCK_SIZE  60

/**********************
 *      TYPEDEFS
 **********************/
extern DMA2D_HandleTypeDef hdma2d;
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
extern LTDC_HandleTypeDef hltdc;

static lv_disp_drv_t disp_drv;

/**********************
 *  STATIC VARIABLES
 **********************/
static int32_t x1_flush;
static int32_t y1_flush;
static int32_t x2_flush;
static int32_t y2_fill;
static int32_t y_fill_act;
static const lv_color_t *buf_to_flush;
static __IO uint16_t *addr;

static __IO uint16_t *fb_buf1 = (__IO uint16_t *)LCD_FB_START_ADDRESS;
static __IO uint16_t *fb_buf2 = (__IO uint16_t *)(LCD_FB_START_ADDRESS + LCD_FB_SIZE_BYTES);

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void DMA_TransferComplete(DMA_HandleTypeDef *han);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /* Инициализация дисплея */
    disp_init();

    /* Настройка двойной буферизации (рекомендуется для DMA2D) */
#if (LV_BUF_TYPE == 3)
    static lv_disp_draw_buf_t draw_buf_dsc;
    lv_disp_draw_buf_init(&draw_buf_dsc,
                          (void *)fb_buf1,
                          (void *)fb_buf2,
                          MY_DISP_HOR_RES * MY_DISP_VER_RES);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf_dsc;
    disp_drv.full_refresh = 1;          // обязательно для двойной буферизации

#elif (LV_BUF_TYPE == 2)
    // Частичная двойная буферизация (можно реализовать позже)
    // ...
#else
    // Однобуферный режим или частичный буфер
    // ...
#endif

    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void disp_init(void)
{
    HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream0,
                             HAL_DMA_XFER_CPLT_CB_ID,
                             DMA_TransferComplete);
}

volatile bool disp_flush_enabled = true;

void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
#if (LV_BUF_TYPE == 3)
    // Двойная буферизация полного экрана — просто меняем адрес в LTDC
    SCB_CleanDCache_by_Addr((uint32_t *)color_p, LCD_FB_SIZE_BYTES);

    // Меняем адрес активного фреймбуфера
    HAL_LTDC_SetAddress(&hltdc, (uint32_t)color_p, ACTIVE_LAYER);

    lv_disp_flush_ready(disp_drv);

#else
    // Частичная прорисовка — копирование через DMA (старый режим)

    x1_flush = area->x1;
    y1_flush = area->y1;
    x2_flush = area->x2;
    y2_fill  = area->y2;
    y_fill_act = area->y1;
    buf_to_flush = color_p;

    addr = (__IO uint16_t *)(hltdc.LayerCfg[ACTIVE_LAYER].FBStartAdress +
                             (2 * (y_fill_act * MY_DISP_HOR_RES + area->x1)));

    // Очистка кэша только для нужной области
    uint32_t area_size = (x2_flush - x1_flush + 1) * (y2_fill - y1_flush + 1) * sizeof(lv_color_t);
    SCB_CleanDCache_by_Addr((uint32_t *)buf_to_flush, area_size);
    SCB_InvalidateICache();

    if ((area->x1 == 0) && (area->x2 == MY_DISP_HOR_RES - 1) &&
        ((area->y2 - area->y1 + 1) > BLOCK_SIZE))
    {
        // Оптимизированная передача большими блоками
        int32_t blocks = (area->y2 - area->y1 + 1) / BLOCK_SIZE;
        int32_t lines = (area->y2 - area->y1 + 1) % BLOCK_SIZE;
        uint32_t length = (x2_flush - x1_flush + 1) * BLOCK_SIZE;

        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

        while (blocks > 0)
        {
            HAL_DMA_Start(&hdma_memtomem_dma2_stream0,
                          (uint32_t)buf_to_flush,
                          (uint32_t)addr,
                          length);

            HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream0,
                                    HAL_DMA_FULL_TRANSFER,
                                    0xFFFF);

            addr += length;
            buf_to_flush += length;
            y_fill_act += BLOCK_SIZE;
            blocks--;
        }

        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

        if (lines == 0)
        {
            lv_disp_flush_ready(disp_drv);
        }
        else
        {
            uint32_t length_last = (x2_flush - x1_flush + 1) * lines;
            HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0,
                             (uint32_t)buf_to_flush,
                             (uint32_t)addr,
                             length_last);
        }
    }
    else
    {
        // Обычная передача оставшейся области
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
        uint32_t length = (x2_flush - x1_flush + 1) * (y2_fill - y1_flush + 1);

        HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0,
                         (uint32_t)buf_to_flush,
                         (uint32_t)addr,
                         length);
    }
#endif
}

void DMA_TransferComplete(DMA_HandleTypeDef *han)
{
    y_fill_act++;

    if (y_fill_act > y2_fill)
    {
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
        SCB_CleanInvalidateDCache();
        SCB_InvalidateICache();
        lv_disp_flush_ready(&disp_drv);
    }
    else
    {
        uint32_t length = (x2_flush - x1_flush + 1);
        buf_to_flush += (x2_flush - x1_flush + 1);

        addr = (__IO uint16_t *)(hltdc.LayerCfg[ACTIVE_LAYER].FBStartAdress +
                                 (2 * (y_fill_act * MY_DISP_HOR_RES + x1_flush)));

        HAL_DMA_Start_IT(han, (uint32_t)buf_to_flush, (uint32_t)addr, length);
    }
}
