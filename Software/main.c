#include <msp430.h>
#include "ws2812.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

void gradualFill(uint32_t n, ColorMap);
unsigned int numInterrupts = 0;
static int randNum;
bool Green = false;
bool Blue = true;


int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
    if (CALBC1_16MHZ==0xFF)    // If calibration constant erased
    {
        while(1);              // do not load, trap CPU!!
    }

    // configure clock to 16 MHz
    BCSCTL1 = CALBC1_16MHZ;    // DCO = 16 MHz
    DCOCTL = CALDCO_16MHZ;
    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    CCR0 = 62500;
    TACTL = TASSEL_2 + MC_1;  // SMCLK, upmode

    P1DIR &= 0x00;

                            // P1.5, 1.6 -> capsense input

    // initialize LED strip
    initStrip();  // ***** HAVE YOU SET YOUR NUM_LEDS DEFINE IN WS2812.H? ******

    // set strip color red
    fillStrip(COLOR_OFF);

    int correctPress = 0;
    int button1Previous = 32;
    int button2Previous = 64;
    bool firstDone = false;


    setLEDColor(8, COLOR_BLUE);
    setLEDColor(9, COLOR_BLUE);
    showStrip();

    __bis_SR_register(GIE);     // allow interrupts

    while (1)
    {
        int button1 = P1IN & 0x20;      //PIN1.5 new input read
        int button2 = P1IN & 0x40;      //PIN1.6 input read
        randNum = rand() % 2;

        if (Blue == true & (button1 == 0) & button1Previous == 32) {     //blue press
                  ++correctPress;
                  if ((correctPress < 10) ){
                      setLEDColor(0, COLOR_GREEN);
                      showStrip();
                      __delay_cycles(800000);
                      setLEDColor(0, COLOR_OFF);
                      showStrip();

                  } else if ((correctPress >= 10) & (correctPress < 30)) {
                      setLEDColor(1, COLOR_GREEN);
                      showStrip();
                      __delay_cycles(800000);
                      setLEDColor(1, COLOR_OFF);
                      showStrip();
                  }
        } else if (( Green == true) & (button2 == 0) & button2Previous ==64){
            ++correctPress;
            if ((correctPress < 10) ){
                setLEDColor(0, COLOR_GREEN);
                showStrip();
                __delay_cycles(800000);
                setLEDColor(0, COLOR_OFF);
                showStrip();

            } else if ((correctPress >= 10) & (correctPress < 30)) {
                setLEDColor(1, COLOR_GREEN);
                showStrip();
                __delay_cycles(800000);
                setLEDColor(1, COLOR_OFF);
                showStrip();
            }
        }


        if ((correctPress >= 10) & (correctPress < 20) ){
            setLEDColor(0, COLOR_GREEN);
            showStrip();
            firstDone = true;

        } else if (correctPress >= 20 & correctPress < 30) {
           // P1OUT |= (0x03);
        } else if (correctPress >= 30) {
            setLEDColor(1, COLOR_GREEN);
            showStrip();
           exit(0);
        } else if (correctPress < 10 & firstDone) {
            correctPress = 10;
        }
        button1Previous = button1;
        button2Previous = button2;
    }
}


// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    if (numInterrupts == 500
            ) {
        if (randNum == 0) {
            Blue = false;
            Green = true;
            setLEDColor(8, COLOR_RED);
            setLEDColor(9, COLOR_RED);
            showStrip();


        } else {
            Blue = true;
            Green = false;
            setLEDColor(8, COLOR_BLUE);
            setLEDColor(9, COLOR_BLUE);
            showStrip();



        }

        showStrip();
  numInterrupts = 0;
    } else {
        numInterrupts +=1;
    }
}











void gradualFill(uint32_t n, ColorMap color)
{
uint32_t i;
    for ( i = 0; i < n; i++)
    {        // n is number of LEDs
        setLEDColor(i, color);
        showStrip();
    }
}
