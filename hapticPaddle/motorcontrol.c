#include <p24FJ128GB206.h>
#include <stdio.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "uart.h"
#include "usb.h"

#define HELLO       0   // Vendor request that prints "Hello World!"
#define SET_VALS    1   // Vendor request that receives 2 unsigned integer values
#define GET_VALS    2   // Vendor request that returns 2 unsigned integer values
#define PRINT_VALS  3   // Vendor request that prints 2 unsigned integer values 

uint16_t angleVal;

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case HELLO:
            printf("Hello World!\n");
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_VALS:
            temp.w = angleVal;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            //temp.w = val2;
            //BD[EP0IN].address[2] = temp.b[0];
            //BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 2;    // set EP0 IN byte count to 4
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
    uint16_t btn2ReadState;
    uint16_t btn2CurrState = 0;
    uint16_t counter2;

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

    pin_digitalOut(&D[6]);

    pin_digitalOut(&D[7]);
    pin_set(&D[7]);

    oc_pwm(&oc1, &D[6], &timer2, 20000, 30000);

    while (USB_USWSTAT!=CONFIG_STATE) {     
        ServiceUSB();                       
    }

    while(1){
        btn2ReadState = !sw_read(&sw2);

        angleVal = pin_read(&A[5]);     
        ServiceUSB(); 

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
                oc_pwm(&oc1, &D[6], &timer2, 20000, 50000);
            }
            else
            {
                pin_clear(&D[6]);
            }
        }
    }
}