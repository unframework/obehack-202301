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
const int buttonLowTriggerCount = 15;
bool pressedState = false;

void initButton() {
  pinMode(BUTTON_SENSE, INPUT);

  // set up the keep-up reference pin
  digitalWrite(BUTTON_REF, HIGH); // write value before enabling output
  pinMode(BUTTON_REF, OUTPUT);
}

struct buttonState_t buttonState = {
    false, // pressed
    false, // released
};

void updateBaseButton() {
  int buttonPinState = digitalRead(BUTTON_SENSE);

  bool newPressed = pressedState;
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

  buttonState.pressed = false;
  buttonState.released = false;

  if (newPressed != pressedState) {
    pressedState = newPressed;

    if (pressedState) {
      buttonState.pressed = true;
    } else {
      buttonState.released = true;
    }
  }
}

unsigned long longPressStartTime = 0;
bool longPressedState = false;

struct buttonState_t longPressState = {
    false, // pressed
    false, // released
};

void updateLongPressed() {
  if (buttonState.pressed) {
    longPressStartTime = millis();
  } else if (buttonState.released) {
    longPressStartTime = 0;
  }

  bool newLongPressed = longPressedState;
  if (longPressStartTime) {
    newLongPressed =
        newLongPressed || ((millis() - longPressStartTime) >= 3000);
  } else {
    newLongPressed = false;
  }

  if (newLongPressed != longPressedState) {
    longPressedState = newLongPressed;

    if (longPressedState) {
      longPressState.pressed = true;
    } else {
      longPressState.released = true;
    }
  }
}

void updateButton() {
  updateBaseButton();
  updateLongPressed();
}