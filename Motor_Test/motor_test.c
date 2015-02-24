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


int16_t main(void) {
    uint16_t btn2ReadState;

    init_clock();
    init_timer();
    init_ui();
    init_pin();
    init_oc();
    init_uart();

    pin_analogIn(&A[5]);

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

    // simple delay
    for (uint16_t counter = 0; counter < 60000; counter ++) {

    }
    led_on(&led1);
    for (uint16_t counter = 0; counter < 60000; counter ++) {

    }
    led_on(&led2);
    for (uint16_t counter = 0; counter < 60000; counter ++) {

    }



    pin_digitalOut(&D[6]);
    pin_set(&D[6]);
    //oc_pwm(&oc1, &D[6], &timer2, 20000, 60000);

    led_on(&led3);

    while(1) {
    }
}