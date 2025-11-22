/*
    A class for downloading data from the National Weather Service.
    
    Copyright (c) 2025 Daniel Savaria
    https://github.com/DTSavaria/ArduinoWeatherDataDownloader
*/

#include "NwsWeatherData.hpp"

NwsWeatherData::NwsWeatherData(
  Client& httpsCapableClient,
  const String& latitude,
  const String& longitude)
  : httpClient(httpsCapableClient,
               "forecast.weather.gov",
               HttpClient::kHttpsPort),
    urlPath("/MapClick.php?lat=" + latitude
            + "&lon=" + longitude
            + "&unit=0&lg=english&FcstType=json"),
    weatherObject(),
    filter(),
    okay(false),
    lastDownload(0) {

  //set up the filter to just the data we need
  JsonObject filterCurrentObservation = filter["currentobservation"].to<JsonObject>();
  filterCurrentObservation["Temp"] = true;
  filterCurrentObservation["Date"] = true;

  JsonObject filterTime = filter["time"].to<JsonObject>();
  filterTime["startPeriodName"] = true;
  filterTime["tempLabel"] = true;

  JsonObject filterData = filter["data"].to<JsonObject>();
  filterData["temperature"] = true;
  filterData["hazard"] = true;
  filterData["weather"] = true;
}

void NwsWeatherData::downloadNewData() {
  initiateDownload();

  // Deserialize the document
  DeserializationError error = deserializeJson(
    weatherObject, httpClient,
    DeserializationOption::Filter(filter));

  if (error) {
    okay = false;
    logMessage("deserializeJson() failed: ");
    logMessageLn(error.f_str());
  } else {
    okay = true;
    lastDownload = millis();
    logMessageLn("json parse okay");
  }

  httpClient.stop();
}

int NwsWeatherData::convertTemperature(
  double temperature,
  TemperatureUnit inUnit,
  TemperatureUnit outUnit) {
  if (inUnit == outUnit) {
    return temperature;
  } else if (inUnit == TemperatureUnit::FAHRENHEIT) {
    return (temperature - 32) / 1.8 + 0.5;
  } else {
    return temperature * 1.8 + 32 + 0.5;
  }
}

void NwsWeatherData::initiateDownload() {
  httpClient.stop();

  String response = "";
  int getStatus = httpClient.get(urlPath);
  logMessage("httpClient status: ");
  logMessageLn(String(getStatus));

  if (getStatus) {
    httpClient.stop();
    logMessageLn("Error with HTTP client");
    return;
  }

  int httpStatus = httpClient.responseStatusCode();

  logMessage("Status code: ");
  logMessageLn(String(httpStatus));

  if ((httpStatus == 200) && httpClient.available()) {
    logMessageLn("Downloaded weather data");
    logMessage("response length: ");
    logMessageLn(String(httpClient.contentLength()));
  } else {
    logMessageLn("Could not download weather data.");
  }
}

bool NwsWeatherData::hasValidData() const {
  return okay;
}

unsigned long NwsWeatherData::getLastDownloadTime() const {
  return lastDownload;
}

TemperatureUnit NwsWeatherData::getTemperatureUnit() const {
  return TemperatureUnit::FAHRENHEIT;
}


double NwsWeatherData::getStationTemperature() const {
  return weatherObject["currentobservation"]["Temp"];
}

double NwsWeatherData::getStationTemperature(
  const TemperatureUnit outUnit) const {
  return NwsWeatherData::convertTemperature(
    getStationTemperature(),
    getTemperatureUnit(), outUnit);
}

String NwsWeatherData::getObservationTime() const {
  return weatherObject["currentobservation"]["Date"];
}

double NwsWeatherData::getTodaysHighTemperature() const {
  if (String(weatherObject["time"]["tempLabel"][0])
        .equalsIgnoreCase("high")) {
    return weatherObject["data"]["temperature"][0];
  } else {
    return NAN;
  }
}

double NwsWeatherData::getTodaysHighTemperature(
  const TemperatureUnit outUnit) const {
  double value = getTodaysHighTemperature();

  if (isnan(value)) {
    return NAN;
  }

  return NwsWeatherData::convertTemperature(
    value, getTemperatureUnit(), outUnit);
}

double NwsWeatherData::getTonightsLowTemperature() const {
  size_t index = 1;
  if (String(weatherObject["time"]["tempLabel"][0])
        .equalsIgnoreCase("low")) {
    index = 0;
  }
  return weatherObject["data"]["temperature"][index];
}

double NwsWeatherData::getTonightsLowTemperature(
  const TemperatureUnit outUnit) const {
  return NwsWeatherData::convertTemperature(
    getTonightsLowTemperature(),
    getTemperatureUnit(), outUnit);
}

size_t NwsWeatherData::getHazardCount() const {
  return weatherObject["data"]["hazard"].size();
}

String NwsWeatherData::getHazard(size_t index) const {
  return weatherObject["data"]["hazard"][index];
}

String NwsWeatherData::getCurrentPeriodName() const {
  return weatherObject["time"]["startPeriodName"][0];
}

String NwsWeatherData::getCurrentPeriodWeather() const {
  return weatherObject["data"]["weather"][0];
}