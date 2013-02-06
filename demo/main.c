#include "ch.h"
#include "hal.h"
#include "stm32l_discovery_lcd.h"
#include <string.h>
#include <stdio.h>

static int intcount = 0;
static Semaphore sp;

/*
 * This is a periodic thread that does absolutely nothing except increasing
 * a seconds counter.
 */
static WORKING_AREA(waThread1, 1024);
static msg_t Thread1(void *arg)
{
    (void) arg;
    char str[10];

    chRegSetThreadName("panel");

    while (TRUE)
    {
        chSemWait(&sp);

        sprintf(str, "%6d\0", intcount);
        LCD_GLASS_DisplayString(str);
    }
    return 0;
}

static void led4off(void *arg) {

  (void)arg;
  palClearPad(GPIOB, GPIOB_LED4);
  ++intcount;
  chSemSignal(&sp);
}

/* Triggered when the button is pressed or released. The LED4 is set to ON.*/
static void extcb1(EXTDriver *extp, expchannel_t channel) {
  static VirtualTimer vt4;

  (void)extp;
  (void)channel;
  palSetPad(GPIOB, GPIOB_LED4);
  chSysLockFromIsr();
  if (chVTIsArmedI(&vt4))
    chVTResetI(&vt4);
  /* LED4 set to OFF after 200mS.*/
  chVTSetI(&vt4, MS2ST(100), led4off, NULL);
  chSysUnlockFromIsr();
}

static const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, extcb1},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};


/*
 * Application entry point.
 */
int main(void)
{
    Thread *tp;

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    LCD_GLASS_Configure_GPIO();
    LCD_GLASS_Init();
    {
        //uint16_t somestring[] = {'1', '2'|DOT, '3', '4'|DOUBLE_DOT, '5', '6'};
        //LCD_GLASS_DisplayStrDeci(somestring);
        LCD_GLASS_DisplayString("TEST");
    }


    /*
     * Creates the example thread.
     */
    chSemInit(&sp, 0);
    tp = chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL );


    /*
     * Activates the EXT driver 1.
     */
    extStart(&EXTD1, &extcfg);


    //chThdSleepMilliseconds(500);
    //chThdWait(tp);


    chRegSetThreadName("leds");
    palSetPadMode(GPIOB, GPIOB_LED3, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOB, GPIOB_LED4, PAL_MODE_OUTPUT_PUSHPULL);
    while (TRUE)
    {
        palSetPad(GPIOB, GPIOB_LED3);
        //palClearPad(GPIOB, GPIOB_LED4);
        chThdSleepMilliseconds(500);
        palClearPad(GPIOB, GPIOB_LED3);
        //palSetPad(GPIOB, GPIOB_LED4);
        chThdSleepMilliseconds(500);
    }


    return 0;
}
