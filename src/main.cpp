#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <car.h>
#include <nice.h>
#include <ESP8266WiFiMulti.h>
#include <wifi_icon.h>

ESP8266WiFiMulti wifiMulti;
const uint32_t connectTimeoutMs = 5000;

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
uint8_t targetTime = 10;       // 目标时间 分钟
uint32_t curTime = 0;        // curTime
uint32_t lastTime = 0;       // for next 1 second timeout

uint8_t BTN1_PIN = PIN_D1;
uint8_t SPIN_SENSOR_PIN = PIN_D2;
uint8_t spinSensorStatus = LOW; 
uint8_t curSpinSensorStatus = LOW; 
uint16_t cirCount = 0;
uint16_t distance = 0;
float kcl = 0;
float speed = 0;
float avgSpeed = 0;
bool inited = false;
bool finished = false;
float pStat[7200]; 


// logo显示
void dispLogo(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2);
  tft.pushImage(tft.width()/2-32,tft.height()/2-32,64,64, (uint16_t  *)gImage_bik);
  tft.drawString("ZXQ's Fit:", tft.width()/2, tft.height(), 2); 
}

void setup() {
  pinMode(SPIN_SENSOR_PIN, INPUT);
  pinMode(BTN1_PIN, INPUT);
  randomSeed(analogRead(PIN_D6));
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  dispLogo();
  // gImage_wifi_off
  tft.pushImage(tft.width()-20, 0, 20, 20, (uint16_t  *)gImage_wifi_off);
  // Register multi WiFi networks
  wifiMulti.addAP("Satelite", "admingyu123");
  wifiMulti.addAP("BHU", "admingyu123");


  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
}

// 设定运动时长,飞轮转一圈自动开始
void setTarget(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2);
  tft.drawString("SET TARGET:",30,0,2);
  while (digitalRead(SPIN_SENSOR_PIN) == LOW){
    if (digitalRead(BTN1_PIN) == HIGH) { // 按一下加一分钟
      byte xpos = 30;
      byte ypos = 60;
      targetTime += 1;
      tft.setTextColor(TFT_CYAN); // Cyan 
      tft.fillRect(xpos, ypos, tft.width()-30, tft.height()/2, TFT_BLACK);
      xpos+= tft.drawNumber(targetTime,xpos,ypos,7); // Draw distance
      tft.drawString(" Min",xpos,38,2); // Draw distance
    }
    delay(500);
  }
  tft.fillScreen(TFT_BLACK);
}

// 打印成绩
void dispFinish(){
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(0, 0, 64, 64, (uint16_t  *)gImage_nice);
  tft.setTextFont(2);
  char kcls[10], times[10], dists[10];
  char kcl_str[6];   //浮点类型转化存储缓冲数组
  dtostrf(kcl, 4,2, kcl_str);  // 4,2 表示转化的精度
  sprintf(kcls,"KCL: %s",kcl_str);
  sprintf(times,"TIME:%dmin", targetTime);
  sprintf(dists,"DIST: %dm", distance);
  tft.drawString(kcls ,tft.width()/2, 0, 2); 
  tft.drawString(times,tft.width()/2, 21, 2); 
  tft.drawString(dists,tft.width()/2, 42, 2); 
  tft.drawFastHLine(0, tft.height()/2, tft.width(), TFT_SKYBLUE);

  // 统计图
  unsigned char i;
  for (i=0; i<7200;i++){
    tft.drawPixel(i*int(tft.width()/7200), pStat[i]*20, TFT_NAVY);
  }
  while(1){
   delay(2000);
  }
}


// 打印模板
void initExec(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2);
  tft.drawString("KCL:",0,64,2); 
  tft.drawString("POWER:",0,84,2); 
  // tft.drawString("POWER:",0,104,2); 
  tft.drawLine(0,60,tft.width(),60,TFT_SKYBLUE);//上下分割线
  tft.drawRect(0,104,tft.width()-1, tft.height()-104,TFT_CYAN); //进度条
  lastTime = millis() + 1000;
  inited = true;
}


void loop() {

  // WIFI连接
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
    // gImage_wifi_on
    tft.pushImage(tft.width()-20, 0, 20, 20, (uint16_t  *)gImage_wifi_on);
  } else {
    // gImage_wifi_off
    tft.pushImage(tft.width()-20, 0, 20, 20, (uint16_t  *)gImage_wifi_off);
  }

  // 是否初始化完成
  if (!inited){
    setTarget();
    initExec();
  }

  // 是否完成锻炼
  if (finished){
    dispFinish();
    delay(100);
    return;
  }

  // laps count
  curSpinSensorStatus = digitalRead(SPIN_SENSOR_PIN);
  if (curSpinSensorStatus != spinSensorStatus) {
    if (curSpinSensorStatus == HIGH){
      cirCount++;
      distance += 1;
      kcl += 0.01;
      
    }
    // tft.println(curSpinSensorStatus);
  }
  spinSensorStatus = curSpinSensorStatus;

  curTime =  millis();
  if (lastTime + 1000 < curTime) {
    // ============假装每秒更新一次数据==========
    distance += cirCount;
    kcl += cirCount*0.01;
    // speed = 1000.00/(curTime-lastTime);
    speed = cirCount*12;
    pStat[int(curTime/1000)] = speed;
    // ============假装每秒更新一次数据==========

    // 屏幕上半区域覆盖黑色
    tft.fillRect(0,0,tft.width(),59,TFT_BLACK);

    
    byte xpos = 6;
    byte ypos = 0;
    tft.setTextColor(TFT_GREEN, TFT_BLACK); // green font with black back
    tft.setCursor (8, 52);
    // tft.print(__DATE__); // This uses the standard ADAFruit small font

    tft.setTextColor(TFT_GREEN); // Orange 
    xpos+= tft.drawNumber(distance,xpos,ypos,7); // Draw distance
    xpos+= tft.drawString(" M",xpos,38,2); // Draw distance

    // xpos += tft.drawChar(':', xpos, ypos - 8, 8);

    //里程速度区域覆盖黑色
    tft.fillRect(50, tft.height()/2, 50, tft.height()-88, TFT_BLACK);

    // 里程速度显示
    tft.setTextFont(2);
    tft.drawFloat(kcl,1,50,64); 
    tft.drawFloat(speed,1,50,84); 
    // tft.drawFloat(speed,1,50,104);

    // 进度
    float progress ;
    progress = (curTime*1.000)/(1000*60*targetTime);

    // 统计图显示
    tft.drawFastVLine(100 +int(progress*64.00), (114-int(speed)), int(speed), TFT_SKYBLUE);

    // 进度条显示
    tft.fillRect(1, 105, tft.width(), tft.height()-106, TFT_BLACK); //进度条黑底
    tft.fillRect(1, 105, int(tft.width()*progress), tft.height()-106, TFT_NAVY); //进度条
    tft.setTextColor(TFT_RED); // TFT_OLIVE 
    
    tft.drawFloat(progress*100, 1, 50, 110,2); //进度条百分比
    tft.drawString("%", 80, 110); //进度条百分比

    lastTime = curTime;
    cirCount = 0;

    if (progress >= 1.00){
      finished = true;
    }
  }

  // 设定初始化完成
  if (inited ==false){
    inited=true;
  }
}