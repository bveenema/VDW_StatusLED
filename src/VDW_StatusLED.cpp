#include "sensorLED.h"

SensorLED RED_SensorLED(writeIO2, IO2_LED_LEFT_RED, IO2_LED_LEFT_GREEN, IO2_LED_LEFT_BLUE);
SensorLED BLUE_SensorLED(writeIO2, IO2_LED_RIGHT_RED, IO2_LED_RIGHT_GREEN, IO2_LED_RIGHT_BLUE);

int8_t SensorLED::findStatus(const char *name){
    int8_t returnVal = -1;
    for(uint8_t i=0; i<numStatuses; i++){
        if(strncmp(name, _LEDstatuses[i].name, 16) == 0){
            returnVal = i;
        }
    }
    return returnVal;
}

bool SensorLED::createStatus(const char *name, Sensor_LED_Color color){
    return createStatus(name, color, SensorLED_Priority_Low, 0, 0);
}
bool SensorLED::createStatus(const char *name, Sensor_LED_Color color, uint16_t blinkRate, uint8_t numBlinks){
    return createStatus(name, color, SensorLED_Priority_Low, blinkRate, numBlinks);
}
bool SensorLED::createStatus(const char *name, Sensor_LED_Color color, Sensor_LED_Priority priority, uint16_t blinkRate, uint8_t numBlinks){
    Serial.printlnf("Create Status:\n\tName: %s\n\tColor: %d", name, color);
    // check to make sure name isn't already used
    if(findStatus(name) != -1) return false;

    strncpy(_LEDstatuses[_statusIndex].name, name, 16);
    _LEDstatuses[_statusIndex].active = false;
    _LEDstatuses[_statusIndex].color = color;
    _LEDstatuses[_statusIndex].blinkRate = blinkRate;
    _LEDstatuses[_statusIndex].numBlinks = numBlinks;
    _LEDstatuses[_statusIndex].blinksCompleted = 0;
    _LEDstatuses[_statusIndex].priority = priority;
    printStatus(_statusIndex);

    // increment the status index, reset if greater than list size
    _statusIndex ++;
    if(_statusIndex >= numStatuses) _statusIndex = 0;

    return true;
}

// turns the status given by name on or off, returns 0 if name is not found in status list
bool SensorLED::setStatus(const char *name, bool isActive){
    int8_t statusIndex = findStatus(name);
    if(statusIndex < 0) return false;

    _LEDstatuses[(uint8_t)statusIndex].active = isActive;
    _LEDstatuses[(uint8_t)statusIndex].blinksCompleted = 0;
    
    return true;
}

// changes the priority level (higher is more important) of the named status, returns 0 if name is not found in status list
bool SensorLED::setPriority(const char *name, uint8_t priority){
    int8_t statusIndex = findStatus(name);
    if(statusIndex < 0) return false;

    _LEDstatuses[(uint8_t)statusIndex].priority = priority;
    
    return true;
}

// makes the named status the highest priority in the list and sets it active, then calls update, returns 0 if name is not found in status list
//  WARNING: should not be used except for absolutely critical statuses that will be active for short durations or just before a crash, reset or power-down would occur
bool SensorLED::displayNow(const char *name){
    int8_t statusIndex = findStatus(name);
    if(statusIndex < 0) return false;

    // find the highest priority in any status
    uint8_t highestPriority = 0;
    for(uint8_t i=0; i<numStatuses; i++){
        if(_LEDstatuses[i].priority > highestPriority) highestPriority = _LEDstatuses[i].priority;
    }

    // make the status active, and  
    _LEDstatuses[(uint8_t)statusIndex].active = true;
    _LEDstatuses[(uint8_t)statusIndex].blinksCompleted = 0;
    _LEDstatuses[(uint8_t)statusIndex].priority = highestPriority + 1;
    update();
    
    return true;
}

// make sure all pins are off, pin modes should be setup prior to calling init
void SensorLED::init(){
    _writePin(_redPin, LOW);
    _writePin(_greenPin, LOW);
    _writePin(_bluePin, LOW);
}

// display highest priority active status, run blink patterns and count number of blinks, reset active status if number of blinks exceeds set number
void SensorLED::update(){
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