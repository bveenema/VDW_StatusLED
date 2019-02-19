#ifndef SENSOR_LED_H
#define SENSOR_LED_H

#include "Particle.h"
#include "pins.h"
#include "globals.h"

enum Sensor_LED_Priority{
    SensorLED_Priority_Low = 1,
    SensorLED_Priority_Med = 4, 
    SensorLED_Priority_High = 7,
    SensorLED_Priority_Critical = 10, 
};

enum Sensor_LED_Color{
    SensorLED_OFF, //000 --> not sure why this would every be used, led should be off if all statuses are inactive
    SensorLED_Blue, //001
    SensorLED_Green, //010
    SensorLED_Cyan, //011
    SensorLED_Red, //100
    SensorLED_Magenta, //101
    SensorLED_Yellow, //110
    SensorLED_White, //111
};

class SensorLED{
    public:
        SensorLED(void (*writePin)(uint8_t, bool), uint8_t redPin, uint8_t greenPin, uint8_t bluePin)
            :   _writePin(writePin)
            ,   _redPin(redPin)
            ,   _greenPin(greenPin)
            ,   _bluePin(bluePin)
            {}

        // creates a new status with name (max 16 characters), color, blinkRate, number of blinks before being set inactive, and priority
        //  returns false and does not set status if name is already in use
        bool createStatus(const char *name, Sensor_LED_Color color);
        bool createStatus(const char *name, Sensor_LED_Color color, uint16_t blinkRate, uint8_t numBlinks = 0);
        bool createStatus(const char *name, Sensor_LED_Color color, Sensor_LED_Priority priority, uint16_t blinkRate = 0, uint8_t numBlinks = 0);

        // turns the status given by name on or off, returns 0 if name is not found in status list
        bool setStatus(const char *name, bool isActive);

        // changes the priority level (higher is more important) of the named status, returns 0 if name is not found in status list
        bool setPriority(const char *name, uint8_t priority);

        // makes the named status the highest priority in the list and sets it active, then calls update to display the status immediately, returns 0 if name is not found in status list
        //  WARNING: should not be used except for absolutely critical statuses that will be active for short durations or just before a crash, reset or power-down would occur
        bool displayNow(const char *name);

        // make sure all pins are off, pin modes should be setup prior to calling init
        void init();

        // display highest priority active status, run blink patterns and count number of blinks, reset active status if number of blinks exceeds set number
        void update();
    private:
        void (*_writePin)(uint8_t, bool) = NULL;
        uint8_t _redPin = 0;
        uint8_t _greenPin = 0;
        uint8_t _bluePin = 0;
        struct ledStatus{
            char name[16] = ""; // name of the status, will be matched (strcmp) when setStatus() is called
            bool active = false; // active status means the color and blink rate will be displayed if it's highest priority
            uint8_t color = 0; // non-hex, last three bits represent red, green, blue ex. (0b00000101) is red and blue ON, green OFF
            uint16_t blinkRate = 0; // time between blinks, duty cycle is 50%, 0 is constant on
            uint8_t numBlinks = 0; // number of blinks before active is set to false, 0 is infinate
            uint8_t blinksCompleted = 0; // the number of times the status has blinked since setStatus() was called
            uint8_t priority = 0; // active ledStatus with highest priority will be the one displayed, if multiple are same priority, lowest index will be displayed
        };
        static const uint8_t numStatuses = 10;
        ledStatus _LEDstatuses[numStatuses]; // up to 10 statuses can be created for the led, if more than 10 are created, they will ovewrite, starting with the first
        uint8_t _statusIndex = 0; // which status should be written to next when createStatus() is called
        uint8_t _lastDisplayedIndex = numStatuses+1;
        bool _blinkState = true; // true if LED is on during blink
        bool _previousBlinkState = false; // blink state for previous update() call, used to determine, rising edges for incrementing blink counts
        uint32_t _lastBlinkTransition = 0; // time in milliseconds of the last blink state change

        // returns the index of the matching LED status, returns -1 if no match
        int8_t findStatus(const char *name);
        void printStatus(uint8_t index){
            Serial.printlnf("Status %d",index);
            Serial.printlnf("\tName: %s", _LEDstatuses[index].name);
            Serial.printlnf("\tActive: %d", _LEDstatuses[index].active);
            Serial.printlnf("\tColor: %d", _LEDstatuses[index].color);
            Serial.printlnf("\tBlink Rate: %d", _LEDstatuses[index].blinkRate);
            Serial.printlnf("\tNum Blinks: %d", _LEDstatuses[index].numBlinks);
            Serial.printlnf("\tBlinks Completed: %d", _LEDstatuses[index].blinksCompleted);
            Serial.printlnf("\tPriority: %d", _LEDstatuses[index].priority);
        }
};

extern SensorLED RED_SensorLED;
extern SensorLED BLUE_SensorLED;

#endif