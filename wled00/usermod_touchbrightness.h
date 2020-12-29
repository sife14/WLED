//
//  usermod_touchbrightness.h
//  github.com/aircoookie/WLED
//
//  Created by Justin Kühner on 14.09.2020.
//  Copyright © 2020 NeariX. All rights reserved.
//  https://github.com/NeariX67/changeBrightness
//  Discord: @NeariX#4799

//  Modification for ESP8266 from S. M.
//  Oktober 2020
//  https://github.com/sife14
//  Idea of software implemented touch from: https://www.mikrocontroller.net/topic/25045
//  Note: The software touch function on the ESP8266 is very noisy, therefor a lot of samples are needed.
//  As per the MIT license, I assume no liability for any damage to you or any other person or equipment. This usermod is still under testing!
#pragma once
#include "wled.h"

//Settings for ESP8266
#define changeBrightness false  //wether the brightness should be changable via touch, default false to avoid false detection
float thresholdFactor=1.05;     //threshold=average of last 500 Values * thresholdFactor
                                //increase if Wled turns detects touch when not touched
                                //decrease if Wled doesn't recognize touch

#define touchPinESP8266 12      //Pin used for touch sensor
#define sampleCount     20      //number of samples for one touch messurement
#define minHighSamples  1       //define the minimum count of samples above the threshold to be detected as touch

//Settings for ESP32
#define thresholdESP32 1
#define touchPinESP32 TO

//Define the 5 brightness levels
//Long press to turn off / on
#define brightness1 51
#define brightness2 102
#define brightness3 153
#define brightness4 204
#define brightness5 255


#ifdef ESP8266 

#define touchPin touchPinESP8266                    

class TouchBrightnessControl : public Usermod {
  private:
    unsigned long lastTime = 0;         //Interval
    unsigned long lastTouch = 0;        //Timestamp of last Touch
    unsigned long lastRelease = 0;      //Timestamp of last Touch release
    boolean released = true;            //current Touch state (touched/released)
    unsigned int touchReading = 0;      //sensor reading
    uint16_t touchDuration = 0;         //duration of last touch
    unsigned int messurementCounter=0;      //counts the taken samples for the average
    unsigned int messurementSum=0;          //sum of all samples

    int lastAverageTimestamp;

    unsigned int completeDischargeCounter;
    float threshold;                    
    float average;
           

  public:

    unsigned int touchRead(int touchPinLocal){  //function to replace the fuction "touchRead" from the ESP32
      int t = 0;                                //stores the time needed to charge the sensor surface
      pinMode(touchPinLocal, OUTPUT);
      digitalWrite(touchPinLocal, LOW);
      pinMode(touchPinLocal, INPUT_PULLUP);       //start charging the sensor surface

      completeDischargeCounter++;

      while (digitalRead(touchPinLocal) == 0){    //wait until the sensor surface is completely charged
        t++;
        if(t>1000){                               //limits the maximum time to prevent the watchdog from triggering
          Serial.println("timed out");
          break;
        }
      }

      pinMode(touchPinLocal, OUTPUT);
      digitalWrite(touchPinLocal, LOW);
      
      return t;
    }

    void setup() {
      unsigned long sum=0;

      for(int x=0; x<200; x++){          //messure initial value for average
        sum+=touchRead(touchPin);
        delay(10);
      }

      average=(sum/200.0);
      threshold=average*thresholdFactor;

      lastTouch = millis();
      lastRelease = millis();
      lastTime = millis();
    }

