
/*
    A class for downloading data from the National Weather Service.
    
    Copyright (c) 2025 Daniel Savaria
    https://github.com/DTSavaria/ArduinoWeatherDataDownloader
 
    In addition to the official Arudino libraries, the ArduinoJson library is 
    used for parsing. Download from https://github.com/bblanchon/ArduinoJson
    Or from within the Arduino IDE, go to the Library Manager and search for
    ArduinoJson (not Arduino_JSON)
    
    NOTE: if you update this class, you might have to update the json filter.
 */

#ifndef _NWS_WEATHER_DATA_HPP_
#define _NWS_WEATHER_DATA_HPP_

#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

#define WEATHER_DEBUG 0  // change to 1 for debugging messages

enum class TemperatureUnit {
  CELSIUS,
  FAHRENHEIT
};

class NwsWeatherData {
public:

  /*`
      Constructor. Needs an SSL Client (like WiFiSSLClient), and the lat/lon as
      decimal degrees Strings. Don't use minutes and seconds. Don't use N/S/E/W.
      Instead N and W should be positive and S and E should be negative.
   */
  NwsWeatherData(
    Client& httpsCapableClient,
    const String& latitude,
    const String& longitude);


  virtual ~NwsWeatherData() = default;

  /*
      Check if data has been downloaded and parsed.
  */
  bool hasValidData() const;

  /*
      Check the millis() time of the last successful download.
  */
  unsigned long getLastDownloadTime() const;

  /*
      Get the temperature unit of the output. Always FAHRENHEIT
  */
  TemperatureUnit getTemperatureUnit() const;

  /*
      Get the current observation Temperature at the nearest station in the
      default unit.
  */
  double getStationTemperature() const;

  /*
      Get the current observation Temperature at the nearest station in the
      given unit.
  */
  double getStationTemperature(const TemperatureUnit outUnit) const;

  /*
      Get the timestamp as a String of the current observation.
  */
  String getObservationTime() const;

  /*
      Get todays' high temperature in the default unit. Returns NAN if it is
      currently night time.
  */
  double getTodaysHighTemperature() const;

  /*
      Get todays' high temperature in the given unit. Returns NAN if it is
      currently night time.
  */
  double getTodaysHighTemperature(const TemperatureUnit outUnit) const;

  /*
      Get tonight's low temperature in the default unit.
  */
  double getTonightsLowTemperature() const;

  /*
      Get tonight's low temperature in the given unit.
  */
  double getTonightsLowTemperature(const TemperatureUnit outUnit) const;

  /*
      Get the number of hazard messages.
  */
  size_t getHazardCount() const;

  /*
      Get hazard message with the given index.
  */
  String getHazard(size_t index) const;

  /*
      Get the name of the current period.
  */
  String getCurrentPeriodName() const;

  /*
      Get the description of the current period weather.
  */
  String getCurrentPeriodWeather() const;

  /*
      Get the current observation weather image file name.
  */
  String getWeatherImageFileName() const;


  /*
      Try to download new data from the server
  */
  void downloadNewData();

  static double convertTemperature(
    double temperature,
    TemperatureUnit inUnit,
    TemperatureUnit outUnit);

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
