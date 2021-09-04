#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>


// A few colour codes:
 
// code	color
// 0x0000	Black
// 0xFFFF	White
// 0xBDF7	Light Gray
// 0x7BEF	Dark Gray
// 0xF800	Red
// 0xFFE0	Yellow
// 0xFBE0	Orange
// 0x79E0	Brown
// 0x7E0	Green
// 0x7FF	Cyan
// 0x1F	Blue
// 0xF81F	Pink

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
uint32_t targetTime = 0;       // for next 1 second timeout

uint8_t SPIN_SENSOR_PIN = PIN_D2;
uint8_t spinSensorStatus = LOW; 
uint8_t curSpinSensorStatus = LOW; 
uint16_t cirCount = 0;
uint16_t distance = 0;
float kcl = 0;
float speed = 0;
float avgSpeed = 0;

void setup() {
  pinMode(SPIN_SENSOR_PIN, INPUT);
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  targetTime = millis() + 1000;
}

void loop() {

  // laps count
  curSpinSensorStatus = digitalRead(SPIN_SENSOR_PIN);
  if (curSpinSensorStatus != spinSensorStatus) {
    if (curSpinSensorStatus == HIGH){
      cirCount++;
      distance += 1;
      kcl += 0.01;
      speed = 1/(millis()-targetTime);
    }
  }
  spinSensorStatus = curSpinSensorStatus;


  if (targetTime + 1000 < millis()) {
    // ============假装每秒更新一次数据==========
    distance += 1;
    kcl += 0.01;
    speed = 3.2;
    // ============假装每秒更新一次数据==========
    tft.fillScreen(TFT_BLACK);
    targetTime = millis()+1000;
    
    byte xpos = 6;
    byte ypos = 0;
    tft.setTextColor(TFT_GREEN, TFT_BLACK); // green font with black back
    tft.setCursor (8, 52);
    // tft.print(__DATE__); // This uses the standard ADAFruit small font

    tft.setTextColor(TFT_GREEN); // Orange 
    xpos+= tft.drawNumber(distance,xpos,ypos,7); // Draw distance
    xpos+= tft.drawString("KM",xpos,6,2); // Draw distance

    // xpos += tft.drawChar(':', xpos, ypos - 8, 8);

    tft.setTextFont(2);
    tft.print(kcl);
    tft.println(" KCL");
    tft.println("DIST:3080");

  }
}