/*******************************************************************************

  Bare Conductive Touch USB MIDI Instrument
  -----------------------------------------

  Midi_interface.ino - USB MIDI touch instrument

  This sketch turns the Bare Conductive Touch Board into a MIDI instrument, 
  allowing capacitive touch inputs to trigger MIDI notes.

  Ensure you have selected "Bare Conductive Touch Board (USB MIDI, iPad compatible)" 
  in the Tools -> Board menu before uploading.

  Written by Stefan Dzisiewski-Smith, Peter Krige, Pascal Loose, and Szymon Kaliski.
  Licensed under the MIT License (https://opensource.org/licenses/MIT)

*******************************************************************************/

#include "Compiler_Errors.h"

// Include necessary libraries for capacitive touch and I2C communication
#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <Wire.h>

// Serial communication baud rate
const uint32_t BAUD_RATE = 115200;

// MPR121 capacitive touch sensor configuration
const uint8_t MPR121_ADDR = 0x5C; // I2C address of the MPR121 sensor
const uint8_t MPR121_INT = 4;     // Interrupt pin for MPR121

// Light sensor thresholds for detecting book cover state
const int calibrationThreshold = 580;  // Threshold to trigger calibration when cover closes
const int coverOpenThreshold   = 150;  // Threshold to detect open cover
boolean wasOpen = false;               // Track if the cover was last open

// Enable or disable MPR121 data streaming for debugging
const bool MPR121_DATASTREAM_ENABLE = false;

// MIDI behavior configuration
const bool SWITCH_OFF = false;  // false = Note only plays while touched
const uint8_t NOTES[] = {59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48}; // MIDI notes (C3 to B3)
const uint8_t CHANNEL = 0; // MIDI channel
bool note_status[12] = {false}; // Track active notes

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize MPR121 touch sensor
  if (!MPR121.begin(MPR121_ADDR)) {
    Serial.println("Error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR: Serial.println("No error"); break;
      case ADDRESS_UNKNOWN: Serial.println("Incorrect address"); break;
      case READBACK_FAIL: Serial.println("Readback failure"); break;
      case OVERCURRENT_FLAG: Serial.println("Overcurrent on REXT pin"); break;
      case OUT_OF_RANGE: Serial.println("Electrode out of range"); break;
      case NOT_INITED: Serial.println("Not initialized"); break;
      default: Serial.println("Unknown error"); break;
    }
    while (1); // Halt execution if initialization fails
  }

  // Set MPR121 interrupt pin
  MPR121.setInterruptPin(MPR121_INT);

  // Configure touch detection thresholds
  if (MPR121_DATASTREAM_ENABLE) {
    MPR121.restoreSavedThresholds();
    MPR121_Datastream.begin(&Serial);
  } else {
    MPR121.setTouchThreshold(40);
    MPR121.setReleaseThreshold(20);
  }

  // Adjust settings for capacitive sensitivity
  MPR121.setFFI(FFI_10);
  MPR121.setSFI(SFI_10);
  MPR121.setGlobalCDT(CDT_4US);

  // Indicate calibration process with LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  MPR121.autoSetElectrodes();
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // Read light sensor value
  int sense = analogRead(A0);
  Serial.println(sense);

  // Perform calibration when cover closes
  if (sense > calibrationThreshold) {
    wasOpen = true;
  } else if (wasOpen) {
    Serial.println("Calibrating...");
    delay(1000);
    MPR121.autoSetElectrodes();
    wasOpen = false;
  }

  // Update touch sensor readings
  MPR121.updateAll();

  // Only process touch input when cover is closed
  if (sense <= coverOpenThreshold) {
    for (int i = 0; i < 12; i++) {
      if (MPR121.isNewTouch(i)) {
        digitalWrite(LED_BUILTIN, HIGH); // Turn on LED when touched

        // Play note depending on mode
        if (!SWITCH_OFF) {
          noteOn(CHANNEL, NOTES[i], 127);
        } else {
          note_status[i] ? noteOff(CHANNEL, NOTES[i], 127) : noteOn(CHANNEL, NOTES[i], 127);
          note_status[i] = !note_status[i];
        }
      } else if (MPR121.isNewRelease(i)) {
        digitalWrite(LED_BUILTIN, LOW); // Turn off LED when released
        if (!SWITCH_OFF) {
          noteOff(CHANNEL, NOTES[i], 127);
        }
      }
    }
  }
  
  // Flush MIDI messages
  MIDIUSB.flush();

  // Update data stream if enabled
  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }
  
  delay(10); // Short delay for stability
}

// Send a MIDI note-on message
void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDIUSB.write({0x09, 0x90 | (channel & 0x0F), pitch, velocity});
}

// Send a MIDI note-off message
void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDIUSB.write({0x08, 0x80 | (channel & 0x0F), pitch, velocity});
}
