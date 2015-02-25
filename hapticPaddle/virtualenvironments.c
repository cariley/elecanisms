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
#include "usb.h"

#define SET_VALS    1   // Vendor request that receives 2 unsigned integer values

#define SPRING      1
#define DAMPED      2
#define WALL        3
#define TEXTURE     4

// Position tracking variables
int16_t rawPos;         // current raw reading from MR sensor
int16_t lastRawPos;     // last raw reading from MR sensor
int16_t lastLastRawPos; // last last raw reading from MR sensor
int16_t flipNumber;     // keeps track of the number of flips over the 180deg mark
int16_t lastRawDiff;
int16_t lastRawOffset;
int16_t cumulativeVal;      // cumulative sensor value
int16_t last_position;
int16_t current_position = 0; // cumulative sensor value - initial position

// more position tracking variables?? need to hunt down exactly what these ones do.
int16_t initPos;
uint16_t flipThresh = 600;  // threshold to determine whether or not a flip over the 180 degree mark occurred
bool flipped = false;

// used for controlled torque modes: spring, damper, and perhaps texture
int16_t force_desired; // how much force do we want applied to the joystick, given the current control law
// (arbitrary units)
int16_t force_current; // how much force are we currently applying (motor current reading)
int16_t last_duty; // keeps track of the last duty cycle used for the simple integrator control loop for
// controlling force.
// *********************************************** IMPORTANT!! BOARD REDESIGN REQUIRED *******************
int16_t duty_temp; // so it turns out there's a noisy section when trying to measure current at the wrong
// drive frequency. later it would be nice to implement a hardware filter. for now, since this region is 
// unusable (and the PID loop actually gets stuck here), temporarily just avoid this region if you're 
// control loop wants to enter it.
uint16_t kI = 100;


int16_t command; // which state is the FSM in?
int16_t argument; // for states that also require a setting, what is that setting?

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

void updatePosition() {
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
    last_position = current_position;
    current_position = cumulativeVal-initPos;
}

void stop_motor(void){
    oc_pwm(&oc2, &D[5], &timer4, 0, 0);
    oc_pwm(&oc1, &D[6], &timer2, 0, 0);
}

// this function measures the current torque (analog reading of motor current), then compares
// this value to the desired torque, then slightly increases or decreases the motor voltage
// (PWM) to compensate. If called frequently enough, actual torque will catch up with desired torque.
// It could be more efficiently replaced with an actual PID loop.
void set_torque() { // should hand in desired torque, but currently all our variables are global . . . :(
    force_current = pin_read(&A[0]) >> 6; // read the current sense resistor

    if (myAbs(myAbs(force_desired)-force_current)>2) { // significant enough to change the motor speed
        if (myAbs(force_desired)-force_current < 0) { // want less!!
            last_duty -= kI;
        }
        else {
            last_duty += kI; // want MORE!!
        }
        //deal with extreme cases
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

void Update_status(_TIMER *self){
    updatePosition(); // check the magnetoresistive sensor to make sure we always keep track of where
    // we are
    switch(command) {
        case SPRING: 
            led_on(&led1); led_off(&led2); led_off(&led3); // for visual feedback (debugging).
            force_desired = -(current_position) >> 6; // arbitrary units. will fix later
            force_desired = force_desired * 15;
            set_torque();
            break;
        case DAMPED:
            led_on(&led1); led_off(&led2); led_off(&led3); // for visual feedback (debugging).
            force_desired = last_position - current_position;
            force_desired = force_desired * 15;
            set_torque();
            break;
        case TEXTURE:
            if ((current_position % 600) > 300) {
                force_desired = -(current_position) >> 6;
                force_desired = force_desired * 8;
                set_torque();
            } else {
                stop_motor();
            }
            break;
        case WALL:
            led_on(&led1); led_on(&led2); led_on(&led3); // for visual feedback (debugging).
            if (current_position > (initPos+2000)) {
                force_desired = -(current_position) >> 6;
                force_desired = force_desired * 15;
                set_torque();
            } else if (current_position < (initPos-2000)) {
                force_desired = -(current_position) >> 6;
                force_desired = force_desired * 15;
                set_torque();
            } else {
                stop_motor();
            }
            break;
        default :
            stop_motor();
            break;
    }

}

void printData(_TIMER *self) {
    printf("%d:%d:%d\n",current_position,last_position - current_position,force_current);
}



void VendorRequests(void) { // deal with 
    WORD temp;

    switch (USB_setup.bRequest) {
        case SET_VALS: // they should all be set_vals, worth checking just to be sure.
            command = USB_setup.wValue.w;
            argument = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
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
    init_uart();
    init_ui();
    init_timer();
    init_pin();
    init_oc();

    command = 0; // just supplying defaults
    argument = 0;

    Config_Motor_Pins();

    lastLastRawPos = pin_read(&A[5]) >> 6;
    lastRawPos = pin_read(&A[5]) >> 6;
    initPos = lastLastRawPos;

    InitUSB();
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }

    timer_every(&timer3,.0005, Update_status); // check the state of the FSM, then check
    // the position of the motor, then update the voltage to the motor (PWM) based on
    // what state it is in and what it should be doing.

    timer_every(&timer1,.01,printData);

    while(1) {
        ServiceUSB(); 
    }
}

