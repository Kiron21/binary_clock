/*
An open-source binary clock for Arduino. 
Based on the code from by Rob Faludi (http://www.faludi.com) and Daniel Spillere Andrade, www.danielandrade.net
Writed by Rostislav Stemon (Kiron21)
http://creativecommons.org/license/cc-gpl
*/


#include <JC_Button.h>
#include "SoftPWM.h"

//#define RTC_MODULE
// Uncomment if you have the RTC module.

#ifdef RTC_MODULE
  #include <ThreeWire.h>  
  #include <RtcDS1302.h>
  ThreeWire myWire(A3,A2,A4); // DAT/IO, SCLK, RST/CE
  RtcDS1302<ThreeWire> Rtc(myWire);
  RtcDateTime now;
#endif

Button btMinutes(A0);  // Minutes button pin
Button btHours(A1);    // Hours button pin
const byte pr_Pin=A5;
const int pr_Limit=1000;
const uint8_t DAY=255; // Brightness for the day mode.
const uint8_t NIGHT=10; // Brightness for the night mode.
const uint8_t DARK=0; // Brightness for OFF LED - you can use very low brightness for off LEDs.

uint8_t brightness=DAY;  
boolean newSecond=true;
uint8_t second=0, minute=53, hour=15; //start the time on 00:00:00
int munit,hunit,valm=0,valh=0,ledstats,i;
boolean error=false;  // Just for debuging - set true insight code you want.

void setup() { 
  SoftPWMBegin();
  //set outputs 
  for(int k=1;k<=13;k++) {
    SoftPWMSet(k,0);
  }
  //set inputs
  pinMode(A5, INPUT);
  btMinutes.begin();
  btHours.begin();

#ifdef RTC_MODULE
  Rtc.Begin();
  //RTC.writeEN(true);
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid()) {
    // Common Causes: 1) first time you ran and the device wasn't running yet 2) the battery on the device is low or even missing
    Rtc.SetDateTime(compiled);
    RtcDateTime now = Rtc.GetDateTime();
  }
  if (Rtc.GetIsWriteProtected())
  {
    Rtc.SetIsWriteProtected(false);
  }
  if (!Rtc.GetIsRunning())
  {
      Rtc.SetIsRunning(true);
  }
  if (!Rtc.GetIsRunning()) error = true;
#endif
}

void loop() {

  static unsigned long lastTick = 0; // set up a local variable to hold the last time we moved forward one second
// (static variables are initialized once and keep their values between function calls)
// move forward one second every 1000 milliseconds

  btMinutes.read();
  btHours.read();

  #ifdef RTC_MODULE 
  //  **** RTC installed *****
    now = Rtc.GetDateTime();
    if (now.Second() == second) newSecond=false;
    second=now.Second();
    minute=now.Second();
    hour=now.Hour();
  //  **** RTC installed *****
  #endif

  #ifndef RTC_MODULE
  //   **** Without RTC   *****
  if (millis() - lastTick >= 1000) {
    lastTick = millis();
    second++;
    newSecond=true;
  } 

  // move forward one minute every 60 seconds
  if (second >= 60) {
    minute++;
    second = 0; // reset seconds to zero
  }

  // move forward one hour every 60 minutes
  if (minute >=60) {
    hour++;
    minute = 0; // reset minutes to zero
  }

  if (hour >=24) {
    hour  = 0;
    minute  = 0; // reset minutes to zero
  }
  //   **** Without RTC   *****
  #endif

  munit = minute%10; //sets the variable munit and hunit for the unit digits
  hunit = hour%10;

  if (newSecond) {
    if (analogRead(pr_Pin) > pr_Limit) brightness=NIGHT; else brightness=DAY;
  
    //minutes units
    if(munit == 1 || munit == 3 || munit == 5 || munit == 7 || munit == 9) {  SoftPWMSet(1, brightness);} else {  SoftPWMSet(1,DARK);}
    if(munit == 2 || munit == 3 || munit == 6 || munit == 7) {SoftPWMSet(2, brightness);} else {SoftPWMSet(2,DARK);}
    if(munit == 4 || munit == 5 || munit == 6 || munit == 7) {SoftPWMSet(3, brightness);} else {SoftPWMSet(3,DARK);}
    if(munit == 8 || munit == 9) {SoftPWMSet(4, brightness);} else {SoftPWMSet(4,DARK);}
    //minutes 
    if((minute >= 10 && minute < 20) || (minute >= 30 && minute < 40) || (minute >= 50 && minute < 60))  {SoftPWMSet(5, brightness);} else {SoftPWMSet(5,DARK);}
    if(minute >= 20 && minute < 40)  {SoftPWMSet(6, brightness);} else {SoftPWMSet(6,DARK);}
    if(minute >= 40 && minute < 60) {SoftPWMSet(7, brightness);} else {SoftPWMSet(7,DARK);}
    //hour units
    if(hunit == 1 || hunit == 3 || hunit == 5 || hunit == 7 || hunit == 9) {SoftPWMSet(8, brightness);} else {SoftPWMSet(8,DARK);}
    if(hunit == 2 || hunit == 3 || hunit == 6 || hunit == 7) {SoftPWMSet(9, brightness);} else {SoftPWMSet(9,DARK);}
    if(hunit == 4 || hunit == 5 || hunit == 6 || hunit == 7) {SoftPWMSet(10, brightness);} else {SoftPWMSet(10,DARK);}
    if(hunit == 8 || hunit == 9) {SoftPWMSet(11, brightness);} else {SoftPWMSet(11,DARK);}
    //hour
    if(hour >= 10 && hour < 20)  {SoftPWMSet(12, brightness);} else {SoftPWMSet(12,DARK);}
    if(hour >= 20 && hour < 24)  {SoftPWMSet(13, brightness);} else {SoftPWMSet(13,DARK);}
    newSecond=false;
    }

    if (error) SoftPWMSet(13, 255); //Example for using error state.

  if (btMinutes.wasPressed()) {
    minute++;
    second = 0;
    #ifdef RTC_MODULE
      now.Minute=minute;
      Rtc.SetDateTime(now)
    #endif
    newSecond=true;
    }
  
  if (btHours.wasPressed()) {
    hour++;
    second = 0;
    #ifdef RTC_MODULE
      now.Minute=minute;
      Rtc.SetDateTime(now)
    #endif   
    newSecond=true;
   }
}
