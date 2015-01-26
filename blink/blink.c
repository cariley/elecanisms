#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();

    led_on(&led1);
    led_on(&led2);
    led_on(&led3);
    timer_setPeriod(&timer2, 0.1);
    timer_start(&timer2);

    int loop_counter = 0;

    while (1) {
        if (timer_flag(&timer2)) {
            timer_lower(&timer2);
            loop_counter ++;
            if (loop_counter%1 == 0) {
                led_toggle(&led1);}
            if (loop_counter%5 == 0) {
                led_toggle(&led2);}
            if (loop_counter%10 == 0) {
                led_toggle(&led3);}
            if (loop_counter == 20) {
                loop_counter = 0;}
        }
    }
}

