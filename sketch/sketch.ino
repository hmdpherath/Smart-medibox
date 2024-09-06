// Include libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h> 


// Define OLED parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Button and DHT sensor pin definitions
#define BUZZER 5
#define LED_1 15
#define LED_2 2
#define PB_CANCEL 34
#define PB_OK 32
#define PB_UP 33
#define PB_DOWN 35
#define DHTPIN 12

// NTP server details for time synchronization
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     19800        // UTC+5:30
#define UTC_OFFSET_DST 0



// Declare objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHTesp dhtSensor;

// Global variables for storing date and time
int year =0;
int month =0;
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

// variables to track cuttenr time and last time
unsigned long timeNow = 0;
unsigned long timeLast = 0;

bool alarm_enabled = true;
int n_alarms = 3;
int alarm_hours[] = {0,1,2};
int alarm_minutes[] = {1,10,15};
bool alarm_triggered[] = {false, false,false};

//defining musical notes for buzzer
int n_notes = 8;
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int notes[] = {C,D,E,F,G,A,B,C_H};


// current option of the menu
int current_mode = 0;
int max_modes = 5;//maximum num of option in menu
String modes[] = {"1.Set Time ", " 2 . Set  Alarm 1", " 3 . Set  Alarm 2", " 4 . Set  Alarm 3","5 .Disable Alarms"};// we have 5 options

// values for different time zones.
// num of elements in UTC_offsets=38 and indexes go until 37
float UTC_offsets[] = {-12, -11, -10, -9.5, -9, -8, -7, -6, -5, -4, -3.5, -3, -2, -1, 0, 1, 2, 3, 3.5, 4, 4.5, 5, 5.5, 5.75, 6, 6.5, 7, 8, 8.75, 9, 9.5, 10, 10.5, 11, 12, 12.75, 13, 14};

void setup() {  
  // Setup button and LED pin modes
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(PB_CANCEL, INPUT);
  pinMode(PB_OK, INPUT);
  pinMode(PB_UP, INPUT);
  pinMode(PB_DOWN, INPUT);

  dhtSensor.setup(DHTPIN, DHTesp::DHT22);

  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(500);

  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WIFI", 2, 0, 0);
  }

  display.clearDisplay();
  print_line("Connected to WIFI", 2, 0, 0);
  // Configure NTP time synchronization
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

  display.clearDisplay();
  print_line("Welcome", 3, 0, 0);
  print_line("to Medibox!", 2, 0, 40);
  delay(2000);
  display.clearDisplay(); 
}
void loop() {
  update_time();
  // Update time and check for alarms
  update_time_with_check_alarm();
  if (digitalRead(PB_OK) == LOW){
    delay(200);
    go_to_menu();
  }
  check_temp();
}
// print a line in given text size and position
void print_line(String text, int text_size, int column, int row){
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(text);

  display.display();
}
//display current time DD:HH:MM:SS

void print_time_now(void){
  display.clearDisplay();
  String date = String(days)+"/" + String(month) + "/" + String(year);
  print_line(date,2, 20,0);
  //print_line(":",2, 20, 0);
  print_line(String(hours),2, 30,30);
  print_line(":",2, 50, 30);
  print_line(String(minutes),2, 60,30);
  print_line(":",2, 80,30);
  print_line(String(seconds), 2,90,30); 
}


//update time using NTP over WiFi
void update_time(){
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  hours = atoi(timeHour);

  char timeMinute[3];
  strftime(timeMinute,3, "%M", &timeinfo);
  minutes = atoi(timeMinute);

  char timeSecond[3];
  strftime(timeSecond,3, "%S", &timeinfo);
  seconds = atoi(timeSecond);

  char timeYear[5];
  strftime(timeYear,5, "%Y", &timeinfo);
  year= atoi(timeYear);

  char timeMonth[3];
  strftime(timeMonth,3, "%m %d %Y", &timeinfo);
  month = atoi(timeMonth);

  char timeDay[3];
  strftime(timeDay,3, "%d", &timeinfo);
  days = atoi(timeDay);
}


//ring the alarm
void ring_alarm(){
  // update the time
  display.clearDisplay();
  print_line("MEDICINE  TIME!", 2, 0, 0);

  digitalWrite(LED_1,HIGH);

  bool break_happened = false;

  //Ring the buzzer
  while (break_happened == false && digitalRead(PB_CANCEL) == HIGH){
    for (int i=0; i < n_notes; i++){
      if (digitalRead(PB_CANCEL) == LOW){
        delay(200);
        break_happened = true;
        break;
      }
      tone(BUZZER, notes[i]);
      delay(500);
      noTone(BUZZER);
      delay(2);
    }
  }
  delay(200);
  digitalWrite(LED_1, LOW);
  display.clearDisplay();
}

// update time and check for alarm triggers
void update_time_with_check_alarm(void){
  // update time over WiFi
  update_time();
  // Display current time
  print_time_now();

   // Check whether alarms are enabled
  if (alarm_enabled == true){
    for (int i=0; i<n_alarms; i++){
      if (alarm_triggered[i]==false && alarm_hours[i] == hours && alarm_minutes[i] == minutes){
        ring_alarm();
        alarm_triggered[i] = true;
      }
    }
  }
}

