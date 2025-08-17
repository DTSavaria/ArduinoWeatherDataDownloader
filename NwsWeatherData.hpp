
/*
 * NwsWeatherData
 * Copyright (c) 2025 Daniel Savaria
 */

#ifndef _NWS_WEATHER_DATA_HPP_
#define _NWS_WEATHER_DATA_HPP_

#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

#define WEATHER_DEBUG 0  // change to 1 for debugging messages

enum TemperatureUnit {
  CELCIUS,
  FAHRENHEIT
};

/**
 * A class for downloading data from the National Weather Service.
 * 
 * In addition to the official Arudino libraries,
 * the ArduinoJson library is used for parsing:
 * https://github.com/bblanchon/ArduinoJson
 * From within the Arduino IDE, go to the Library Manager and
 * search for ArduinoJson (not Arduino_JSON)
 *
 * NOTE: if you update this class, you might have to update the
 * json filter.
 *
 */
class NwsWeatherData {
public:

  /**`
   * Constructor. Needs an SSL Client (like WiFiSSLClient),
   * and the lat/lon as decimal degrees Strings.
   */
  NwsWeatherData(
    Client& httpsCapableClient,
    const String& latitude,
    const String& longitude);


  virtual ~NwsWeatherData() = default;

  /**
   * Check if data has been downloaded and parsed.
   */
  bool hasValidData();

  /**
   * Check the millis() time of the last successful download.
   */
  unsigned long getLastDownloadTime();

  /**
   * Get the temperature unit of the output. Always FAHRENHEIT
   */
  TemperatureUnit getTemperatureUnit();

  /**
   * Get the current observation Temperature at the nearest station.
   */
  double getStationTemperature();

  /**
   * Get the timestamp as a String of the current observation.
   */
  String getObservationTime();

  /**
   * Get todays' high temperature. Returns NAN if it is currently night time.
   */
  double getTodaysHighTemperature();

  /**
   * Get tonight's low temperature.
   */
  double getTonightsLowTemperature();

  /**
   * Try to download new data from the server
   */
  void downloadNewData();

private:
  HttpClient httpClient;
  const String urlPath;
  JsonDocument weatherObject;
  JsonDocument filter;

  bool okay;
  long lastDownload;

  void initiateDownload();

  void logMessage(const String& message) {
#if (WEATHER_DEBUG == 1)
    Serial.print(message);
#endif
  }

  void logMessageLn(const String& message) {
#if (WEATHER_DEBUG == 1)
    Serial.println(message);
#endif
  }
};

#endif