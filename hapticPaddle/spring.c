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
int16_t lastRawDiff;
int16_t lastRawOffset;
int16_t cumulativeVal;      // cumulative sensor value
int16_t current_position; // cumulative sensor value - initial position
int16_t force_desired; // how much force do we want applied to the joystick, given the current control law
// (arbitrary units)
int16_t force_current; // how much force are we currently applying (motor current reading)
int16_t initPos;
uint16_t flipThresh = 600;  // threshold to determine whether or not a flip over the 180 degree mark occurred
bool flipped = false;

void updateMotorPWM(_TIMER *self) {
    rawPos = pin_read(&A[5]) >> 6;
     
    lastRawDiff = rawPos - lastLastRawPos; 
    lastRawOffset = abs(lastRawDiff);
    
    lastLastRawPos = lastRawPos;
    lastRawPos = rawPos;
    
    //check for flip and increment or decrement accordingly
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

    cumulativeVal = rawPos + flipNumber*700;    //each flip changes cumulative value by 700
    current_position = cumulativeVal-initPos;

    force_desired = -(current_position-800) >> 6; // arbitrary units. will fix later

    if (force_desired < 0) {
        oc_pwm(&oc2, &D[5], &timer4, 0, 0);
        oc_pwm(&oc1, &D[6], &timer2, 20000, abs(force_desired) * 5000);
    } else {
        oc_pwm(&oc1, &D[6], &timer2, 0, 0);
        oc_pwm(&oc2, &D[5], &timer4, 20000, abs(force_desired) * 5000);
    }
}

int16_t main(void) {
    init_clock();
    init_timer();
    init_ui();
    init_pin();
    init_oc();
    init_uart();

    Config_Motor_Pins();

    // initialize the location of the joystick- joystick should be manually 
    // positioned in the 'zero' position when reseting (or powering on).
    lastLastRawPos = pin_read(&A[5]) >> 6;
    lastRawPos = pin_read(&A[5]) >> 6;
    initPos = lastLastRawPos;

    timer_every(&timer3,.0005,updateMotorPWM); //keep track of position

    timer_every(&timer1,.5,printData); //report position

    while(1) { 
        // avoid reboot
    }
}



// this function prints all required data. Currently prints through
// serial port, but we want it to eventually print through USB Vendor 
// specific requests.
void printData(_TIMER *self) {
    printf("%d\n",force_desired);
}


// the motorsheild needs a handful of digital pins set to specific values
// to operate in the mode we want it to, plus A5 must be configured as an
// input.
void Config_Motor_Pins() {
    pin_analogIn(&A[5]); // magnetoresistive sensor
    pin_analogIn(&A[0]); // current sensing resistor (amplified)

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
    pin_clear(&D[6]);
}