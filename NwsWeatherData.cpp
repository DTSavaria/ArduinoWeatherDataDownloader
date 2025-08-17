/*
 * NwsWeatherData
 * Copyright (c) 2025 Daniel Savaria
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
  JsonObject filter_currentobservation = filter["currentobservation"].to<JsonObject>();
  filter_currentobservation["Temp"] = true;
  filter_currentobservation["Date"] = true;
  filter["time"]["tempLabel"] = true;
  filter["data"]["temperature"] = true;
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

bool NwsWeatherData::hasValidData() {
  return okay;
}

unsigned long NwsWeatherData::getLastDownloadTime() {
  return lastDownload;
}

TemperatureUnit NwsWeatherData::getTemperatureUnit() {
  return TemperatureUnit::FAHRENHEIT;
}


double NwsWeatherData::getStationTemperature() {
  return weatherObject["currentobservation"]["Temp"];
}

String NwsWeatherData::getObservationTime() {
  return weatherObject["currentobservation"]["Date"];
}

double NwsWeatherData::getTodaysHighTemperature() {
  if (String(weatherObject["time"]["tempLabel"][0])
        .equalsIgnoreCase("high")) {
    return weatherObject["data"]["temperature"][0];
  } else {
    return NAN;
  }
}

double NwsWeatherData::getTonightsLowTemperature() {
  size_t index = 1;
  if (String(weatherObject["time"]["tempLabel"][0])
        .equalsIgnoreCase("low")) {
    index = 0;
  }
  return weatherObject["data"]["temperature"][index];
}