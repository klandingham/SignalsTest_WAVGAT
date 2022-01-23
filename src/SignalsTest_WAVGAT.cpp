/*

  Signals_Test (WAVGAT version)

  This is a modified version of Blinds_Control_NodeMCU.cpp - modified to
  continually produce stepper rotation signals. Intended to be used for
  troubleshooting.

  Controls 28BYJ-48 stepper motors via shift registers to
  open/close window blinds. See history.txt

*/

// #define TRACE

#include <Arduino.h>

// pin assignments
// NodeMCU
//const int latchPin = D2;  // to pin 12 on the 75HC595
//const int clockPin = D5;  // to pin 11 on the 75HC595
//const int dataPin  = D7;  // to pin 14 on the 75HC595
// WAVGAT
const int latchPin = 4;   // to pin 12 on the 75HC595
const int clockPin = 2;   // to pin 11 on the 75HC595
const int dataPin  = 3;   // to pin 14 on the 75HC595
const int resetPin = 5;   // for testing only

const int fullStepValueCount = 4;
const int stepDelay = 250; // milliseconds
// values for two-phase, full-step operation (AB->BC->CD->DA)
// examples:
//
// single motor:
//  1100 (12)
//  0110 (6)
//  0011 (3)
//  1001 (9)
// two motors:
//  11001100 (204)
//  01100110 (102)
//  00110011 (51)
//  10011001 (153)
int TF_stepperValues1[4]   = {  12,     6,    3,    9};
int TF_stepperValues2[4]   = { 192,    96,   48,  144};
int TF_stepperValues3[4]   = {3072,  1536,  768, 2304};
int TF_stepperValues12[4]  = { 204,   102,   51,  153};
int TF_stepperValues123[4] = {3276,  1638,  819, 2457};
int stepIndex;

//////////////////////////////////////////////////////////////////////////////
// Sends next bit pattern to motor(s). Moves value index according to specified direction.
void SendNextSequence(boolean opening) {
    // Note: using a global for sequence (step) index so I can start it at 0 in setup()
    static uint16_t serialValue;
    // moving this delay to the main loop!  delay(10);  // delay for stepper motor...may need adjustment in final version
    // Note: will need to replace next statement with computation: add to final value
    // for each enabled motor; right now just driving motor 1
    serialValue = TF_stepperValues123[stepIndex];  // get next value (bit pattern) to output
    digitalWrite(latchPin, LOW);  // ready shift register for data
    delay(5);
    shiftOut(dataPin, clockPin, MSBFIRST, serialValue); // send value to shift register
    digitalWrite(latchPin, HIGH);  // latch shift register (sends bits to motor(s))
    delay(5);
    if (opening) {
        stepIndex++;
        if (stepIndex >= fullStepValueCount) {
        	stepIndex = 0;
        }
    }
    else {
        stepIndex--;
        if (stepIndex < 0) {
        	stepIndex = fullStepValueCount - 1;
        }
    }
} // SendNextSequence()

// Send 10101010 to shift register, latch, delay, then clear
void SendBitPattern() {
	int clockDelay = 10; // milliseconds
	uint8_t bit = 0;
	uint8_t bitToggle = 0;
	digitalWrite(latchPin, LOW);
	digitalWrite(clockPin, LOW);
	for (int i = 0; i < 8; i++) {
		digitalWrite(resetPin, HIGH);
		if (++bitToggle > 2) {
			bit = !bit;
			bitToggle = 0;
		}
		delay(clockDelay);
		digitalWrite(dataPin, bit);
		delay(clockDelay);
		digitalWrite(clockPin, HIGH);
		delay(clockDelay);
		digitalWrite(clockPin, LOW);
		digitalWrite(latchPin, HIGH);
		delay(clockDelay * 8);
		digitalWrite(resetPin, LOW);
		digitalWrite(latchPin, LOW);
	}
}
//////////////////////////////////////////////////////////////////////////////
void setup() {

    Serial.begin(9600);

    while (!Serial)  // Wait for the serial connection to be established.
      delay(50);

    pinMode(dataPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);

    stepIndex = 0;
} // setup()

//////////////////////////////////////////////////////////////////////////////
void loop() {
    // Simply send the next sequence in the OPEN direction
    SendNextSequence(true);
    // delay(1);
    // Note: If stepper driver lights come on, but motor does not turn, try adding a delay here.
    // first, test outputs to 74595
	// SendBitPattern();
} // loop()

