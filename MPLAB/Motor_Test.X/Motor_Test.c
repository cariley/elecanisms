#include <p24FJ128GB206.h>
#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"

// Position tracking variables
long long updatedPos;     // keeps track of the latest updated value of the MR sensor reading
long long rawPos;         // current raw reading from MR sensor
long long lastRawPos;     // last raw reading from MR sensor
long long lastLastRawPos; // last last raw reading from MR sensor
int16_t flipNumber;     // keeps track of the number of flips over the 180deg mark
uint16_t tempOffset;
uint16_t rawDiff;
uint16_t lastRawDiff;
uint16_t rawOffset;
uint16_t lastRawOffset;
uint16_t flipThresh = 700;  // threshold to determine whether or not a flip over the 180 degree mark occurred
bool flipped = false;

int16_t main(void) {
    uint16_t btn2ReadState;
    uint16_t btn2CurrState = 0;
    uint16_t counter2;

    init_clock();
    init_timer();
    init_ui();
    init_pin();
    init_oc();
    init_uart();

    pin_analogIn(&A[0]);

    pin_digitalOut(&D[2]);
    pin_set(&D[2]);

    pin_digitalOut(&D[3]);
    pin_clear(&D[3]);

    pin_digitalOut(&D[4]);
    pin_set(&D[4]);

    pin_digitalOut(&D[5]);
    pin_clear(&D[5]);

    pin_digitalOut(&D[7]);
    pin_set(&D[7]);

    pin_digitalOut(&D[6]);
    oc_pwm(&oc1, &D[6], &timer2, 20000, 30000);

    lastLastRawPos = pin_read(&A[0]);
    lastRawPos = pin_read(&A[0]);

    led_on(&led1);
    led_on(&led2);

    while(1){
        rawPos = pin_read(&A[0]);

        printf("%d   :   %d\n",rawPos,updatedPos);
    }
}