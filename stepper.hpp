class Stepper {
public:
    Stepper(PinName en, PinName dir, PinName step);
    ~Stepper();
    void init();
    void enable();
    void disable();
    void reset();
    void step(uint8_t dir, uint8_t speed);
    void stepNoCount(uint8_t dir, uint8_t speed);
    void stepToward(int16_t offset, uint8_t speed);
    void doStepTick();
    void stop();
    void setDestOffset(int16_t offset);
    void setSpeed(uint8_t speed);
    void setContMode(uint8_t dir);
    int16_t getOffset();
    uint8_t getSpeed();
    bool isMoving();
    bool isContMode();

protected:
    DigitalOut en;
    DigitalOut dir;
    DigitalOut pulse;
    uint8_t contdir;
    uint8_t speed;
    int16_t offset;
    int16_t destOffset;
};