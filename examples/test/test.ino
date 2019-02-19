#include "VDW_StatusLED.h"

#define RED_PIN D0
#define GREEN_PIN D1
#define BLUE_PIN D2

// Native Particle pin constructor
VDW_StatusLEDTarget<> LEDTarget(RED_PIN, GREEN_PIN, BLUE_PIN);

// Alternative IO Expansion constructor
	// writeFunc must be a function of the form --> void writeFunc(uint8_t pin, bool dir)
	VDW_StatusLED<> LEDTarget(writeFunc, RED_PIN, GREEN_PIN, BLUE_PIN);
	// Example "writeFunc"
	void writeFunc(uint8_t pin, bool dir){
		IOEXP.writePin(pin, dir);
	}

// Create Statuses for the LEDTarget
VDW_LEDStatus solidRedLowPriority(LEDTarget, StatusLED_Color_Red); // constructor for solid red LED with default priority (low)
VDW_LEDStatus blinkingGreenLowPriority(LEDTarget, StatusLED_Color_Green, StatusLED_Pattern_Blink, StatusLED_Speed_Medium); // constructor for medium speed blinking green LED, default priority
VDW_LEDStatus customBlinkBlueLowPriority(LEDTarget, StatusLED_Color_Blue, StatusLED_Pattern_Blink, 50); // constructor for 50ms on, 50ms off (100ms period 50% duty) blinking blue LED, default priority
VDW_LEDStatus solidCyanHighPriority(LEDTarget, StatusLED_Color_Cyan, StatusLED_Priority_High); // constructor for solid Cyan LED, high priority
VDW_LEDStatus 10blinkMagentaCriticalPriority(LEDTarget, StatusLED_Color_Cyan, StatusLED_Priority_Critical, StatusLED_Pattern_Blink, StatusLED_Speed_Fast, 10); // constructor for 10 fast blinks, Cyan, critical priority that will self-deactive after 10 blinks, to allow other, lower priority statuses to dislplay 

void setup(){
	// Initialize Serial
  Serial.begin(230400);

	// Initialize the target LED
	LEDTarget.init();
	// if the alternative IO Expansion constructor is used, then the user is required to intilize the pins
	// EX.
		// IOEXP.setupPin(RED_PIN, OUTPUT);
		// IOEXP.setupPin(GREEN_PIN, OUTPUT);
		// IOEXP.setupPin(BLUE_PIN, OUTPUT);

}

void loop(){
	// Update the LEDTarget, checks for statuses to be activated and displays them
	LEDTarget.update();

	runTests();
}
	
void runTests(){
	// ---------------------------------------------
  // Status LED Tests
  // ---------------------------------------------
  static uint32_t testNumber = 1;

  switch(testNumber){
    case 1: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Create a Status\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        RED_SensorLED.createStatus("Solid Red", SensorLED_Red);
        BLUE_SensorLED.createStatus("Solid Red", SensorLED_Red);
        RED_SensorLED.setStatus("Solid Red", true);
        BLUE_SensorLED.setStatus("Solid Red", true);
        Serial.println("LED should be SOLID RED");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 1000){
          Serial.println("Test complete");
          runTest = true;
          testNumber++;
        }
      }
      break;
    }

    case 2: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Priorities\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        RED_SensorLED.createStatus("Solid Green", SensorLED_Green, SensorLED_Priority_Med);
        BLUE_SensorLED.createStatus("Solid Green", SensorLED_Green, SensorLED_Priority_Med);
        RED_SensorLED.setStatus("Solid Green", true);
        BLUE_SensorLED.setStatus("Solid Green", true);
        Serial.println("LED Should be SOLID GREEN");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 1000){
          Serial.println("Test complete");
          RED_SensorLED.setStatus("Solid Red", false);
          BLUE_SensorLED.setStatus("Solid Red", false);
          RED_SensorLED.setStatus("Solid Green", false);
          BLUE_SensorLED.setStatus("Solid Green", false);
          runTest = true;
          testNumber++;
        }
      }
      break;
    }

    case 3: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Blink\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        RED_SensorLED.createStatus("Blink Yellow", SensorLED_Yellow, 500);
        BLUE_SensorLED.createStatus("Blink Blue", SensorLED_Blue, 750);
        RED_SensorLED.setStatus("Blink Yellow", true);
        BLUE_SensorLED.setStatus("Blink Blue", true);
        Serial.println("LEDs should be YELLOW and BLUE and BLINKING");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 5000){
          Serial.println("Test complete");
          runTest = true;
          testNumber++;
        }
      }
      break;
    }

    case 4: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Turn Off\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        RED_SensorLED.setStatus("Blink Yellow", false);
        BLUE_SensorLED.setStatus("Blink Blue", false);
        Serial.println("LEDs should be OFF");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 1000){
          Serial.println("Test complete");
          runTest = true;
          testNumber++;
        }
      }
      break;
    }

    case 5: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Blink X Times\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        RED_SensorLED.createStatus("Blink Red 3", SensorLED_Red, SensorLED_Priority_Med, 500, 3);
        BLUE_SensorLED.createStatus("Blink Green 3", SensorLED_Green, SensorLED_Priority_Med, 500, 4);
        RED_SensorLED.createStatus("Blink Blue 2", SensorLED_Blue, SensorLED_Priority_Med, 250, 2);
        RED_SensorLED.setStatus("Blink Red 3", true);
        BLUE_SensorLED.setStatus("Blink Green 3", true);
        RED_SensorLED.setStatus("Blink Blue 2", true);
        Serial.println("LEDs should blink RED 3 TIMES and GREEN 4 TIMES and then BLUE 2 TIMES");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 10000){
          Serial.println("Test complete");
          runTest = true;
          testNumber++;
        }
      }
      break;
    }

    case 6: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Display Now\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
        RED_SensorLED.setPriority("Blink Yellow", SensorLED_Priority_High);
        BLUE_SensorLED.setPriority("Blink Blue", SensorLED_Priority_High);
        RED_SensorLED.createStatus("Fast Blink Magenta", SensorLED_Magenta, 100);
        BLUE_SensorLED.createStatus("Fast Blink Cyan", SensorLED_Cyan, 100);
        RED_SensorLED.displayNow("Fast Blink Magenta");
        BLUE_SensorLED.displayNow("Fast Blink Cyan");
        Serial.println("LEDs should be MAGENTA and CYAN FAST BLINKING");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 5000){
          Serial.println("Test complete");
          RED_SensorLED.setStatus("Blink Yellow", false);
          RED_SensorLED.setStatus("Blink Blue", false);
          runTest = true;
          testNumber++;
        }
      }
      break;
    }

    default:{
      static bool AllTestsComplete = false;
      if(!AllTestsComplete){
        Serial.println("\nAll Test Complete");
        AllTestsComplete = true;
        System.dfu();
      }
      break;
    }
  }
}