#include "ch.h"
#include "hal.h"
#include "lcd_handler.h"
#include <string.h>
#include <stdio.h>


static Thread *tp;

static void led4off(void *arg) {

  (void)arg;
  palClearPad(GPIOB, GPIOB_LED4);
  chSysLockFromIsr();
  chEvtSignalI(tp, (eventmask_t)1);
  chSysUnlockFromIsr();
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
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    /*
     * setup the main thread
     */
    tp = chThdSelf();
    chRegSetThreadName("leds");
    palSetPadMode(GPIOB, GPIOB_LED3, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOB, GPIOB_LED4, PAL_MODE_OUTPUT_PUSHPULL);


    /*
     * Start the LCD handler
     */
    lcd_handler_init();

    /*
     * Activates the EXT driver 1.
     */
    extStart(&EXTD1, &extcfg);


    /*
     * main thread
     */
    while (TRUE)
    {
        static int tog = 0;
        static int intcount = 0;
        static char message[8];

        if (chEvtWaitOneTimeout((eventmask_t)1, MS2ST(500)))
        {
            ++intcount;
            sprintf(message, "%6d", intcount);
            lcd_display_message(message, strlen(message));
        }

        if (tog)
        {
            palSetPad(GPIOB, GPIOB_LED3);
            //palClearPad(GPIOB, GPIOB_LED4);
        }
        else
        {
            palClearPad(GPIOB, GPIOB_LED3);
            //palSetPad(GPIOB, GPIOB_LED4);
        }
        tog = ~tog;
    }

    return 0;
}
