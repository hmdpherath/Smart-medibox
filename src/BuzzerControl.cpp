// Define the pin for the buzzer
#define BUZZER_PIN 13

void setup() {
  // Set the buzzer pin as an output
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Turn the buzzer on
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000); // Wait for 1 second
  
  // Turn the buzzer off
  digitalWrite(BUZZER_PIN, LOW);
  delay(1000); // Wait for 1 second
}
