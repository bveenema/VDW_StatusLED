#include "VDW_StatusLED.h"

#define RED_PIN D0
#define GREEN_PIN D1
#define BLUE_PIN D2

// Native Particle pin constructor
VDW_StatusLEDTarget LEDTarget(RED_PIN, GREEN_PIN, BLUE_PIN);

// Alternative IO Expansion constructor
	// writeFunc must be a function of the form --> void writeFunc(uint8_t pin, bool dir)
	// VDW_StatusLED LEDTarget(writeFunc, RED_PIN, GREEN_PIN, BLUE_PIN);
	// // Example "writeFunc"
	// void writeFunc(uint8_t pin, bool dir){
	// 	IOEXP.writePin(pin, dir);
	// }

// Create Statuses for the LEDTarget
VDW_LEDStatus solidRedLowPriority(&LEDTarget, StatusLED_Color_Red); // constructor for solid red LED with default priority (low)
VDW_LEDStatus solidGreenMedPriority(&LEDTarget, StatusLED_Color_Green);
VDW_LEDStatus solidCyanHighPriority(&LEDTarget, StatusLED_Color_Cyan, StatusLED_Priority_High); // constructor for solid Cyan LED, high priority
VDW_LEDStatus solidWhiteLowPriority(&LEDTarget, StatusLED_Color_White);

VDW_LEDStatus blinkingYellow(&LEDTarget, StatusLED_Color_Yellow, StatusLED_Pattern_Blink, StatusLED_Speed_Medium); // constructor for medium speed blinking green LED, default priority
VDW_LEDStatus blinkingBlue(&LEDTarget, StatusLED_Color_Blue, StatusLED_Pattern_Blink, StatusLED_Speed_Fast);

VDW_LEDStatus customBlinkBlueLowPriority(&LEDTarget, StatusLED_Color_Blue, StatusLED_Pattern_Blink, 50); // constructor for 50ms on, 50ms off (100ms period 50% duty) blinking blue LED, default priority

VDW_LEDStatus blinkRed3TimesMedPriority(&LEDTarget, StatusLED_Color_Red, StatusLED_Priority_Med, StatusLED_Pattern_Blink, StatusLED_Speed_Medium, 3);
VDW_LEDStatus blinkBlue2TimesLowPriority(&LEDTarget, StatusLED_Color_Blue, StatusLED_Priority_Low, StatusLED_Pattern_Blink, StatusLED_Speed_Medium, 2);

VDW_LEDStatus blinkMagentaLowPriority(&LEDTarget, StatusLED_Color_Magenta, StatusLED_Priority_Low, StatusLED_Pattern_Blink, StatusLED_Speed_Fast); // constructor for 10 fast blinks, Cyan, critical priority that will self-deactive after 10 blinks, to allow other, lower priority statuses to dislplay 

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
				solidRedLowPriority.setStatus(Active);
        Serial.println("LED should be SOLID RED");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 1000){
					solidRedLowPriority.setStatus(Disabled);
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
				solidRedLowPriority.setStatus(Active);
				solidGreenMedPriority.setStatus(Active);
        Serial.println("LED Should be SOLID GREEN");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 1000){
          Serial.println("Test complete");
					solidRedLowPriority.setStatus(Disabled);
					solidGreenMedPriority.setStatus(Disabled);
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
				blinkingYellow.setStatus(Active);
        Serial.println("LED should be YELLOW and BLINKING");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 5000){
					blinkingYellow.setStatus(Disabled);
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
				blinkRed3TimesMedPriority.setStatus(Active);
				blinkBlue2TimesLowPriority.setStatus(Active);
        Serial.println("LED should blink RED 3 TIMES and then BLUE 2 TIMES");
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
				solidCyanHighPriority.setStatus(Active);
				blinkMagentaLowPriority.displayNow();
        Serial.println("LED should be MAGENTA FAST BLINKING");
        testTimer = millis();
        setupTest = true;
      }
      if(!runTest) {
        if(millis() - testTimer > 5000){
          Serial.println("Test complete");
          solidCyanHighPriority.setStatus(Disabled);
					blinkMagentaLowPriority.setStatus(Disabled);
          runTest = true;
          testNumber++;
        }
      }
      break;
    }

		case 7: {
      static bool runTest = false;
      static bool printTestTitle = false;
      static bool setupTest = false;
      static uint32_t testTimer = millis();
      if(!printTestTitle){
        Serial.printlnf("\n****************\nTest %d: Recover from Display Now\n****************\n",testNumber);
        printTestTitle = true;
      }
      if(!setupTest) {
				solidWhiteLowPriority.setStatus(Active);
        Serial.println("LED should be SOLID WHITE");
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

    default:{
      static bool AllTestsComplete = false;
      if(!AllTestsComplete){
        Serial.println("\nAll Test Complete");
        AllTestsComplete = true;
        Particle.connect();
      }
      break;
    }
  }
}