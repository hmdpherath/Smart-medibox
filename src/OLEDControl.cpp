#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed if the display isn't connected
  }
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0, 0);     
  display.println("Medibox Status:");
  display.display();
}

void loop() {
  // Update display with some dynamic data (for example, temperature or time)
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Temperature: 25C");
  display.println("Humidity: 60%");
  display.display();
  delay(2000); // Update every 2 seconds
}
