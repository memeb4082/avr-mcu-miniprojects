#include <avr/io.h>
#include "timer.h"
#include "spi.h"
#include "buzzer.h"

/***
 * Tutorial 12: Introduction
 *
 * In this week's tutorial you will implement a music player.  Pressing
 * each QUTy pushbutton will result in a tone being played.  In addition,
 * the two segments of the 7-segment display above the pushbutton will
 * be lit.
 *
 * The frequencies of the four tones are as follows:
 *
 * TONE1             TONE2             TONE3     TONE4
 * 4XX * 2^(-5/12)   4XX * 2^(-8/12)   4XX       4XX * 2^(-17/12)
 *
 * where XX are the last two digits of your student number.
 *
 * Playing a tone and lighting the segemnts above the pushbutton when the
 * pushbutton is pressed is a requirement for Assessment 2.
 *
 * Code has already been provided in timer.c and spi.c, for dispalying
 * the segment pattern in segs[0] and segs[1] to the two digits of the
 * 7-segment display.  This code uses TCB0 with a periodic interrupt
 * every 10ms to refresh the 7-segment display.  It also uses the SPI
 * peripheral and the SPI0 interrupt to provide a rising edge on
 * DISP_LATCH, so that the bits shifted out through SPI_MOSI appear
 * on bits Q0-7 of the SHIFT register, which drive the inputs of
 * the 7-segment display.
 */

//     ABCDEFG
//    xFABGCDE
#define SEGS_EF 0b00111110
#define SEGS_BC 0b01101011
#define SEGS_OFF 0b01111111

volatile uint8_t segs[] = {SEGS_OFF, SEGS_OFF}; // segs initially off

volatile uint8_t pb_debounced = 0xFF;

void pb_init(void)
{
    // already configured as inputs

    // PBs PA4-7, enable pullup resistors
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
}

void pb_debounce(void)
{
    // Write your code for Ex 12.4 here
    static uint8_t c0 = 0;
    static uint8_t c1 = 0;
    uint8_t pb_sample = PORTA.IN;
    uint8_t pb_changed = pb_sample ^ pb_debounced;
    c1 = (c1 ^ c0) & pb_changed;
    c0 = ~c0 & pb_changed;
    pb_debounced ^= (c0 & c0);
    // Write your code for Ex 12.4 above this line
}

/***
 * Ex 12.0
 *
 * Declare an enumerated type (enum) that can hold the values of each state
 * required to implement the state machine in "state_machine_tut12.png".
 * Use meaningful names for each enum value.
 */
typedef enum
{
    WAIT,
    TONE1,
    TONE2,
    TONE3,
    TONE4
} my_state;
// Write your code for Ex 12.0 above this line

