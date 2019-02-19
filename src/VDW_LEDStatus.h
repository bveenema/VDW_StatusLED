#ifndef LED_STATUS_H
#define LED_STATUS_H

enum StatusLED_Color {
    StatusLED_OFF, //000 --> not sure why this would every be used, led should be off if all statuses are inactive
    StatusLED_Blue, //001
    StatusLED_Green, //010
    StatusLED_Cyan, //011
    StatusLED_Red, //100
    StatusLED_Magenta, //101
    StatusLED_Yellow, //110
    StatusLED_White, //111
};

enum StatusLED_Priority {
    StatusLED_Priority_Low = 1,
    StatusLED_Priority_Med = 4, 
    StatusLED_Priority_High = 7,
    StatusLED_Priority_Critical = 10, 
};

enum StatusLED_Pattern {
    StatusLED_Pattern_Solid,
    StatusLED_Pattern_Blink,
};

enum StatusLED_Speed {
    StatusLED_Speed_Fast = 100,
    StatusLED_Speed_Medium = 350,
    StatusLED_Speed_Slow = 750,
};

class VDW_LEDStatus{
    public:

        // creates a new status with name (max 16 characters), color, blinkRate, number of blinks before being set inactive, and priority
        //  returns false and does not set status if name is already in use
        VDW_LEDStatus(VDW_StatusLEDTarget &TargetLED, StatusLED_Color color);
        VDW_LEDStatus(&TargetLED, StatusLED_Color color, StatusLED_Pattern pattern, StatusLED_Speed speed = StatusLED_Speed_Medium, uint8_t numBlinks = 0);
        VDW_LEDStatus(&TargetLED, StatusLED_Color color, StatusLED_Pattern pattern, uint32_t speed = 350, uint8_t numBlinks = 0);
        VDW_LEDStatus(&TargetLED, StatusLED_Color color, StatusLED_Priority priority);
        VDW_LEDStatus(&TargetLED, StatusLED_Color color, StatusLED_Priority priority, StatusLED_Pattern pattern, StatusLED_Speed speed = StatusLED_Speed_Medium, uint8_t numBlinks = 0);
        VDW_LEDStatus(&TargetLED, StatusLED_Color color, StatusLED_Priority priority, StatusLED_Pattern pattern, uint32_t speed = 350, uint8_t numBlinks = 0);

        // turns the status on or off
        void setStatus(bool isActive){
            _active = isActive;
            _blinksCompleted = 0;
        }

        // changes the priority level (higher is more important) of the status
        bool setPriority(uint8_t priority){
            _priority = priority;
        }

        // makes the status the highest priority in the list and sets it active, then calls update to display the status immediately
        //  WARNING: should not be used except for absolutely critical statuses that will be active for short durations or just before a crash, reset or power-down would occur
        bool displayNow();

    private:

        bool _active = false; // active status means the color and blink rate will be displayed if it's highest priority
        uint8_t _color = 0; // non-hex, last three bits represent red, green, blue ex. (0b00000101) is red and blue ON, green OFF
        uint16_t _blinkRate = 0; // time between blinks, duty cycle is 50%, 0 is constant on
        uint8_t _numBlinks = 0; // number of blinks before active is set to false, 0 is infinate
        uint8_t _blinksCompleted = 0; // the number of times the status has blinked since setStatus() was called
        uint8_t _priority = 0; // active ledStatus with highest priority will be the one displayed, if multiple are same priority, lowest index will be displayed
        uint32_t _index = 0; // position of the status in the parent array



        void printStatus(){
            Serial.printlnf("Status %d", _index);
            Serial.printlnf("\tActive: %d", _active);
            Serial.printlnf("\tColor: %d", _color);
            Serial.printlnf("\tBlink Rate: %d", _blinkRate);
            Serial.printlnf("\tNum Blinks: %d", _numBlinks);
            Serial.printlnf("\tBlinks Completed: %d", _blinksCompleted);
            Serial.printlnf("\tPriority: %d", _priority);
        }
};

#endif