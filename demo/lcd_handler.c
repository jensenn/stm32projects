/*
 * lcd_handler.c
 *
 * Handles all LCD functions.
 *
 *  Created on: Feb 9, 2013
 */
#include "ch.h"
#include "hal.h"
#include "stm32l_discovery_lcd.h"
#include <string.h>
#include <stdio.h>

static Thread *lcd_tp;
static MUTEX_DECL(lcdmtx);

static uint16_t lcdstr[8] = {'D', 'E', 'M', 'O', 0, 0, 0, 0};


static WORKING_AREA(waLCDThread, 1024);
static msg_t LCDThread(void *arg)
{
    (void) arg;

    chRegSetThreadName("lcd");

    while (TRUE)
    {
        chEvtWaitAny((eventmask_t)1);

        chMtxLock(&lcdmtx);

        LCD_GLASS_DisplayStrDeci(lcdstr);

        chMtxUnlock();
    }
    return 0;
}


int lcd_handler_init(void)
{

    chMtxLock(&lcdmtx);

    LCD_GLASS_Configure_GPIO();
    LCD_GLASS_Init();
    {
        LCD_GLASS_DisplayStrDeci(lcdstr);
    }

    /*
     * Creates the example thread.
     */
    lcd_tp = chThdCreateStatic(waLCDThread, sizeof(waLCDThread), LOWPRIO, LCDThread, NULL );

    chMtxUnlock();

    return 0;
}


int lcd_display_message(char *message, int length)
{
    int i;

    chMtxLock(&lcdmtx);

    for (i=0; i<8; ++i)
    {
        if (length > i)
        {
            lcdstr[i] = message[i];
        }
        else
        {
            lcdstr[i] = 0;
        }
    }

    chMtxUnlock();

    /* tell the lcd thread to run */
    chEvtSignal(lcd_tp, (eventmask_t)1);

    return 0;
}