    void loop() {

      if(completeDischargeCounter>3000)//experimental
      {
          completeDischargeCounter=0;
          pinMode(touchPinESP8266, OUTPUT);
          digitalWrite(touchPinESP8266, LOW);
          delay(8);
      }


      else if (millis()-lastTime >= 5 && messurementCounter<sampleCount){  //take samples
        messurementSum=messurementSum+touchRead(touchPin);
        messurementCounter++;
        lastTime=millis();
        //Serial.println(threshold);
      }
      

      else if (messurementCounter>=sampleCount){                        //process values if samples are taken
        touchReading=messurementSum/sampleCount;
        messurementSum=0;
        messurementCounter=0;

        if (touchReading<threshold)                                     //only update the average if not touched
        {
          average=(((average*80.0)+touchReading)/81.0);                 //update the average
          threshold=average*thresholdFactor;                            //calculate the threshold from the average
        }


        Serial.print(touchReading);
        Serial.print("  ");
        Serial.print(average);
        Serial.print("  ");
        Serial.println(threshold);
        

        if(touchReading > threshold && released) {               //Touch started
          released = false;
          lastTouch = millis();
        }
        else if(touchReading <= threshold && !released ) {       //Touch released
          released = true;
          lastRelease = millis();
          touchDuration = lastRelease - lastTouch;               //Calculate duration
        }

        
        #if changeBrightness==false
          
          if(touchDuration >= minHighSamples*sampleCount*5 && released) {          //Toggle lamp
            touchDuration = 0;                                     
            toggleOnOff();
            colorUpdated(NOTIFIER_CALL_MODE_DIRECT_CHANGE);
          }
          
        

        #elif changeBrightness==true                               //not tested! use with care!

          if(touchDuration >= 1000 && released) {                   //Toggle power if button press is longer than 800ms
            touchDuration = 0;                                     //Reset touch duration to avoid multiple actions on same touch
            toggleOnOff();
            colorUpdated(NOTIFIER_CALL_MODE_DIRECT_CHANGE);
          }
          else if(touchDuration >= 10 && released) {               //Switch to next brightness if touch is between 100 and 800ms
            touchDuration = 0;                                     //Reset touch duration to avoid multiple actions on same touch
            if(bri < brightness1) {
              bri = brightness1;
            } else if(bri >= brightness1 && bri < brightness2) {
              bri = brightness2;
            } else if(bri >= brightness2 && bri < brightness3) {
              bri = brightness3;
            } else if(bri >= brightness3 && bri < brightness4) {
              bri = brightness4;
            } else if(bri >= brightness4 && bri < brightness5) {
              bri = brightness5;
            } else if(bri >= brightness5) {
              bri = brightness1;
            }                                    
            colorUpdated(NOTIFIER_CALL_MODE_DIRECT_CHANGE);       //Refresh values
          } 
        #else
          #error Please enter true or false for changeBrightness
        #endif
      }

    }

    void addToJsonInfo(JsonObject& root)
    {
      int reading = 20;
      //this code adds "u":{"Light":[20," lux"]} to the info object
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");
      JsonArray lightArr = user.createNestedArray("Touch"); //name
      lightArr.add(reading); //value
      lightArr.add(" lux"); //unitd
    }
};
#endif


#ifdef ESP32
#define touchPin touchPinESP32                    

class TouchBrightnessControl : public Usermod {
  private:
    unsigned long lastTime = 0;         //Interval
    unsigned long lastTouch = 0;        //Timestamp of last Touch
    unsigned long lastRelease = 0;      //Timestamp of last Touch release
    boolean released = true;            //current Touch state (touched/released)
    uint16_t touchReading = 0;          //sensor reading, maybe use uint8_t???
    uint16_t touchDuration = 0;         //duration of last touch
  public:
  
    void setup() {
      lastTouch = millis();
      lastRelease = millis();
      lastTime = millis();
    }

    void loop() {
      if (millis() - lastTime >= 50) {                           //Check every 50ms if a touch occurs
        lastTime = millis();
        touchReading = touchRead(touchPin);                      //Read touch sensor on pin T0 (GPIO4 / D4)
        
        if(touchReading < thresholdESP32 && released) {               //Touch started
          released = false;
          lastTouch = millis();
        }
        else if(touchReading >= thresholdESP32 && !released) {        //Touch released
          released = true;
          lastRelease = millis();
          touchDuration = lastRelease - lastTouch;               //Calculate duration
        }
        
        //Serial.println(touchDuration);

        if(touchDuration >= 800 && released) {                   //Toggle power if button press is longer than 800ms
          touchDuration = 0;                                     //Reset touch duration to avoid multiple actions on same touch
          toggleOnOff();
          colorUpdated(2);                                       //Refresh values
        }
        else if(touchDuration >= 100 && released) {              //Switch to next brightness if touch is between 100 and 800ms
          touchDuration = 0;                                     //Reset touch duration to avoid multiple actions on same touch
          if(bri < brightness1) {
            bri = brightness1;
          } else if(bri >= brightness1 && bri < brightness2) {
            bri = brightness2;
          } else if(bri >= brightness2 && bri < brightness3) {
            bri = brightness3;
          } else if(bri >= brightness3 && bri < brightness4) {
            bri = brightness4;
          } else if(bri >= brightness4 && bri < brightness5) {
            bri = brightness5;
          } else if(bri >= brightness5) {
            bri = brightness1;
          }
          colorUpdated(2);                                       //Refresh values
        }
        
      }
    }
  
};

#endif
