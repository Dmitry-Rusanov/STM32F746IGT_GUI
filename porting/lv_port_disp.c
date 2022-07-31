/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "main.h"
#include "stm32746g_lcd.h"
#include "ltdc.h"
/*********************
 *      DEFINES
 *********************/

#define CPY_BUF_DMA_STREAM               DMA2_Stream0
#define CPY_BUF_DMA_CHANNEL              DMA_CHANNEL_0
#define CPY_BUF_DMA_STREAM_IRQ           DMA2_Stream0_IRQn
#define CPY_BUF_DMA_STREAM_IRQHANDLER    DMA2_Stream0_IRQHandler

#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    320
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    240
#endif

/**********************
 *      TYPEDEFS
 **********************/
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
extern LTDC_HandleTypeDef hltdc;
static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/

static int32_t x1_flush;
static int32_t y1_flush;
static int32_t x2_flush;
static int32_t y2_fill;
static int32_t y_fill_act;
static const lv_color_t *buf_to_flush;
static __IO uint16_t *my_fb = (__IO uint16_t*) (0xD0000000);
static __IO uint16_t *buf1 = (__IO uint16_t*) (0xD0400000);
static __IO uint16_t *buf2 = (__IO uint16_t*) (0xD0600000);
static __IO uint16_t *addr;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
		lv_color_t *color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

void DMA_TransferComplete(DMA_HandleTypeDef *han);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
	/*-------------------------
	 * Initialize your display
	 * -----------------------*/
	disp_init();

	/*-----------------------------
	 * Create a buffer for drawing
	 *----------------------------*/

	/**
	 * LVGL requires a buffer where it internally draws the widgets.
	 * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
	 * The buffer has to be greater than 1 display row
	 *
	 * There are 3 buffering configurations:
	 * 1. Create ONE buffer:
	 *      LVGL will draw the display's content here and writes it to your display
	 *
	 * 2. Create TWO buffer:
	 *      LVGL will draw the display's content to a buffer and writes it your display.
	 *      You should use DMA to write the buffer's content to the display.
	 *      It will enable LVGL to draw the next part of the screen to the other buffer while
	 *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
	 *
	 * 3. Double buffering
	 *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
	 *      This way LVGL will always provide the whole rendered screen in `flush_cb`
	 *      and you only need to change the frame buffer's address.
	 */
#if(LV_BUF_TYPE == 1)
    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/
#endif
#if(LV_BUF_TYPE == 2)
	/* Example for 2) */
	static lv_disp_draw_buf_t draw_buf_dsc_2;
	static lv_color_t buf_2_1[MY_DISP_HOR_RES * LV_VER_RES_MAX]; /*A buffer for 10 rows*/
	static lv_color_t buf_2_2[MY_DISP_HOR_RES * LV_VER_RES_MAX]; /*An other buffer for 10 rows*/
	lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2,
	MY_DISP_HOR_RES * LV_VER_RES_MAX); /*Initialize the display buffer*/
#endif
#if(LV_BUF_TYPE == 3)
	/* Example for 3) also set disp_drv.full_refresh = 1 below*/
	static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
//    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
	lv_disp_draw_buf_init(&draw_buf_dsc_3, buf1, buf2,
	MY_DISP_VER_RES * LV_VER_RES_MAX); /*Initialize the display buffer*/
#endif
	/*-----------------------------------
	 * Register the display in LVGL
	 *----------------------------------*/

	lv_disp_drv_init(&disp_drv); /*Basic initialization*/

	/*Set up the functions to access to your display*/

	/*Set the resolution of the display*/
	disp_drv.hor_res = MY_DISP_HOR_RES;
	disp_drv.ver_res = MY_DISP_VER_RES;

	/*Used to copy the buffer's content to the display*/
	disp_drv.flush_cb = disp_flush;

	/*Set a display buffer*/
#if(LV_BUF_TYPE == 1)
    disp_drv.draw_buf = &draw_buf_dsc_1;
#endif
#if(LV_BUF_TYPE == 2)
	disp_drv.draw_buf = &draw_buf_dsc_2;
#endif
#if(LV_BUF_TYPE == 3)
	disp_drv.draw_buf = &draw_buf_dsc_3;
#endif
	/*Required for Example 3)*/
	//disp_drv.full_refresh = 1;
	/* Fill a memory array with a color if you have GPU.
	 * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
	 * But if you have a different GPU you can use with this callback.*/
	//disp_drv.gpu_fill_cb = gpu_fill;
	/*Finally register the driver*/
	lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
	/*You code here*/
	HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream0,
			HAL_DMA_XFER_CPLT_CB_ID, DMA_TransferComplete);

}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
	disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
	disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
		lv_color_t *color_p)
{

	x1_flush = area->x1;
	y1_flush = area->y1;
	x2_flush = area->x2;
	y2_fill = area->y2;
	y_fill_act = area->y1;
	buf_to_flush = color_p;
	addr = (hltdc.LayerCfg[0].FBStartAdress + (2 * (y_fill_act * MY_DISP_HOR_RES + area->x1)));
	SCB_CleanInvalidateDCache();
	SCB_InvalidateICache();
	HAL_StatusTypeDef err;



		uint32_t length = (x2_flush - x1_flush + 1);
		err = HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0,	(uint32_t) buf_to_flush, (uint32_t) addr, length);
		if (err != HAL_OK)
		{
			while (1)
				; /*Halt on error*/
		}

//	if (disp_flush_enabled)
//	{
//		/*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
//
//        int32_t x;
//        int32_t y;
//        for(y = area->y1; y <= area->y2; y++) {
//            for(x = area->x1; x <= area->x2; x++) {
//                /*Put a pixel to the display. For example:*/
//                /*put_px(x, y, *color_p)*/
//            	BSP_LCD_DrawPixel(x, y, color_p->full);
//                color_p++;
//            }
//        }
//	}

	/*IMPORTANT!!!
	 *Inform the graphics library that you are ready with the flushing*/
	//lv_disp_flush_ready(disp_drv);
}
void DMA_TransferComplete(DMA_HandleTypeDef *han)
{

	y_fill_act++;

	if (y_fill_act > y2_fill)
	{
		SCB_CleanInvalidateDCache();
		SCB_InvalidateICache();
		lv_disp_flush_ready(&disp_drv);
	}
	else
	{
		uint32_t length = (x2_flush - x1_flush + 1);
		buf_to_flush += x2_flush - x1_flush + 1;
		addr = (hltdc.LayerCfg[0].FBStartAdress
				+ (2 * (y_fill_act * MY_DISP_HOR_RES + x1_flush)));
		if (HAL_DMA_Start_IT(han, (uint32_t) buf_to_flush, (uint32_t) addr,
				length) != HAL_OK)
		{
			while (1)
				; /*Halt on error*/
		}
	}

	//lv_disp_flush_ready(&disp_drv);

}
/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
static void gpu_fill(lv_disp_drv_t *disp_drv, lv_color_t *dest_buf,
		lv_coord_t dest_width, const lv_area_t *fill_area, lv_color_t color)
{
	/*It's an example code which should be done by your GPU*/
	int32_t x, y;
	dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

	for (y = fill_area->y1; y <= fill_area->y2; y++)
	{
		for (x = fill_area->x1; x <= fill_area->x2; x++)
		{
			dest_buf[x] = color;
		}
		dest_buf += dest_width; /*Go to the next line*/
	}
}
#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
