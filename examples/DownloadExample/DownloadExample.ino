/*
 * ArduinoWeatherDataDownloader DownloadExample
 * Copyright (c) 2025 Daniel Savaria
 * Demonstrate downloading weather data over wifi.
 * 1. Open the Serial monitor and set to 9600
 * 2. See the CUSTOMIZE options below, especially the wifi settings
 * 3. Upload the example sketch to your Arduino
 */

#include <WiFiS3.h>
#include <NwsWeatherData.hpp>

#include "arudino_secrets.h"

////// vv CUSTOMIZE OPTIONS vv //////

// first set the your location as decimal degree Strings
String lat = "41.8292";
String lon = "-71.4132";

// then set your wifi network and password in arduino_secrets.h
String ssid = SECRET_SSID;
String pass = SECRET_PASS;

////// ^^ CUSTOMIZE OPTIONS ^^ //////

WiFiSSLClient wifi;
NwsWeatherData* weatherData;

void setup() {
  // start the serial connection
  Serial.begin(9600);

  delay(1000);
  Serial.println();
  Serial.println("Starting");

  //connect to the wifi
  connectToWifi();

  // create a data downloader
  weatherData = new NwsWeatherData(wifi, lat, lon);

  // grab the data
  Serial.println("Downloading new data");
  weatherData->downloadNewData();

  // check if it downloaded okay
  if (!weatherData->hasValidData()) {
    Serial.println("Could not download weather data. Press reset button.");
    while (true)
      ;
  }

  // print the temperatures
  Serial.print("Recorded temperature: ");
  Serial.print(weatherData->getStationTemperature());
  Serial.print(" °F at ");
  Serial.println(weatherData->getObservationTime());

  double high = weatherData->getTodaysHighTemperature();
  if (!isnan(high)) {
    Serial.print("Today's high: ");
    Serial.print(high);
    Serial.println(" °F");
  }

  Serial.print("Tonight's low: ");
  Serial.print(weatherData->getTonightsLowTemperature());
  Serial.println(" °F");

  Serial.println("Press the Arduino reset button to refresh data");
}

void loop() {
  //not used
}

void connectToWifi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the wifi firmware");
  }

  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + ssid);
    status = WiFi.begin(ssid.c_str(), pass.c_str());
    if (WiFi.localIP() == "0.0.0.0") {
      status = WL_IDLE_STATUS;
    }
    delay(100);
  }

  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}