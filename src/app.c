/*
 * @Author AbdouSam
 * @brief, simplest code to test-run a PIC32MZ without any use of libraries
 *
 */
#include <stdbool.h>

#include <xc.h>
#include <string.h>

#include "pic32_config.h"
#include "debug.h"

#include "helpers.h"
#include "sysclk.h"
#include "gpio.h"
#include "uart.h"
#include "delay.h"
#include "timer.h"
#include "interrupt.h"
#include "i2c.h"
#include "rtc.h"
#include "app.h"
#include "uart.h"

/* This define is to set the  µC to run on internal clock
 * config is set to run CPU at 200 Mhz,
 * with internal or 24Mhz external clock
 */

/*#define CONFIG_CPU_USE_FRC */

/*** DEVCFG0 ***/
#pragma config DEBUG =      OFF
#pragma config JTAGEN =     OFF
#pragma config ICESEL =     ICS_PGx1
#pragma config TRCEN =      OFF
#pragma config BOOTISA =    MIPS32
#pragma config FECCCON =    OFF_UNLOCKED
#pragma config FSLEEP =     OFF
#pragma config DBGPER =     PG_ALL
#pragma config SMCLR =      MCLR_NORM
#pragma config SOSCGAIN =   GAIN_2X
#pragma config SOSCBOOST =  ON
#pragma config POSCGAIN =   GAIN_0_5X
#pragma config POSCBOOST =  ON
#pragma config EJTAGBEN =   NORMAL
#pragma config CP =         OFF /* code protect */

/*** DEVCFG1 ***/

#pragma config FNOSC =      SPLL
#pragma config DMTINTV =    WIN_127_128
#pragma config FSOSCEN =    OFF
#pragma config IESO =       OFF

#ifdef CONFIG_CPU_USE_FRC
#pragma config POSCMOD =    OFF
#else
#pragma config POSCMOD =    HS
#endif

#pragma config OSCIOFNC =   OFF
#pragma config FCKSM =      CSECME
#pragma config WDTPS =      PS1048576
#pragma config WDTSPGM =    STOP
#pragma config FWDTEN =     OFF /* WatchDog timer */
#pragma config WINDIS =     NORMAL
#pragma config FWDTWINSZ =  WINSZ_25
#pragma config DMTCNT =     DMT31
#pragma config FDMTEN =     OFF /* Deadman timer */

/*** DEVCFG2 ***/

#ifdef CONFIG_CPU_USE_FRC
#pragma config FPLLIDIV =   DIV_1
#else
#pragma config FPLLIDIV =   DIV_3
#endif

#pragma config FPLLRNG =    RANGE_5_10_MHZ

#ifdef CONFIG_CPU_USE_FRC
#pragma config FPLLICLK =   PLL_FRC
#else
#pragma config FPLLICLK =   PLL_POSC
#endif

#pragma config FPLLMULT =   MUL_50
#pragma config FPLLODIV =   DIV_2
#pragma config UPLLFSEL =   FREQ_24MHZ

/*** DEVCFG3 ***/

#pragma config USERID =     0xffff
#pragma config FMIIEN =     OFF
#pragma config FETHIO =     ON
#pragma config PGL1WAY =    ON
#pragma config PMDL1WAY =   ON
#pragma config IOL1WAY =    ON
#pragma config FUSBIDIO =   OFF

/*** BF1SEQ0 ***/

#pragma config TSEQ =       0x0000
#pragma config CSEQ =       0xffff


static unsigned int  current_time = 0;

static void wdt_clear(void)
{
  volatile uint16_t * wdtclrkey;
  asm volatile("di");
  /* get address of upper 16-bit word of the WDTCON SFR */
  wdtclrkey     = ( (volatile uint16_t *)&WDTCON ) + 1;
  *wdtclrkey    = 0x5743;
  asm volatile("ei");
}

int app_init(void)
{

  sysclk_init();

  /* Initial IO as it is set in pic32_config.h */
  gpio_output_set(LED_ORANGE);
  gpio_output_set(LED_RED);
  gpio_output_set(LED_GREEN);

  gpio_state_clear(LED_ORANGE);
  gpio_state_clear(LED_RED);
  gpio_state_clear(LED_GREEN);


  timer_1_init();

  interrupt_init();
  gpio_state_set(RELAY_OUT1);
  return 0;
}

void app_task(void)
{
  bool detecteur_de_mvt=gpio_state_get(DIG_IN1);
  static unsigned int m;
  static bool old_state;
  if (detecteur_de_mvt==true){
    gpio_state_set(LED_GREEN );
    old_state=1;
  }
  else if (detecteur_de_mvt==false && old_state==1)
  {
    m=interrupt_tick_get();
    old_state=false;
  }
  else{
    unsigned int r=interrupt_tick_get();
    if (r-m>=3000){
      gpio_state_clear(LED_GREEN );
    }
  }
  

  if (interrupt_tick_get() - current_time >= 1000)
    {
      /* test timer/interrupt/gpio */
      gpio_state_toggle(LED_ORANGE);

      current_time = interrupt_tick_get();
    }


  wdt_clear();
  

}