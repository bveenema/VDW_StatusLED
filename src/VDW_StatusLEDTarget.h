#ifndef STATUS_LED_TARGET_H
#define STATUS_LED_TARGET_H

#include "Particle.h"

class VDW_LEDStatus;
typedef VDW_LEDStatus* StatusPtr;

inline void digitalWriteParamConverter(uint8_t pin , bool dir){
    digitalWrite(pin, dir);
}

class VDW_StatusLEDTarget{
        public:
        // Native Pin constructor
        VDW_StatusLEDTarget(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, bool activeLow = false)
            :   _writePin(digitalWriteParamConverter)
            ,   _redPin(redPin)
            ,   _greenPin(greenPin)
            ,   _bluePin(bluePin)
            ,   _ledOff(activeLow)
            ,   _externalIO(false)
            {}

        // External IO pin constructor
        VDW_StatusLEDTarget(void (*writePin)(uint8_t, bool), uint8_t redPin, uint8_t greenPin, uint8_t bluePin, bool activeLow = false)
            :   _writePin(writePin)
            ,   _redPin(redPin)
            ,   _greenPin(greenPin)
            ,   _bluePin(bluePin)
            ,   _ledOff(activeLow)
            ,   _externalIO(true)
            {}

        // make sure all pins are off, pin modes should be setup prior to calling init
        void init();
        
        StatusPtr addStatus(StatusPtr status);
        StatusPtr pushBack(StatusPtr status);
        StatusPtr removeStatus(StatusPtr status);

        // display highest priority active status, run blink patterns and count number of blinks, reset active status if number of blinks exceeds set number
        void update();

    private:  
        // Constructor Settings
        void (*_writePin)(uint8_t, bool);
        uint8_t _redPin;
        uint8_t _greenPin;
        uint8_t _bluePin;
        bool _ledOff;
        bool _externalIO;
        
        // Status List
        StatusPtr _headStatusList = NULL;
        StatusPtr _lastActiveStatus = NULL;        

        // Blink Control
        bool _blinkState = true; // true if LED is on during blink
        bool _previousBlinkState = false; // blink state for previous update() call, used to determine, rising edges for incrementing blink counts
        uint32_t _lastBlinkTransition = 0; // time in milliseconds of the last blink state change

        void printStatuses();
};

#endif