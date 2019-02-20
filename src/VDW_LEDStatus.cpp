#include "VDW_LEDStatus.h"
#include "VDW_StatusLEDTarget.h"



VDW_LEDStatus::VDW_LEDStatus(VDW_StatusLEDTarget *TargetLED, StatusLED_Color color){
    construct(TargetLED, color, StatusLED_Priority_Low, StatusLED_Pattern_Solid, 0, 0);
}
VDW_LEDStatus::VDW_LEDStatus(VDW_StatusLEDTarget *TargetLED, StatusLED_Color color, StatusLED_Pattern pattern, StatusLED_Speed speed, uint32_t numBlinks){
    construct(TargetLED, color, StatusLED_Priority_Low, pattern, speed, numBlinks);
}
VDW_LEDStatus::VDW_LEDStatus(VDW_StatusLEDTarget *TargetLED, StatusLED_Color color, StatusLED_Pattern pattern, uint32_t blinkRate, uint32_t numBlinks){
    construct(TargetLED, color, StatusLED_Priority_Low, pattern, blinkRate, numBlinks);
}
VDW_LEDStatus::VDW_LEDStatus(VDW_StatusLEDTarget *TargetLED, StatusLED_Color color, StatusLED_Priority priority){
    construct(TargetLED, color, priority, StatusLED_Pattern_Solid, 0, 0);
}
VDW_LEDStatus::VDW_LEDStatus(VDW_StatusLEDTarget *TargetLED, StatusLED_Color color, StatusLED_Priority priority, StatusLED_Pattern pattern, StatusLED_Speed speed, uint32_t numBlinks){
    construct(TargetLED, color, priority, pattern, speed, numBlinks);
}
VDW_LEDStatus::VDW_LEDStatus(VDW_StatusLEDTarget *TargetLED, StatusLED_Color color, StatusLED_Priority priority, StatusLED_Pattern pattern, uint32_t blinkRate, uint32_t numBlinks){
    construct(TargetLED, color, priority, pattern, blinkRate, numBlinks);
}

void VDW_LEDStatus::construct(VDW_StatusLEDTarget *TargetLED, StatusLED_Color color, StatusLED_Priority priority, StatusLED_Pattern pattern, uint32_t blinkRate, uint32_t numBlinks){
    _TargetLED = TargetLED;
    _color = color;
    _priority = priority;
    _pattern = pattern;
    if(_pattern != StatusLED_Pattern_Solid) _blinkRate = blinkRate;
    else _blinkRate = 0;
    _numBlinks = numBlinks;

    _nextStatus = _TargetLED->addStatus(this);
}

// makes the named status the highest priority in the list and sets it active, then calls update, returns 0 if name is not found in status list
//  WARNING: should not be used except for absolutely critical statuses that will be active for short durations or just before a crash, reset or power-down would occur
bool VDW_LEDStatus::displayNow(){
    // remove the status from the current location in the list and insert it at the beginning of the list
    _nextStatus = _TargetLED->removeStatus(this);
    _nextStatus = _TargetLED->pushBack(this);

    // make the status active, and  
    setStatus(Active);
    _TargetLED->update();
    
    return true;
}