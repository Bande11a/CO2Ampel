#include <Arduino.h>
#include "MHZ19.h"
#include "FastLED.h"
#include <TM1637Display.h>

#define DATA_PIN 25
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;

#define RX_PIN 9
#define TX_PIN 10
#define BAUDRATE 9600               // Native to the sensor (do not change)
MHZ19 myMHZ19;
HardwareSerial mySerial(1);


#define CLK 18
#define DIO 19
TM1637Display display(CLK, DIO);



unsigned long getDataTimer = 0;

int count = 0;
void setup()
{
    Serial.begin(9600);

    mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);                                 // Uno example: Begin Stream with MHZ19 baudrate

    myMHZ19.begin(mySerial);                                // *Important, Pass your Stream reference

    myMHZ19.autoCalibration(true);
    Serial.print("ABC Status: "); myMHZ19.getABC() ? Serial.println("ON") :  Serial.println("OFF");

    FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);

}

void loop()
{
    if (millis() - getDataTimer >= 2000)     // Check if interval has elapsed (non-blocking delay() equivilant)
    {
        int CO2;
        CO2 = myMHZ19.getCO2();

        Serial.print("CO2 (ppm): ");
        Serial.println(CO2);

        int8_t Temp;                           // Buffer for temperature
        Temp = myMHZ19.getTemperature();       // Request Temperature (as Celsius)

        Serial.print("Temperature (C): ");
        Serial.println(Temp);


        if(CO2 <= 600){
            leds[0] = CRGB(0,255,0);
            FastLED.show();
        }
        else if(CO2 <= 800){
            leds[0] = CRGB(255,255,0);
            FastLED.show();
        }
        else{
            leds[0] = CRGB(255,0,0);
            FastLED.show();
        }
        display.setBrightness(0x0f);
        display.showNumberDecEx(count, 0x00, false, 4, 0);
        count ++;
        if(count == 10000){
            count = 0;
        }

        getDataTimer = millis();              // Update inerval
    }
}

