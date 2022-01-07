// Starts a water pump to water plants when the moiture values raise above the defined threshold
// Includes RTC to maintain correct time
// Includes DHT11 to read air temperature and humidity values
// Saves the sensor values to an SD card in json format at defined interval
// Future improvements:
// - Add sensor to read water level on the water container and stop pump when empty
// - Add small display to show the sensor values
// - Add buttons to allow online setting changes
// Created by Sergio Vieira
// 07/01/2022

#include <Wire.h>
#include <DS3231.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN 2
DHT dht(DHTPIN, DHTTYPE);

RTClib myRTC;

// Configurable settings
const int PumpRunningTime = 10; //seconds
const int PumpLockoutTime = 300; //seconds
const int WriteDataDelay = 300; //seconds
const int MoistureThreshold = 357;

// Hardware connections
const int SoilSensorPin = A0;
const int WaterPumpPin = 5;
const int SDCardChipSelect = 10;

// Variable definition
int soilMoisture = 0;
float airTemperature = 0;
float airHumidity = 0;
float heatIndex = 0;
long pumpStartTime = 0;
long latestWriteTime = 0;
bool pumpRunning = false;
File gardenData;

long WriteToSDCard(DateTime currentTime, int soilMoisture, float temperature, float humidity, float heatIndex, bool pumpRunning){

  if (!SD.begin(SDCardChipSelect)) {
    Serial.println("Card not detected...");
    return;
  }
  
  gardenData = SD.open("data.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (gardenData) {
    gardenData.print("{\"dateTime\":\"");
    gardenData.print(currentTime.day());
    gardenData.print("-");
    gardenData.print(currentTime.month());
    gardenData.print("-");
    gardenData.print(currentTime.year());
    gardenData.print(" ");
    gardenData.print(currentTime.hour());
    gardenData.print(":");
    gardenData.print(currentTime.minute());
    gardenData.print(":");
    gardenData.print(currentTime.second());
    gardenData.print("\",");
    gardenData.print("\"soilMoisture\":");
    gardenData.print(soilMoisture);
    gardenData.print(",");
    gardenData.print("\"temperature\":");
    gardenData.print(temperature);
    gardenData.print(",");
    gardenData.print("\"airHumidity\":");
    gardenData.print(humidity);
    gardenData.print(",");
    gardenData.print("\"heatIndex\":");
    gardenData.print(heatIndex);
    gardenData.print(",");
    gardenData.print("\"pumpRunning\":\"");
    if(pumpRunning){
      gardenData.print("true");
    }else{
      gardenData.print("false");
    }
    gardenData.println("\"}");
    // close the file:
    gardenData.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening data.txt");
  }
  
  return currentTime.unixtime();
}

void PrintSerial(DateTime currentTime, int soilMoisture, float temperature, float humidity, float heatIndex, bool pumpRunning){

    Serial.print(currentTime.day());
    Serial.print("-");
    Serial.print(currentTime.month());
    Serial.print("-");
    Serial.print(currentTime.year());
    Serial.print(" ");
    Serial.print(currentTime.hour());
    Serial.print(":");
    Serial.print(currentTime.minute());
    Serial.print(":");
    Serial.println(currentTime.second());
    Serial.println("Sensor Data:");
    Serial.print (" - Soil Moisture: ");
    Serial.println (soilMoisture);
    Serial.print (" - Temperature: ");
    Serial.print (temperature);
    Serial.println ("*C ");
    Serial.print (" - Humidity: ");
    Serial.print (humidity);
    Serial.println ("%");
    Serial.print (" - Heat index: ");
    Serial.print (heatIndex);
    Serial.println ("*C");
    Serial.print(" - Pump Running: ");
    if(pumpRunning){
      Serial.println("true\n");
    }else{
      Serial.println("false\n");
    }
}

void setup () {
    Serial.begin(9600);
    Wire.begin();
    delay(500);
    dht.begin();
    pinMode(WaterPumpPin, OUTPUT);
    digitalWrite(WaterPumpPin, LOW);
}

void loop () {
    
    delay(1000);

    //Get current time from RTC
    DateTime now = myRTC.now();

    // Read soil moisture from analogue sensor
    soilMoisture = analogRead(SoilSensorPin);

    //Read air temperature and humidity from DHT11
    airTemperature = dht.readTemperature();
    airHumidity = dht.readHumidity();
    if (isnan(airTemperature) || isnan(airHumidity)){
      heatIndex = 0;
    }else{
      heatIndex = dht.computeHeatIndex(airTemperature, airHumidity, false);
    }
    
    // Start water pump if soil moisture is lower than the pre-defined threshold and lockout time is finished
    if(soilMoisture > MoistureThreshold && !pumpRunning && (now.unixtime() - pumpStartTime) > PumpLockoutTime){
      digitalWrite(WaterPumpPin, HIGH);
      pumpRunning = true;
      pumpStartTime = now.unixtime();
      Serial.println ("waterPump Started!");
      WriteToSDCard(now, soilMoisture, airTemperature, airHumidity, heatIndex, pumpRunning);
    }

    // Stop water pump after the pre-defined running time
    if(pumpRunning && (now.unixtime() - pumpStartTime) > PumpRunningTime){
      digitalWrite(WaterPumpPin, LOW);
      pumpRunning = false;
      Serial.println ("WaterPump Stopped!");
      WriteToSDCard(now, soilMoisture, airTemperature, airHumidity, heatIndex, pumpRunning);
    }

    // Write to SD card sensor data in pre-defined intervals
    if((now.unixtime() - latestWriteTime) > WriteDataDelay){
      latestWriteTime = WriteToSDCard(now, soilMoisture, airTemperature, airHumidity, heatIndex, pumpRunning);
    }

    //PrintSensorData(now, soilMoisture, airTemperature, airHumidity, heatIndex, pumpRunning);
}
