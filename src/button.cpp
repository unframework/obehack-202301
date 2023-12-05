#include <Arduino.h>

#include "button.h"

#if ARDUINO_ESP8266_NODEMCU_ESP12E

// ESP8266 NodeMCU ESP-12E pinout
#define BUTTON_SENSE 12 // the number of the pushbutton pin
#define BUTTON_REF 14   // the number of the reference pin

#else

#error unknown board pinout

#endif

int buttonLowCount = 0;
const int buttonLowTriggerCount = 5; // 10ms
bool pressed = false;
unsigned long buttonStartMillis = 0;
bool longPressed = false;

const unsigned long buttonLongPressMillis = 3000;

void initButton() {
  pinMode(BUTTON_SENSE, INPUT);

  // set up the keep-up reference pin
  digitalWrite(BUTTON_REF, HIGH); // write value before enabling output
  pinMode(BUTTON_REF, OUTPUT);
}

struct buttonState_t buttonState = {false};

void updateButton() {
  int buttonPinState = digitalRead(BUTTON_SENSE);

  bool newPressed = pressed;
  if (buttonPinState == LOW) {
    buttonLowCount = max(buttonLowTriggerCount, buttonLowCount + 1);
    if (buttonLowCount >= buttonLowTriggerCount) {
      newPressed = true;
    }
  } else {
    buttonLowCount = min(0, buttonLowCount - 1);
    if (buttonLowCount == 0) {
      newPressed = false;
    }
  }

  if (newPressed != pressed) {
    pressed = newPressed;

    // @todo report early-up event
    buttonState.longPressedJustNow = true;
  } else {
    buttonState.longPressedJustNow = false;
  }

  // report long-press event (as a one-time thing)
  // const unsigned long curTime = millis();
  // const bool newLongPressed =
  //     buttonStartMillis ? (curTime - buttonStartMillis) >=
  //     buttonLongPressMillis
  //                       : false;

  // buttonState.longPressedJustNow = false;
  // if (!longPressed != newLongPressed) {
  //   longPressed = newLongPressed;
  //   if (longPressed) {
  //     buttonState.longPressedJustNow = true;
  //   }
  // }
}
