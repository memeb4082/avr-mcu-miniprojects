#include <avr/io.h>
#include <avr/interrupt.h>
#include <spi.h>
void timer_init() {
    cli();
    TCB0.CTRLB = TCB_CNTMODE_INT_gc;    // Configure TCB0 in periodic interrupt mode
    TCB0.CCMP = 3333;                   // Set interval for 1ms (3333 clocks @ 3.3 MHz)
    TCB0.INTCTRL = TCB_CAPT_bm;         // CAPT interrupt enable
    TCB0.CTRLA = TCB_ENABLE_bm;         // Enable
    sei(); 
}

volatile int aDisplayIndex = 0;

ISR(TCB0_INT_vect) {

    if (aDisplayIndex) {
        spi_write(0b1101011 | (0x01 << 7));
        aDisplayIndex = 0;
    } else {
        spi_write(0b1101011);
        aDisplayIndex = 1;
    }

    TCB0.INTFLAGS = TCB_CAPT_bm;
}