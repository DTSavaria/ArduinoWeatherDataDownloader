/*
    ArduinoWeatherDataDownloader WifiDownloadExample
    Copyright (c) 2025 Daniel Savaria
    https://github.com/DTSavaria/ArduinoWeatherDataDownloader
    
    Demonstrate downloading weather data over wifi.
    1. Open the Serial monitor and set to 9600
    2. See the CUSTOMIZE options below, especially the wifi settings
    3. Upload the example sketch to your Arduino
 */

#include <WiFiS3.h>
#include <NwsWeatherData.hpp>

////// vv CUSTOMIZE OPTIONS vv //////

// 1.  Open this file and set your wifi network name and password
#include "arduino_secrets.h"
const String WIFI_SSID = SECRET_SSID;
const String WIFI_PASS = SECRET_PASS;

/*
    2. Set the your location as latitude and longitude. You can look it up on
    a map. Use decimal degrees. Don't use minutes and seconds. Don't use
    N/S/E/W. Instead N and W should be positive and S and E should be negative.
*/
const String LAT = "41.8292";
const String LON = "-71.4132";

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
  weatherData = new NwsWeatherData(wifi, LAT, LON);

  // grab the data
  Serial.println("Downloading new data");
  weatherData->downloadNewData();

  // check if it downloaded okay
  if (!weatherData->hasValidData()) {
    Serial.println("Could not download weather data. Press reset button.");
    while (true)
      ;
  }

  // start with hazards (watches and warnings)
  size_t hazardCount = weatherData->getHazardCount();

  for (size_t i = 0; i < hazardCount; i++) {
    Serial.print("Alert ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(weatherData->getHazard(i));
  }

  Serial.println(weatherData->getCurrentPeriodName()
                 + ": " + weatherData->getCurrentPeriodWeather());

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

  if (WIFI_SSID.length() == 0) {
    Serial.println("Enter your WiFi network and password in arduino_secrets.h and try again");

    // don't continue
    while (true)
      ;
  }

  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + WIFI_SSID);

    status = WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
    Serial.print("Status code: ");
    Serial.println(status);

    if (status == WL_CONNECT_FAILED) {
      while (true) {
        Serial.println("WiFi connection failed. Check SSID/password");
      }
    }

    if (WiFi.localIP() == "0.0.0.0") {
      status = WL_IDLE_STATUS;
    }
  }

  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}