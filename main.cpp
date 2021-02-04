/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"
//#include "USBSerial.h"
#include "usb/USBHID.h"
#include "stepper.hpp"

#define PICARD_USB_TWISTER_SERIAL 1

USBHID hid(false, 8, 8, 0x0461, 0x0021, PICARD_USB_TWISTER_SERIAL);

DigitalOut led(PC_12);

// CMD
// 0x08 - moveTo
// 0x0a, magic -32768 - Cont +
// 0x08, magic -32768 - Cont -
// 0x00 - Halt
// 0x01, magic -32768 - Set zero

Stepper stepper0(D8, D6, D7);
//Stepper stepper1(D12, D5, D4);

Thread reportThread;
Thread commandThread;
Thread motorThread;

void reportThreadMain() {
    HID_REPORT output_report = {
        .length = 8,
        .data = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
    };

    for(;;) {
        led = !led;
        output_report.data[0] = ((0x10 - stepper0.getSpeed()) << 4);

        if (stepper0.isContMode()) {
            output_report.data[0] += 0x0a;
        } else if (stepper0.isMoving()) {
            output_report.data[0] = 0x09;
        } else {
            output_report.data[0] = 0x00;
        }
        int16_t curpos = stepper0.getOffset();
        memcpy(&(output_report.data[1]), &curpos, 2);
        hid.send(&output_report);
        ThisThread::sleep_for(500ms);
    }
}

void commandThreadMain() {
    HID_REPORT command = {
        .length = 8,
        .data = {0x00}
    };

    for(;;) {
        bool isSuccess = hid.read(&command);
        if (!isSuccess) {
            ThisThread::sleep_for(1ms);
            continue;
        }

        uint8_t spd = 0x10 - (command.data[0] >> 4);
        uint8_t cmd = command.data[0] & 0x0f;
        int16_t pos = (int16_t)((uint16_t)(command.data[2] << 8) + command.data[1]);

        stepper0.setSpeed(spd);
        switch (cmd) {
            case 0x08:
            if (pos == -32768) {
                // Cont +
                stepper0.setContMode(1);
            } else {
                stepper0.setDestOffset(pos);
            }
            break;
            case 0x0a:
            if (pos == -32768) {
                stepper0.setContMode(0);
            } else {
                //WTF?
                printf("Uh-oh\r\n");
            }
            break;
            case 0x00:
            stepper0.stop();
            break;
            case 0x01:
            stepper0.reset();
            break;
        }
    }
}

void stepMotor(int16_t destpos) {
    stepper0.stepToward(destpos, 1);
}

void motorThreadMain() {
    for(;;) {
        stepper0.doStepTick();
        ThisThread::yield();
    }
}

int main()
{
    DigitalOut disc(PC_11);
    disc = 0; // Enable USB Pull UP
    printf("USB PullUP activated.\r\n");
    printf("USB activated.\r\n");
    hid.connect();
    printf("HID OK!\r\n");

    reportThread.start(reportThreadMain);
    commandThread.start(commandThreadMain);
    motorThread.start(motorThreadMain);
    for(;;) {
        ThisThread::sleep_for(1s);
    }
}