int main(void)
{
    pb_init();
    timer_init();
    spi_init();
    buzzer_init();
    uint8_t pb_sample = 0xFF;
    uint8_t pb_sample_r = 0xFF;
    uint8_t pb_changed, pb_rising, pb_falling;
    /***
     * Ex 12.1
     *
     * Declare a variable below, of the enumerated type you declared above.
     * Initialise this variable with the initial state of the state machine.
     *
     */
    my_state STATE = WAIT;
    // Write your code for Ex 12.1 above this line

    /***
     * Ex 12.2
     *
     * Complete the code in the function buzzer_init(void) (in file buzzer.c)
     * to initialise PORTB PIN0 and TCA0 to drive the buzzer.
     *
     * Determine your frequencies according to your student number, and
     * work out appropriate values for TCA0.SINGLE.PER in each case.
     * Update the #defines in buzzer.h to reflect these values.
     *
     * Do not write your code here, edit function buzzer_init(void) in file
     * buzzer.c and #defines in buzzer.h.
     *
     */

    while (1)
    {
        pb_sample_r = pb_sample;
        pb_sample = pb_debounced;
        pb_changed = pb_sample ^ pb_sample_r;
        pb_rising = pb_changed & pb_sample;
        pb_falling = pb_changed & pb_sample_r;
        /***
         * Ex 12.3
         *
         * We have already written some code in "buttons.c" which uses periodic
         * interrupts to sample the state of the pushbuttons for you; the state
         * of the pushbuttons will be available in variable "pb_state".
         *
         * Write some code below which will allow you to detect button pressed events
         * for S1-S4 (i.e. that can detect the falling edge of the bits in "pb_state").
         * Store the output of this detection in a variable.
         * Detect rising and falling edges of the pushbuttons and store the
         * results in variables pb_falling and pb_rising.
         */
        // Write your code for Ex 12.3 above this line
        /***
         * Ex 12.4
         *
         * Write the switch-case statement below that implements the state machine
         * documented in "state_machine_tut12.png".
         *
         * Your switch statement should test the value of the variable declared
         * in Ex 12.1. A case should be defined for every state in the state machine.
         *
         * Use the output of the code you wrote above in Ex 12.3 to trigger the
         * appropriate transitions between states.
         *
         * For each state:
         *    - write the appropriate value to segs[0] and segs[1] (SEGS_EF,
         *      SEGS_BC, or SEGS_OFF)
         *    - update TCAO PER and BUFn, and enable/disable TCA0 appropriately
         *      so that tones of the four frequencies are sounded.
         */
        switch (STATE)
        {
        case WAIT:
            if (pb_falling & PIN4_bm)
            {
                STATE = TONE1;
                segs[0] = SEGS_OFF;
                segs[1] = SEGS_OFF;
                TCA0.SINGLE.PERBUF = TONE1_PER;
                TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 2;
            }
            else if (pb_falling & PIN5_bm)
            {
                STATE = TONE2;
                segs[0] = SEGS_BC;
                segs[1] = SEGS_OFF;
                TCA0.SINGLE.PERBUF = TONE2_PER;
                TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 2;
            }
            else if (pb_falling & PIN6_bm)
            {
                STATE = TONE3;
                segs[0] = SEGS_OFF;
                segs[1] = SEGS_EF;
                TCA0.SINGLE.PERBUF = TONE3_PER;
                TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 2;
            }
            else if (pb_falling & PIN7_bm)
            {
                STATE = TONE4;
                segs[0] = SEGS_OFF;
                segs[1] = SEGS_BC;
                TCA0.SINGLE.PERBUF = TONE4_PER;
                TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 2;
            }
            break;
        case TONE1:
            if (pb_rising & PIN4_bm)
            {
                STATE = WAIT;
                segs[0] = SEGS_OFF;
                segs[1] = SEGS_OFF;
                TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm; // Turn off buzzer (disable pwm)
            }
            break;
        case TONE2:
            if (pb_rising & PIN5_bm)
            {
                STATE = WAIT;
                segs[0] = SEGS_OFF;
                segs[1] = SEGS_OFF;
                TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm; // Turn off buzzer (disable pwm)
            }
            break;
        case TONE3:
            if (pb_rising & PIN6_bm)
            {
                STATE = WAIT;
                segs[0] = SEGS_OFF;
                segs[1] = SEGS_OFF;
                TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm; // Turn off buzzer (disable pwm)
            }
            break;
        case TONE4:
            if (pb_rising & PIN7_bm)
            {
                STATE = WAIT;
                segs[0] = SEGS_OFF;
                segs[1] = SEGS_OFF;
                TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm; // Turn off buzzer (disable pwm)
            }
            break;
        default:
            STATE = WAIT;
            segs[0] = SEGS_OFF;
            segs[1] = SEGS_OFF;
            break;
        }
        // Write your code for Ex 12.4 above this line

        /* Ex 12.5
         *
         * Complete the code in pb_debounce(void) to provide a debounced
         * pushbutton read.
         *
         * Note tha pb_debounced is already called in the periodic interrupt in
         * timer.c.
         *
         * Do not write your code here, edit function pb_debouce above.
         */
    }
}