//wait for a button press and return the pressed button
int wait_for_button_press(){
  
  while(true){
    if(digitalRead(PB_UP) == LOW){
      delay(200);
      return PB_UP;
    }
    if(digitalRead(PB_DOWN) == LOW){
      delay(200);
      return PB_DOWN;
    }
    if(digitalRead(PB_OK) == LOW){
      delay(200);
      return PB_OK;
    }
    if(digitalRead(PB_CANCEL) == LOW){
      delay(200);
      return PB_CANCEL;
    }
    update_time();
  }
}

//go to the menu and handle menu interactions
void go_to_menu(){
  while (digitalRead(PB_CANCEL) == HIGH){
    display.clearDisplay();
    print_line(modes[current_mode], 2, 0, 0);
    int pressed = wait_for_button_press();
    if (pressed == PB_UP){
      delay(200);
      current_mode +=1;
      current_mode = current_mode % max_modes;
      
    }

    else if (pressed == PB_DOWN){
      delay(200);
      current_mode -=1;
      if (current_mode < 0){
        current_mode = max_modes - 1;
      }
      
    }
    else if (pressed == PB_OK){
      delay(200);
      // Run the selected mode 
      run_mode(current_mode);
    }

    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }
}



// Set the time based on user-selected UTC offset
void set_time() {
  int temp_UTC_OFFSET_index = 14;

  while (true) {
    display.clearDisplay();
    print_line("Enter UTC offset:" + String(UTC_offsets[temp_UTC_OFFSET_index]), 0, 0, 2);
  
    int pressed = wait_for_button_press();
    delay(200);

    switch (pressed) {
      case PB_UP:
        temp_UTC_OFFSET_index = (temp_UTC_OFFSET_index + 1) % 38;
        break;
      
      case PB_DOWN:
        temp_UTC_OFFSET_index = (temp_UTC_OFFSET_index - 1 + 38) % 38;
        break;

      case PB_OK:
        configTime(3600 * UTC_offsets[temp_UTC_OFFSET_index], UTC_OFFSET_DST, NTP_SERVER);
        return;

      case PB_CANCEL:
        return;
    }
  }
}


//set the alarm time
void set_alarm(int alarm){

  int temp_hour = alarm_hours[alarm];
  while (true) {
    display.clearDisplay();
    print_line("Enter hour:" + String(temp_hour), 2, 0, 0);
    // Wait for button press
    int pressed = wait_for_button_press();
    // Handle the buttons 
    if (pressed == PB_UP){
      delay(200);
      temp_hour +=1;
      temp_hour = temp_hour % 24;
    }

    else if (pressed == PB_DOWN){
      delay(200);
      temp_hour -=1;
      if (temp_hour < 0){
        temp_hour = 23;
      }
    }
    else if (pressed == PB_OK){
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    }

    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }  
  // set the minutes
  int temp_minute = alarm_minutes[alarm];
  while (true) {
    display.clearDisplay();
    print_line("Enter the minutes:" + String(temp_minute), 2, 0, 0);
  
    int pressed = wait_for_button_press();
    if (pressed == PB_UP){
      delay(200);
      temp_minute +=1;
      temp_minute = temp_minute % 60;
    }

    else if (pressed == PB_DOWN){
      delay(200);
      temp_minute -=1;
      if (temp_minute < 0){ 
        temp_minute = 59;
      }
    }
    else if (pressed == PB_OK){
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      alarm_enabled = true;
      break;
    }

    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }  
  display.clearDisplay();
  print_line("The Time  of Alarm is set", 2, 0, 0);
  delay(1000);
}
// Function to execute different modes based on the menu selection
void run_mode(int mode){
  if (mode == 0){
    // set system time
    set_time();
  }

  else if (mode == 1 || mode == 2 || mode ==3){
    // set the alarm time based on menu selection
    alarm_enabled = true ;
    // Set alarm time according to menu selection
    set_alarm(mode - 1);
  }

  else if (mode == 4){
    // Toggle alarm enable/disable
    alarm_enabled = !alarm_enabled;
    display.clearDisplay();
    
  if (alarm_enabled) {
    print_line("ENABLED", 2, 40, 25);
  } else {
    print_line("DISABLED", 2, 40, 25);
  }
    delay(2000);
  }
}


void check_temp(void){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  bool all_good = true;
  // Display temperature and humidity messages 
  if (data.temperature > 32){
    all_good = false;
    digitalWrite(LED_2, HIGH);
    print_line("TEMP HIGH", 1,40, 50);
  }
  else if (data.temperature < 26){
    all_good = false;
    digitalWrite(LED_2, HIGH);
    print_line("TEMP LOW", 1,40, 50);
  }
  if (data.humidity > 80){
    all_good = false;
    digitalWrite(LED_2, HIGH);
    print_line("HUMD HIGH", 1, 50, 60);
  }
  else if (data.humidity < 60){
    all_good = false;
    digitalWrite(LED_2, HIGH);
    print_line("HUMD LOW", 1, 50, 60);
  }
  if (all_good == true){
    digitalWrite(LED_2, LOW);
  }
}


 

