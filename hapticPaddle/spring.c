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
int16_t last_duty; // keeps track of the last duty cycle used for the simple integrator control loop for
// controlling force.
// *********************************************** IMPORTANT!! BOARD REDESIGN REQUIRED *******************
int16_t duty_temp; // so it turns out there's a noisy section when trying to measure current at the wrong
// drive frequency. later it would be nice to implement a hardware filter. for now, since this region is 
// unusable (and the PID loop actually gets stuck here), temporarily just avoid this region if you're 
// control loop wants to enter it.
uint16_t kI = 100;

// because apparently PIC C doesn't have abs already built into its libraries. Or if id does,
// I don't know where to look. #include "math.h" didn't do it.
// note: really cool way to do this with terinery operator: return (x) > 0 ? (x) : -(x)
int myAbs(int x) {
    if (x > 0) {
        return x;
    }
    else {
        return -(x);
    }
}

void updateMotorPWM(_TIMER *self) {
    rawPos = pin_read(&A[5]) >> 6;
     
    lastRawDiff = rawPos - lastLastRawPos; 
    lastRawOffset = myAbs(lastRawDiff);
    
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
    force_desired = force_desired * 5;

    force_current = pin_read(&A[0]) >> 6; // read the current sense resistor

    if (myAbs(myAbs(force_desired)-force_current)>2) { // significant enough to change the motor speed
        if (myAbs(force_desired)-force_current < 0) { // want less!!
            last_duty -= kI;
        }
        else {
            last_duty += kI; // want MORE!!
        }
        // deal with extreme cases
        if (last_duty < 0) {
            last_duty = 0;
        }
        if (last_duty > 65000) {
            last_duty = 65000;
        }
        if ((last_duty > 3500) && (last_duty < 5500)) { // avoid dead spot
            last_duty = 8000;
        }
        if ((last_duty > 5549) && (last_duty < 7900)) {
            last_duty = 3400;
        }
        // now, update motor!
        if (force_desired > 0) {
            oc_pwm(&oc2, &D[5], &timer4, 0, 0);
            oc_pwm(&oc1, &D[6], &timer2, 20000, last_duty);
        } else {
            oc_pwm(&oc1, &D[6], &timer2, 0, 0);
            oc_pwm(&oc2, &D[5], &timer4, 20000, last_duty);
        }
    }
}

// this function prints all required data. Currently prints through
// serial port, but we want it to eventually print through USB Vendor 
// specific requests.
void printData(_TIMER *self) {
    printf("%d:%d:%d\n",force_desired,force_current,last_duty);
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

int16_t main(void) {
    init_clock();
    init_timer();
    init_ui();
    init_pin();
    init_oc();
    init_uart();

    Config_Motor_Pins();

    last_duty = 0;

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



