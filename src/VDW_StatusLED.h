#ifndef STATUS_LED_H
#define STATUS_LED_H

#include "Particle.h"
#include "VDW_LEDStatus.h"

template<size_t NUMBER_OF_STATUSES = 5>
class VDW_StatusLEDTarget{
    friend class VDW_LEDStatus;
    public:
        VDW_StatusLEDTarget(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, bool activeLow = false)
            :   _redPin(redPin)
            ,   _greenPin(greenPin)
            ,   _bluePin(bluePin)
            ,   _ledOff(activeLow)
            {}
        VDW_StatusLEDTarget(void (*writePin)(uint8_t, bool), uint8_t redPin, uint8_t greenPin, uint8_t bluePin, bool activeLow = false)
            :   _writePin(writePin)
            ,   _redPin(redPin)
            ,   _greenPin(greenPin)
            ,   _bluePin(bluePin)
            ,   _ledOff(activeLow)
            ,   _externalIO(true)
            {}

        // make sure all pins are off, pin modes should be setup prior to calling init
        void init(){
            if(_externalIO){
                _writePin(_redPin, _ledOff);
                _writePin(_greenPin, _ledOff);
                _writePin(_bluePin, _ledOff);
            } else {
                pinMode(_redPin, OUTPUT);
                pinMode(_greenPin, OUTPUT);
                pinMode(_bluePin, OUTPUT);
                digitalWrite(_redPin, _ledOff);
                digitalWrite(_greenPin, _ledOff;
                digitalWrite(_bluePin, _ledOff);
            }
        }

        // display highest priority active status, run blink patterns and count number of blinks, reset active status if number of blinks exceeds set number
        void update(){
            // find the highest priority, active status
            uint8_t highestActivePriority = 0;
            uint8_t index = numStatuses + 1;
            for(uint8_t i=0; i<numStatuses; i++){
                if(_LEDstatuses[i].active && _LEDstatuses[i].priority > highestActivePriority){
                    highestActivePriority = _LEDstatuses[i].priority;
                    index = i;
                }
            }

            // if all statuses inactive, turn LED off and exit update();
            if(index > numStatuses){
                _writePin(_redPin, LOW);
                _writePin(_greenPin, LOW);
                _writePin(_bluePin, LOW);
                return;
            }

            // if a new status is active, reset _blinkState
            if(index != _lastDisplayedIndex) _blinkState = false;
            _lastDisplayedIndex = index;

            // if solid color (blinkRate == 0) or blink state is ON turn the LED to the specific color
            if(_LEDstatuses[index].blinkRate == 0 || _blinkState == true){
                bool redDir     = _LEDstatuses[index].color >> 2 & 0x01;
                bool greenDir   = _LEDstatuses[index].color >> 1 & 0x01;
                bool blueDir    = _LEDstatuses[index].color & 0x01;

                _writePin(_redPin, redDir);
                _writePin(_greenPin, greenDir);
                _writePin(_bluePin, blueDir);
            } else {
                _writePin(_redPin, LOW);
                _writePin(_greenPin, LOW);
                _writePin(_bluePin, LOW);
            }
            
            // exit update() if solid color
            if(_LEDstatuses[index].blinkRate == 0) return;

            // determine if a blink state change is due
            if(millis() - _lastBlinkTransition > _LEDstatuses[index].blinkRate){
                _lastBlinkTransition = millis();
                _blinkState = !_blinkState;
            }

            // exit update() if blink should go on forever
            if(_LEDstatuses[index].numBlinks == 0) return;

            // increment blinksCompleted when blink cycle is completed, detected by falling edge of blink
            Serial.printlnf("color: %d,\n\t_blinkState: %d\n\t_previousBlinkState: %d\n", _LEDstatuses[index].color, _blinkState, _previousBlinkState);
            delay(50);
            if((_blinkState != _previousBlinkState) && (_previousBlinkState == 1)) _LEDstatuses[index].blinksCompleted += 1;
            _previousBlinkState = _blinkState;

            // if status has blinked numBlinks (and numBlinks was set), set the status inactive
            if(_LEDstatuses[index].blinksCompleted >= _LEDstatuses[index].numBlinks) _LEDstatuses[index].active = false;
        }

    private:
        bool _externalIO = false;
        bool _ledOff = false;
        void (*_writePin)(uint8_t, bool) = NULL;
        uint8_t _redPin = 0;
        uint8_t _greenPin = 0;
        uint8_t _bluePin = 0;
        
        uint32_t _numStatuses = NUMBER_OF_STATUSES;
        VDW_LEDStatus _LEDStatuses[NUMBER_OF_STATUSES];

        uint8_t _statusIndex = 0; // which status should be written to next when createStatus() is called
        uint8_t _lastDisplayedIndex = numStatuses+1;
        bool _blinkState = true; // true if LED is on during blink
        bool _previousBlinkState = false; // blink state for previous update() call, used to determine, rising edges for incrementing blink counts
        uint32_t _lastBlinkTransition = 0; // time in milliseconds of the last blink state change
};

#endif