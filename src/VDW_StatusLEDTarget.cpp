#include "VDW_StatusLEDTarget.h"
#include "VDW_LEDStatus.h"

void VDW_StatusLEDTarget::init(){
    if(!_externalIO){
        pinMode(_redPin, OUTPUT);
        pinMode(_greenPin, OUTPUT);
        pinMode(_bluePin, OUTPUT);
    }
    _writePin(_redPin, _ledOff);
    _writePin(_greenPin, _ledOff);
    _writePin(_bluePin, _ledOff);
}

LEDStatusPtr VDW_StatusLEDTarget::addStatus(LEDStatusPtr status){
    // Insert Elements sorted by priority, highest to lowest
    LEDStatusPtr nStatus = _headStatusList; // head is NULL if no elements in list
    LEDStatusPtr pStatus = NULL;
    while(nStatus){
        if(nStatus->_priority <= status->_priority) break; // find the element where the priority is less than or equal to the new element
        pStatus = nStatus; // record the status that will be just before the status
        nStatus = nStatus->_nextStatus; // get the next status to test against, if we get to the end of the list, _nextStatus will be NULL
    }

    // The previous element should now reference the incoming element
    if(pStatus){
        pStatus->_nextStatus = status;
    } else{ // if there was no previous element, this is the highest priority element and therefor the new head
        _headStatusList = status;
    }

    // the incoming element should reference the next element
    return nStatus; // will be NULL if first element or last element
}

LEDStatusPtr VDW_StatusLEDTarget::pushBack(LEDStatusPtr status){
    // insert the incoming element to the beginning of the list
    LEDStatusPtr tempStatus = _headStatusList;
    _headStatusList = status;

    return tempStatus;
}

LEDStatusPtr VDW_StatusLEDTarget::removeStatus(LEDStatusPtr status){
    Serial.println("Before Removal");
    printStatuses();
    // find the elements just before and after the incoming element
    LEDStatusPtr nStatus = _headStatusList;
    LEDStatusPtr pStatus = NULL;
    while(nStatus){
        if(nStatus == status){
            nStatus = nStatus->_nextStatus;
            break;
        }
        pStatus = nStatus;
        nStatus = nStatus->_nextStatus;
    }

    // set the previous element to point to the element after the incoming element
    pStatus->_nextStatus = nStatus;
    Serial.println("After Removal");
    printStatuses();

    // return a NULL pointer
    return NULL;
}

// display highest priority active status, run blink patterns and count number of blinks, reset active status if number of blinks exceeds set number
void VDW_StatusLEDTarget::update(){

    LEDStatusPtr aStatus = NULL; // the "active" status

    // find the highest priority, active status. list is sorted by priority ==> first active element is highest priority, active element
    LEDStatusPtr cStatus = _headStatusList;
    while(cStatus != NULL){
        if(cStatus->_active){
            aStatus = cStatus;
            break;
        }
        cStatus = cStatus->_nextStatus;
    }

    // if all statuses inactive, turn LED off and exit update();
    if(aStatus == NULL){
        _writePin(_redPin, LOW);
        _writePin(_greenPin, LOW);
        _writePin(_bluePin, LOW);
        return;
    }

    // if a new status is active, reset _blinkState
    if(aStatus != _lastActiveStatus) _blinkState = false;
    _lastActiveStatus = aStatus;

    // if solid color (blinkRate == 0) or blink state is ON turn the LED to the specific color
    if(aStatus->_blinkRate == 0 || _blinkState == true){
        bool redDir     = (aStatus->_color >> 2 & 0x01)^_ledOff; // XOR with _ledOff to get correct polarity for active low LEDs
        bool greenDir   = (aStatus->_color >> 1 & 0x01)^_ledOff; // ditto
        bool blueDir    = (aStatus->_color & 0x01)^_ledOff;      // ditto^2
        _writePin(_redPin, redDir);
        _writePin(_greenPin, greenDir);
        _writePin(_bluePin, blueDir);
    } else {
        _writePin(_redPin, _ledOff);
        _writePin(_greenPin, _ledOff);
        _writePin(_bluePin, _ledOff);
    }

    // exit update() if solid color
    if(aStatus->_blinkRate == 0) return;

    // determine if a blink state change is due
    if(millis() - _lastBlinkTransition > aStatus->_blinkRate){
        _lastBlinkTransition = millis();
        _blinkState = !_blinkState;
    }

    // exit update() if blink should go on forever
    if(aStatus->_numBlinks == 0) return;

    // increment blinksCompleted when blink cycle is completed, detected by falling edge of blink
    if((_blinkState != _previousBlinkState) && (_previousBlinkState == 1)) aStatus->_blinksCompleted += 1;
    _previousBlinkState = _blinkState;

    // if status has blinked numBlinks (and numBlinks was set), set the status inactive
    if(aStatus->_blinksCompleted >= aStatus->_numBlinks) aStatus->_active = false;

    return;
}

void VDW_StatusLEDTarget::printStatuses(){
    Serial.println("----------");
    LEDStatusPtr status = _headStatusList;
    while(status){
        status->printStatus();
        status = status->_nextStatus;
    }
}