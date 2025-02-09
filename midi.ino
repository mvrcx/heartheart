/*******************************************************************************

  Bare Conductive Touch USB MIDI instrument
  -----------------------------------------

  Midi_interface.ino - USB MIDI touch instrument

  Remember to select Bare Conductive Touch Board (USB MIDI, iPad compatible)
  in the Tools -> Board menu

  Bare Conductive code written by Stefan Dzisiewski-Smith, Peter Krige, Pascal
  Loose and Szymon Kaliski.

  This work is licensed under a MIT license https://opensource.org/licenses/MIT

  Copyright (c) 2016, Bare Conductive

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*******************************************************************************/

#include "Compiler_Errors.h"

// touch includes
#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <Wire.h>

// touch constants
const uint32_t BAUD_RATE = 115200;
const uint8_t MPR121_ADDR = 0x5C;
const uint8_t MPR121_INT = 4;

// Lichtsensor-Schwellenwerte
const int calibrationThreshold = 580;  // Sobald der Sensorwert unter diesen Wert fällt, gilt: Buchdeckel geschlossen -> Kalibrieren
const int coverOpenThreshold   = 150;    // Wenn der Sensorwert über 150 liegt, ist der Buchdeckel offen
boolean wasOpen = false;

// MPR121 Datastream behaviour
const bool MPR121_DATASTREAM_ENABLE = false;

// MIDI behaviour constants
const bool SWITCH_OFF = false;  // false = Note nur so lange, wie berührt wird
const uint8_t NOTES[] = {59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48};  // Noten C3 bis B3
const uint8_t CHANNEL = 0;
bool note_status[12] = {false, false, false, false, false, false, false, false, false, false, false, false};

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(LED_BUILTIN, OUTPUT);

  if (!MPR121.begin(MPR121_ADDR)) {
    Serial.println("error setting up MPR121");
    switch (MPR121.getError()) {
      case NO_ERROR:
        Serial.println("no error");
        break;
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;
    }
    while (1);
  }

  MPR121.setInterruptPin(MPR121_INT);

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121.restoreSavedThresholds();
    MPR121_Datastream.begin(&Serial);
  } else {
    MPR121.setTouchThreshold(40);
    MPR121.setReleaseThreshold(20);
  }

  MPR121.setFFI(FFI_10);
  MPR121.setSFI(SFI_10);
  MPR121.setGlobalCDT(CDT_4US);  // für größere Kapazitäten

  // Während der Kalibrierung leuchtet die LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  MPR121.autoSetElectrodes();
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // Lichtsensor auslesen
  int sense = analogRead(A0);
  Serial.println(sense);

  // Kalibrierung: Wenn der Sensorwert über 'calibrationThreshold' liegt, merken wir uns, dass der Deckel offen ist.
  // Sobald der Wert unter 'calibrationThreshold' fällt (also vermutlich nach dem Schließen des Deckels),
  // wird die Kalibrierung durchgeführt.
  if (sense > calibrationThreshold) {
    wasOpen = true;
  } else {
    if (wasOpen) {
      Serial.println("calibrate!!!!!");
      delay(1000);
      MPR121.autoSetElectrodes();
      wasOpen = false;
    }
  }

  // Update der Touch-Elektroden
  MPR121.updateAll();

  // Nur Touch-Events verarbeiten, wenn der Buchdeckel geschlossen ist
  if (sense <= coverOpenThreshold) {
    for (int i = 0; i < 12; i++) {
      if (MPR121.isNewTouch(i)) {
        // Beim neuen Touch wird die LED eingeschaltet und
        // je nach Modi (toggle oder nicht) wird eine Note an- bzw. ausgeschaltet
        digitalWrite(LED_BUILTIN, HIGH);
        if (!SWITCH_OFF) {
          noteOn(CHANNEL, NOTES[i], 127);
        } else {
          if (note_status[i]) {
            noteOff(CHANNEL, NOTES[i], 127);
          } else {
            noteOn(CHANNEL, NOTES[i], 127);
          }
          note_status[i] = !note_status[i];
        }
      } else if (MPR121.isNewRelease(i)) {
        digitalWrite(LED_BUILTIN, LOW);
        if (!SWITCH_OFF) {
          noteOff(CHANNEL, NOTES[i], 127);
        }
      }
    }
  }
  
  // Alle MIDI-Daten sofort senden
  MIDIUSB.flush();

  if (MPR121_DATASTREAM_ENABLE) {
    MPR121_Datastream.update();
  }
  
  delay(10);  // kleine Pause, damit der USB MIDI-Receiver Zeit hat
}

void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDIUSB.write({0x09, 0x90 | (channel & 0x0F), pitch, velocity});
}

void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MIDIUSB.write({0x08, 0x80 | (channel & 0x0F), pitch, velocity});
}
