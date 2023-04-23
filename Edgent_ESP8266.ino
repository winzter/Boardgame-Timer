/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
 *************************************************************
  Blynk.Edgent implements:
  - Blynk.Inject - Dynamic WiFi credentials provisioning
  - Blynk.Air    - Over The Air firmware updates
  - Device state indication using a physical LED
  - Credentials reset using a physical Button
 *************************************************************/

/* Fill in information from your Blynk Template here */
/* Read more: https://bit.ly/BlynkInject */
//#define BLYNK_TEMPLATE_ID           "TMPxxxxxx"
//#define BLYNK_TEMPLATE_NAME         "Device"
#define BLYNK_TEMPLATE_ID "TMPLbaQz8_wK"
#define BLYNK_TEMPLATE_NAME "BoardGame"
#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd_1(0x27, 16, 2);

const int start = D0;
const int settime = D4;
const int buzz = 13;
const int ledGreen = 14;
const int ledRed = 15;
const int pirPin = 12;


int state=0;
int count = 0;
long seconds = 40;
long minutes = 0;
long hours = 0;
long countdown_time = (hours * 3600) + (minutes * 60) + seconds;

int countPIR = 0;
long secondsPIR = 20;
long minutesPIR = 0;
long hoursPIR = 0;
long countdown_timePIR = (hoursPIR * 3600) + (minutesPIR * 60) + secondsPIR;

unsigned long StartTimer = 0;
unsigned long PirTimer = 0;

int value;

BLYNK_WRITE(V2)
{
  value = param.asInt(); // Get value as integer
  Serial.print("value: ");
  Serial.println(value);
  // The param can contain multiple values, in such case:
  if(value==1){
    //reset
    reset(1);
  }


}

void setTime(){
  lcd_1.clear();
  lcd_1.setCursor(4, 0);
 count +=3600;
  
  lcd_1.print(count/3600);
  lcd_1.print(" Hour");
  delay(1000);
  lcd_1.clear();
  Serial.println("Time set");
}


void time(int count){
  Serial.println("time");
    digitalWrite(ledGreen,LOW);
    //digitalWrite(ledRed,HIGH);
    delay(100);
    unsigned long elapsed = millis()-StartTimer;
    unsigned long elapsedPIR = millis()-PirTimer;
    unsigned long countdowntime_seconds = (countdown_time+count) - elapsed/1000;

    unsigned long countdowntime_secondsPIR = (countdown_timePIR) - elapsedPIR/1000;
    Serial.print("PIR:");
    Serial.print(countdowntime_secondsPIR);

    if(digitalRead(pirPin)==HIGH){
      PirTimer = millis();
      Serial.println("DETECT");
    }
    else if(countdowntime_secondsPIR==0){
      Serial.println("No Human");
      PirTimer = 0;
      state=0;
    }
   if (countdowntime_seconds >= 0){
      long countdown_hour = countdowntime_seconds / 3600;
      long countdown_minute = ((countdowntime_seconds / 60) % 60);
      long countdown_sec = countdowntime_seconds % 60;
      lcd_1.setCursor(4, 0);
      if (countdown_hour < 10) {
        lcd_1.print("0");
      }
      lcd_1.print(countdown_hour);
      lcd_1.print(":");
      if (countdown_minute < 10) {
        lcd_1.print("0");
      }
      lcd_1.print(countdown_minute);
      lcd_1.print(":");
      if (countdown_sec < 10) {
        lcd_1.print("0");
      }
      lcd_1.print(countdown_sec);
  // Serial.print("count : ");
  // Serial.println(countdowntime_seconds);

      if (countdowntime_seconds == 0) {
        delay(1000);
        // lcd_1.setCursor(4, 1);
        // tone(buzz,600,3000);
        // delay(500000);
        // lcd_1.print("Time Out!");
        // Serial.println("END");
        // lcd_1.clear();
        // lcd_1.print("Available");
        state=0;
        return;
        
      }
    delay(500);
   }
   delay(500);
}
void reset(int cheak){
  StartTimer = 0;
  state=0;
  count=0;
  lcd_1.clear();
  tone(buzz,600);
      delay(2000);
      noTone(buzz);
  if(cheak==0){
    lcd_1.setCursor(4, 0);
      lcd_1.print("Time Out!");
      Serial.println("Sound");
      digitalWrite(ledRed,HIGH);
      
      digitalWrite(ledRed,LOW);
      
  }
      
        Serial.println("END");
        delay(5000);
        lcd_1.clear();
        lcd_1.setCursor(4, 0);
        lcd_1.print("Available");
         Blynk.virtualWrite(V2,1);
        digitalWrite(ledGreen,HIGH);
}
void setup() {
  pinMode(buzz,OUTPUT);
  pinMode(ledGreen,OUTPUT);
  pinMode(ledRed,OUTPUT);
  pinMode(start,INPUT);
  pinMode(settime,INPUT);
  pinMode(pirPin,INPUT);

  digitalWrite(ledGreen,HIGH);
  // digitalWrite(ledRed,HIGH);
  Serial.begin (115200);
  lcd_1.begin();
  lcd_1.display();        //เปิดหน้าจอ
  lcd_1.backlight();      //เปิดไฟ backlight 
  lcd_1.clear();          //ล้างหน้าจอ
  delay(100);

  BlynkEdgent.begin();
}

void loop() {
  BlynkEdgent.run();
  float h = digitalRead(pirPin);
  Blynk.virtualWrite(V1,h);
  // Serial.print("ledRed: ");
  // Serial.println(digitalRead(ledRed));
  //Serial.println(state);
  
  if(digitalRead(settime)==LOW){
    setTime();
   }
  else if(digitalRead(start)==HIGH){
    Blynk.virtualWrite(V2,0);
    Serial.println("Yes");
    lcd_1.clear();
    state=1;

  }

  if(state==1){
    if(StartTimer == 0 || PirTimer == 0){
      StartTimer = millis();
      PirTimer = millis();

    }

  
    
   time(count);
   if(state == 0){
      reset(0);
   }
  }
 
}
