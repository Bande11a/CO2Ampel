/*

This program was developed by Malte Gehrmann. It is CO2 trafic light powered by an ESP32. It notifys people in the room
when they need to open their windows to let in fresh air by changing an LED from Green to yellow to red.
It will also display the current time and current CO2 value on a 4 digit 7 segment display

Any questions email: malte.gehrmann@honeywell.com

*/


//=======================================================
//Importing all libaries for the project
#include <Arduino.h> // This is included by default if you use the arduino IDE

#include "MHZ19.h"
#include "FastLED.h"
#include <TM1637Display.h>
//=======================================================


//=======================================================
//Setting up the FastLED libaray
#define DATA_PIN 26
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
#define CLK 33
#define DIO 25
TM1637Display display(CLK, DIO);
//=======================================================

//=======================================================
// Input Pin
#define Switch_pin 21
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

    pinMode(Switch_pin, INPUT);                                     //defining the switch
    
    mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);           //Setting up the CO2 sensor        
    myMHZ19.begin(mySerial);                             
    myMHZ19.autoCalibration();

    FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);         //Setting up the FASTLED libaray  
}
//=======================================================


//=======================================================
void loop()
{
    buttonState = digitalRead(Switch_pin);

    if(!buttonState){
        if(DayMode){
            DayMode = false;
            leds[0] = CRGB(0,0,0);
            FastLED.show();
            display.clear();
            Serial.println("Turning Off");
        }
        else{
            DayMode = true;
            Colour = 0;
            Serial.println("Turning On");
        }
        delay(500);
    }


    if (millis() - getDataTimer >= 2000 && DayMode)    
    {
        int CO2;                                // Creates CO2 variable and reads the current CO2 value
        CO2 = myMHZ19.getCO2();
        display.setBrightness(0x0f);
        display.showNumberDecEx(CO2, 0x00, false, 4, 0);

        if(CO2 <= 750){
            if(Colour != 1){
                leds[0] = CRGB(0,255,0);        //if the value of the co2 is less than 800 the LED will be set to green
                FastLED.show();
                Colour = 1;
            }                         
        }
        else if(CO2 <= 1250){
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
        getDataTimer = millis();              // Update inerval
    }
}
//=======================================================
