//This usermod is still in testing, use it with care!


#include "wled.h"
#ifndef WLED_ENABLE_MQTT
#error "The user mod mqtt_button_input requires MQTT to be enabled."
#endif

//Config:

#define buttonPin 1
#define enableInternalPullup true //enable or disable the internal pullup resisitor
#define defaultPinState  LOW //defines the state of the pin when the button isn't pressed

#define mqttTopic "test"
#define mqttPayload "huhu"


class UsermodMqttSwitch: public Usermod
{
private:
    bool prevState = defaultPinState;

public:
    void setup()
    {
        #if enableInternalPullup == true 
            pinMode(buttonPin, INPUT_PULLUP);
        #else
            pinMode(buttonPin, INPUT);
        #endif
    }

    void loop()
    {
        bool inputState=digitalRead(buttonPin);

        if(inputState!=defaultPinState && prevState==defaultPinState)//detect begin of button Press
        {
            publishMqtt(mqttPayload);
        }
        prevState=inputState;
    }

    void publishMqtt(String payload) //function from Usermod "PIR_sensor_mqtt_v1" from calebmah
    {
        //Check if MQTT Connected, otherwise it will crash the 8266
        if (mqtt != nullptr){
            char subuf[38];
            strcpy(subuf, mqttDeviceTopic);
            strcat(subuf, mqttTopic);
            mqtt->publish(subuf, 0, true, payload.c_str());
        }
    }

};
