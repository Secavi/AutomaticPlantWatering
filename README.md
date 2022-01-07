# AutomaticPlantWatering

Starts a water pump to water plants when the moiture on the plant is low
Includes RTC to maintain correct time and DHT11 to read air temperature and humidity values
Saves the sensor values to an SD card in json format at defined interval
Future improvements:
- Add sensor to read water level on the water container and stop pump when empty
- Add small display to show the sensor values
- Add buttons to allow online setting changes

Hardware

Project developed using Arduino nano!

RTC DS3231 - Real time clock:
SCL - A5
SDA - A4
VCC - 5V
GND - GND

DHT11 - Air temperature and humidity sensor:
Data - D2
VCC - 5V
GND - GND

SPI SD card adaptor:
CS - D10
SCK - D13
MOSI - D11
MISO - D12
VCC - 5V
GND - GND

Soil Capacitive Moisture Sensor:
Analogue Output - A0
VCC - 5V
GND - GND

D5 configured as output to control the water pump