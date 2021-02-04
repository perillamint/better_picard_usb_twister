#include "mbed.h"
#include "stepper.hpp"

Stepper::Stepper(PinName en, PinName dir, PinName step): en{en}, dir{dir}, pulse{step} {
    this -> offset = 0;
    this -> destOffset = 0;
    this -> speed = 1;
    this -> contdir = 0;
}

Stepper::~Stepper() {
    //
}

void Stepper::init() {
    this -> dir = 0;
    this -> en = 0;
    this -> pulse = 0;
}

void Stepper::enable() {
    this -> en = 0;
}

void Stepper::disable() {
    this -> en = 1;
}

void Stepper::reset() {
    this -> offset = 0;
}

void Stepper::step(uint8_t dir, uint8_t speed) {
    if (dir > 1) return;
    if (dir == 1) {
        this -> offset += 1;
    } else {
        this -> offset -= 1;
    }
    this -> stepNoCount(dir, speed);
}

void Stepper::stepNoCount(uint8_t dir, uint8_t speed) {
    if (dir > 1) return;
    this -> dir = dir;
    this -> pulse = 1;
    ThisThread::sleep_for(1ms); // TODO: Make it adjustable
    this -> pulse = 0;
    ThisThread::sleep_for(1ms); // TODO: Make it adjustable
}

void Stepper::stepToward(int16_t offset, uint8_t speed) {
    if (this -> offset > offset) {
        this -> step(0, speed);
    } else if (this -> offset < offset) {
        this -> step(1, speed);
    }
}

void Stepper::doStepTick() {
    if (this -> contdir == 0) {
        this -> stepToward(this -> destOffset, this -> speed);
    } else {
        this -> stepNoCount(this -> contdir - 1, this -> speed);
    }
}

void Stepper::stop() {
    this -> contdir = 0;
    this -> destOffset = this -> offset;
}

void Stepper::setDestOffset(int16_t offset) {
    this -> destOffset = offset;
}

void Stepper::setSpeed(uint8_t speed) {
    this -> contdir = 0;
    this -> speed = speed;
}

void Stepper::setContMode(uint8_t dir) {
    if (dir > 1) {
        this -> contdir = 0;
    } else {
        this -> contdir = dir + 1;
    }
}

int16_t Stepper::getOffset() {
    return this -> offset;
}

uint8_t Stepper::getSpeed() {
    return this -> speed;
}

bool Stepper::isMoving() {
    return this -> offset != this -> destOffset;
}

bool Stepper::isContMode() {
    return this -> contdir != 0;
}