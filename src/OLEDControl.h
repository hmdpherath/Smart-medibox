#include "BuzzerControl.h"
#include <Arduino.h>

// Global variable for the buzzer pin
int buzzerPin;

// Function to initialize the buzzer pin
void setupBuzzer(int pin) {
  buzzerPin = pin;
  pinMode(buzzerPin, OUTPUT);
}

// Function to activate the buzzer
void activateBuzzer() {
  digitalWrite(buzzerPin, HIGH);
}

// Function to deactivate the buzzer
void deactivateBuzzer() {
  digitalWrite(buzzerPin, LOW);
}
