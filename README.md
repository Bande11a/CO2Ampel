# CO2Ampel / CO2 Traffic Light

This program was developed by Malte Gehrmann. It is CO2 traffic light powered by an ESP32. It notifys people in the room
when they need to open their windows to let in fresh air by changing an LED from Green to yellow to red.
It will also display the current time and current CO2 value on a 4 digit 7 segment display.

## How to open

This program was developed using Platformio in VSCode and the folder strucuture should allow you to just open it as a PlatformIO project.
If you are using a diffrent IDE just copy the code out of /src/main.cpp into your IDE of choice.

## Libaries used
The libarys I used are:
1. MHZ19 by Jonathan Dempsey
2. FastLED by Daniel Garcia
3. TM1637Display by avishorp
4. RTClib by Adafruit


## PinOuts

1. CO2 Sensor: Tx -> SO2, Rx -> SO3
2. LED: Di -> G25
3. RTC: SDA -> G16, SCL -> G17
4. 7 Segment: CLK -> G18, DIO - > G19
5. Sleep Button: G32


## Questions:
Any questions email: malte.gehrmann@honeywell.com
