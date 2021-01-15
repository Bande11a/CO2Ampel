/*

This program was developed by Malte Gehrmann. It is CO2 trafic light powered by an ESP32. It notifys people in the room
when they need to open their windows to let in fresh air by changing an LED from Green to yellow to red.
It will also display the current time and current CO2 value on a 4 digit 7 segment display

Any questions email: malte.gehrmann@honeywell.com

*/


//=======================================================
//Importing all libaries for the project
#include <Arduino.h>
#include "MHZ19.h"
#include "FastLED.h"
#include <TM1637Display.h>
#include "RTClib.h"
//=======================================================


//=======================================================
//Setting up the FastLED libaray
#define DATA_PIN 25
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
//=======================================================


//=======================================================
//Setting up the libaray for the CO2 Sensor
#define RX_PIN 9
#define TX_PIN 10
#define BAUDRATE 9600               // Native to the sensor (do not change)
MHZ19 myMHZ19;
HardwareSerial mySerial(0);
//=======================================================


//=======================================================
//Setting up the 4 digit display
#define CLK 18
#define DIO 19
TM1637Display display(CLK, DIO);
//=======================================================


//=======================================================
//Setup for the RTC
RTC_DS3231 rtc;

//=======================================================


//=======================================================
//Defining any variables needed for the loop and setup functions
unsigned long getDataTimer = 0;
unsigned long ClockDataTimer = 0;
bool ShowTime = false;
int ShowTimeCount = 0;
bool blink = true;
byte Colour;
int buttonState;
unsigned long ButtonCoolDown;
bool DayMode = true;

//=======================================================
void setup()
{
    Serial.begin(9600);


    pinMode(32, INPUT);
    
    

    mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);                                 // Uno example: Begin Stream with MHZ19 baudrate
    myMHZ19.begin(mySerial);                                // *Important, Pass your Stream reference
    myMHZ19.autoCalibration();

    FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);

     if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        abort();
    }
    // DateTime compile = DateTime(F(__DATE__), F(__TIME__));
    // rtc.adjust(DateTime(compile.unixtime() - 12)); //Uncomment this to adjust the time and timezone

    if(rtc.lostPower()) {
        // this will adjust to the date and time at compilation
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  
}
//=======================================================


//=======================================================
void loop()
{
    buttonState = digitalRead(32);

    if(!buttonState){
        if(DayMode){
            DayMode = false;
            leds[0] = CRGB(0,0,0);
            FastLED.show();
            display.clear();
        }
        else{
            DayMode = true;
            Colour = 0;
        }
        buttonState = millis();
        delay(500);
        while(!digitalRead(32)){
            if(millis() - buttonState > 10000){
                unsigned int Forwardstime = rtc.now().unixtime();
                DateTime AdjsutForwardsTime = DateTime(Forwardstime + 3600);
                rtc.adjust(AdjsutForwardsTime);
                int Forwardshour = AdjsutForwardsTime.hour();
                int Forwardsmin = AdjsutForwardsTime.minute();
                int ForwardsDisplayTime = Forwardshour * 100 + Forwardsmin;
                display.setBrightness(0x0f);
                display.showNumberDecEx(ForwardsDisplayTime, 0x40, false, 4, 0);
                while(!digitalRead(32)){
                    if(millis() - buttonState > 15000){
                        unsigned int Backwardstime = rtc.now().unixtime();
                        DateTime AdjsutBackwardsTime = DateTime(Backwardstime -7200);
                        rtc.adjust(AdjsutBackwardsTime);
                        int Backwardshour = AdjsutBackwardsTime.hour();
                        int Backwardsmin = AdjsutBackwardsTime.minute();
                        int BackwardsDisplayTime = Backwardshour * 100 + Backwardsmin;
                        display.setBrightness(0x0f);
                        display.showNumberDecEx(BackwardsDisplayTime, 0x40, false, 4, 0);
                        break;
                    }
                    
                }
                break;
                
            }
        }
    }


    if (millis() - getDataTimer >= 2000 && DayMode)    
    {
        int CO2;                                // Creates CO2 variable and reads the current CO2 value
        CO2 = myMHZ19.getCO2();

        if(CO2 <= 800){
            if(Colour != 1){
                leds[0] = CRGB(0,255,0);        //if the value of the co2 is less than 800 the LED will be set to green
                FastLED.show();
                Colour = 1;
            }                         
            
        }
        else if(CO2 <= 1000){
            if(Colour != 2){
                leds[0] = CRGB(255,255,0);        //if the value of the co2 is between 600 and 800 the LED will be set to yellow
                FastLED.show();
                Colour = 2;         
            }                    
            
        }
        else{
            if(Colour != 3){
                leds[0] = CRGB(255,0,0);            //if the value of the co2 is more than 800 the LED will turn to red
                FastLED.show();
                Colour = 3;
            }

        }

        if(!ShowTime){
            display.setBrightness(0x0f);            //Updates the value showen on the display
            display.showNumberDecEx(CO2, 0x00, false, 4, 0);
            if(ShowTimeCount >= 15){
                ShowTimeCount = 0;
                ShowTime = true;
            }
            else{
                ShowTimeCount++;
            }
        }
    
        getDataTimer = millis();              // Update inerval

        
    }

    if(millis() - ClockDataTimer >= 500 && ShowTime && DayMode){
        DateTime time = rtc.now();

        int hour = time.hour();
        int min = time.minute();
        int DisplayTime = hour * 100 + min;
        if(blink){
            display.setBrightness(0x0f); 
            display.showNumberDecEx(DisplayTime, 0x40, false, 4, 0);
            blink = false;
        }
        else{
            display.setBrightness(0x0f); 
            display.showNumberDecEx(DisplayTime, 0x00, false, 4, 0);
            blink = true;
        }
        if(ShowTimeCount >= 60){
            ShowTimeCount = 0;
            ShowTime = false;
        }
        else{
            ShowTimeCount++;
        }

        ClockDataTimer = millis();
    }
}
//=======================================================
