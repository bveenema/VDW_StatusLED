#include "VDW_LEDStatus.h"

int8_t VDW_LEDStatus::findStatus(const char *name){
    int8_t returnVal = -1;
    for(uint8_t i=0; i<numStatuses; i++){
        if(strncmp(name, _LEDstatuses[i].name, 16) == 0){
            returnVal = i;
        }
    }
    return returnVal;
}

bool VDW_LEDStatus::createStatus(const char *name, Sensor_LED_Color color){
    return createStatus(name, color, VDW_LEDStatus_Priority_Low, 0, 0);
}
bool VDW_LEDStatus::createStatus(const char *name, Sensor_LED_Color color, uint16_t blinkRate, uint8_t numBlinks){
    return createStatus(name, color, VDW_LEDStatus_Priority_Low, blinkRate, numBlinks);
}
bool VDW_LEDStatus::createStatus(const char *name, Sensor_LED_Color color, Sensor_LED_Priority priority, uint16_t blinkRate, uint8_t numBlinks){
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

// makes the named status the highest priority in the list and sets it active, then calls update, returns 0 if name is not found in status list
//  WARNING: should not be used except for absolutely critical statuses that will be active for short durations or just before a crash, reset or power-down would occur
bool VDW_LEDStatus::displayNow(){
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