/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"
//#include "USBSerial.h"
#include "usb/USBHID.h"

#define PICARD_USB_TWISTER_SERIAL 1

USBHID hid(false, 8, 8, 0x0461, 0x0021, PICARD_USB_TWISTER_SERIAL);

DigitalOut led(PC_12);

HID_REPORT output_report = {
    .length = 8,
    .data = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

HID_REPORT command = {
    .length = 8,
    .data = {0x00}
};

struct lastCmd {
    uint8_t spd;
    uint8_t cmd;
    int16_t pos;
} lastCmd;

int16_t curpos = 0;

Thread reportThread;
Thread commandThread;
Thread motorThread;

// CMD
// 0x08 - moveTo
// 0x0a, magic -32768 - Cont +
// 0x08, magic -32768 - Cont -
// 0x00 - Halt
// 0x01, magic -32768 - Set zero

void reportThreadMain() {
    for(;;) {
        led = !led;           
        hid.send(&output_report);
        ThisThread::sleep_for(500ms);
    }
}

void commandThreadMain() {
    for(;;) {
        bool isSuccess = hid.read(&command);
        if (!isSuccess) {
            ThisThread::sleep_for(1ms);
            continue;
        }

        lastCmd.spd = 0x10 - (command.data[0] >> 4);
        lastCmd.cmd = command.data[0] & 0x0f;
        lastCmd.pos = (int16_t)((uint16_t)(command.data[2] << 8) + command.data[1]);
    }
}

void motorThreadMain() {
    for(;;) {
        ThisThread::sleep_for(1s);
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