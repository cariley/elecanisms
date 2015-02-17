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
int16_t rawPos;         // current raw reading from MR sensor
int16_t lastRawPos;     // last raw reading from MR sensor
int16_t lastLastRawPos; // last last raw reading from MR sensor
int16_t flipNumber;     // keeps track of the number of flips over the 180deg mark
int16_t tempOffset;
int16_t rawDiff;
int16_t lastRawDiff;
int16_t rawOffset;
int16_t lastRawOffset;
int16_t raw[400];
int16_t flip[400];
uint16_t times[400];
uint16_t flipThresh = 700;  // threshold to determine whether or not a flip over the 180 degree mark occurred
bool flipped = false;

int16_t main(void) {
    uint16_t btn2ReadState;
    uint16_t btn2CurrState = 0;
    uint16_t counter2;
    uint16_t readTime;

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
    oc_pwm(&oc1, &D[6], &timer2, 20000, 60000);

    timer_start(&timer1);

    lastLastRawPos = pin_read(&A[0]) >> 6;
    lastRawPos = pin_read(&A[0]) >> 6;

    led_on(&led2);

    while(1){
        led_off(&led1);
        btn2ReadState = !sw_read(&sw2);
        if (btn2CurrState != btn2ReadState) {
            counter2++;
        }
        else {
            counter2 = 0;
        }

        if (counter2 > 10) {
            counter2 = 0;

            btn2CurrState = btn2ReadState;
            if (btn2CurrState)
            {
                oc_pwm(&oc1, &D[6], &timer2, 0, 0);
            }
        }

        for (int a = 0; a < 400; a++) {
            for (int b = 0; b < 100; b++) {
                rawPos = pin_read(&A[0]) >> 6;
                readTime = timer_read(&timer1);

                rawDiff = rawPos - lastRawPos;          
                lastRawDiff = rawPos - lastLastRawPos; 
                rawOffset = abs(rawDiff);
                lastRawOffset = abs(lastRawDiff);
                
                lastLastRawPos = lastRawPos;
                lastRawPos = rawPos;
                  
                if((lastRawOffset > flipThresh) && (!flipped)) { 
                    if(lastRawDiff > 0) {        
                        flipNumber--;             
                    } else {                     
                        flipNumber++;             
                    }
                    flipped = true;          
                } else {                       
                    flipped = false;
                }
            }
            raw[a] = rawPos;
            flip[a] = flipNumber;
            times[a] = readTime;
        }
        led_on(&led1);
        for (int c = 0; c < 400; c++) {
            printf("%d:%d;%d\n",raw[c],flip[c],times[c]);
        }
    }
}