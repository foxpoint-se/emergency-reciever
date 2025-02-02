#include <SPI.h>
#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String contents = "";
int nof_new_lines = 6;

int FRAME_RATE_MS = 2000;
long TIME_NOW = 0;
long LAST_FRAME_UPDATE = 0;

void setup() {
  Serial.begin(9600);

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);                     // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.clearDisplay();
  display.setCursor(0,0); 

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
}

void loop() {
  int index = 1;
  String work_string = "";
  String display_output = "";
  String timestamp = "";
  String state = "-1";
  String longitude = "";
  String latitude = "";
  String percent_bat_lvl = "";

  TIME_NOW = millis();

  // First check if there are any new incoming messages, if so print special msg and restart loop
  int packet_size = LoRa.parsePacket();
  if (packet_size) {
    work_string = getIncomingMessage();

    // An incoming message is expected to contain 12 lines, loop over them and create frames
    for(int i=1; i < (nof_new_lines + 1); i++){
      
      // Parse the string and create a tmp row to be displayed
      index = work_string.indexOf("\n");
      if (index >= 0) {
        switch(i)
        {
          case 1:
              timestamp = work_string.substring(0, index);
              break;

          case 2:
              state = work_string.substring(0, index);
              break;

          case 3:
              longitude = work_string.substring(0, index);
              break;

          case 4:
              latitude = work_string.substring(0, index);
              break;

          case 5:
              percent_bat_lvl = work_string.substring(0, index);
              break;

          // operator doesn't match any case constant +, -, *, /
          default:
              Serial.println(" ");
        }
        
        work_string = work_string.substring(index + 1);
        
      }else {
        break;
      }
  
    }

  }

  bool valid_state = state != "-1";

  if (((TIME_NOW - LAST_FRAME_UPDATE) > FRAME_RATE_MS) && valid_state) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("T:");
    display.println(timestamp);
    display.print("Lon:");
    display.println(longitude);
    display.print("Lat:");
    display.println(latitude);
    display.print("Bat%:");
    display.print(percent_bat_lvl);
    display.print("    State:");
    display.println(state);
    display.display();

    // Also display data on the serial monitor
    Serial.print("Timestamp: ");
    Serial.println(timestamp);
    Serial.print("Longitude: ");
    Serial.println(longitude);
    Serial.print("Latitude: ");
    Serial.println(latitude);
    Serial.print("Battery percentage: ");
    Serial.print(percent_bat_lvl);
    Serial.println("%");
    Serial.print("State: ");
    Serial.println(state);
    Serial.println();

    state = "-1";
  }
}

String getIncomingMessage(){
  String tmp_content = "";

  while (LoRa.available()) {
    tmp_content += (char)LoRa.read();
  }

  return tmp_content;
}
