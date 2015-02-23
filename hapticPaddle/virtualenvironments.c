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

#define SET_VALS    0   // Vendor request that receives 2 unsigned integer values
#define SPRING      1
#define DAMPED      2
#define WALL        3
#define TEXTURE     4
#define INTTHRESH   700
#define FLIPTHRESH  600
#define POSWALL     1200
#define NEGWALL     -1200
#define DUTYSCALE   5000

// Position tracking variables
int16_t rawPos;         // current raw reading from MR sensor
int16_t lastRawPos;     // last raw reading from MR sensor
int16_t lastLastRawPos; // last last raw reading from MR sensor
int16_t flipNumber;     // keeps track of the number of flips over the 180deg mark
int16_t lastRawDiff;
int16_t lastRawOffset;
int16_t cumulativePos;      // cumulative sensor value
bool clockwise;
bool flipped = false;

int16_t initPos;
int16_t oldInitPos;

int16_t error;
int16_t lastPos;
int16_t p, i, d;
int16_t duty;
int16_t integral = 0;
uint16_t kP = 17;
uint16_t kI = 0;
uint16_t kD = 0;

bool walls = false;

int16_t scaledPos;

void (*environment)(void);

void calculatePos(_TIMER *self) {
    rawPos = pin_read(&A[5]) >> 6;
     
    lastRawDiff = rawPos - lastLastRawPos; 
    lastRawOffset = abs(lastRawDiff);
    
    lastLastRawPos = lastRawPos;
    lastRawPos = rawPos;
    
    //check for flip and increment or decrement accordingly
    if((lastRawOffset > FLIPTHRESH) && (!flipped)) { 
        if(lastRawDiff > 0) {        
            flipNumber--;             
        } else {                     
            flipNumber++;             
        }
        flipped = true;          
    } else {                       
        flipped = false;
    }

    if (lastRawDiff < 0) {
        clockwise = false;
    } else {
        clockwise = true;
    }

    cumulativePos = rawPos + flipNumber*700;    //each flip changes cumulative value by 700

    pidControl();
}

void pidControl(void) {
    error = initPos - cumulativePos;

    if (abs(error) < INTTHRESH) {
        integral = integral + error;
    }
    else {
        integral = 0;
    }

    p = error*kP;                   // proportional term
    i = integral*kI;                // integral term
    d = (lastPos - cumulativePos)*kD; // derivative term
    duty = p + i + d;

    if (walls) {
        if (cumulativePos != POSWALL && cumulativePos != NEGWALL) {
            duty = 0;
        }
    }

    printf("%d\n", duty);
        
    if (duty == 0) {
        //oc_pwm(&oc1, &D[5], &timer4, 0, 0);
        //oc_pwm(&oc1, &D[6], &timer2, 0, 0);
    } else if (duty < 0) {
       // oc_pwm(&oc2, &D[5], &timer4, 0, 0);
        //oc_pwm(&oc1, &D[6], &timer2, 0, 0);
    } else {
        //oc_pwm(&oc1, &D[6], &timer2, 0, 0);
        //oc_pwm(&oc2, &D[5], &timer4, 0, 0);
    }

    lastPos = cumulativePos;
}

void stopMotor(_TIMER *self) {
    oc_pwm(&oc1, &D[5], &timer4, 0, 0);
    oc_pwm(&oc1, &D[6], &timer2, 0, 0);
}

void texture(void) {
    scaledPos = cumulativePos >> 10;

    if (scaledPos % 2 == 0) {
        if (clockwise) {
            oc_pwm(&oc2, &D[5], &timer4, 0, 0);
            oc_pwm(&oc1, &D[6], &timer2, 20000, abs(duty) * DUTYSCALE);
        } else {
            oc_pwm(&oc1, &D[6], &timer2, 0, 0);
            oc_pwm(&oc2, &D[5], &timer4, 20000, abs(duty) * DUTYSCALE);
        }
    } else {
        oc_pwm(&oc1, &D[5], &timer4, 0, 0);
        oc_pwm(&oc1, &D[6], &timer2, 0, 0);
    }
}

void printError(_TIMER *self) {
    printf("%d\n",error);
}

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case SET_VALS:
            kP = USB_setup.wValue.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SPRING:
            environment = &pidControl;
            walls = false;
            kP = 17;
            kI = 0;
            kD = 0;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case DAMPED:
            environment = &pidControl;
            walls = false;
            kP = 10;
            kI = 2;
            kD = 5;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case WALL:
            environment = &pidControl;
            walls = true;
            kP = 10;
            kI = 2;
            kD = 5;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case TEXTURE:
            environment = &texture;
            walls = false;
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

int16_t main(void) {
    init_clock();
    init_timer();
    init_ui();
    init_pin();
    init_oc();
    init_uart();
    InitUSB();

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

    pin_digitalOut(&D[6]);
    pin_clear(&D[6]);

    lastLastRawPos = pin_read(&A[5]) >> 6;
    lastRawPos = pin_read(&A[5]) >> 6;
    initPos = lastLastRawPos;
    lastPos = initPos;
    oldInitPos = initPos;

    environment = &pidControl;

    //timer_every(&timer1,.5,printForce);     //report position

    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }

    timer_every(&timer3,.0005,calculatePos); //keep track of position

    while(1) {
        ServiceUSB(); 
        //calculatePos();
        //pidControl();
    }
}