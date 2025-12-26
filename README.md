# ArduinoWeatherDataDownloader
A library to download weather data from the National Weather Service for use by Arduino sketches.  

To use this library, place this directory into your `Arduino/libraries` directory. 
Then select it in the Arudino IDE by going to `Sketch > Include Library > (Contributed libraries) > ArduinoWeatherDataDownloader` Your sketch should now include the following line:
```C
#include <ArduinoWeatherDataDownloader.hpp>
```

An example can be found by using the Arduino IDE to go to `Files > Examples > (Examples from Custom Libraries) > ArduinoWeatherDataDownloader`

Useful documentation can be found in the `NwsWeatherData.hpp` file.

This has been tested with the Arduino Uno R4 Wifi.  

It relies on the ArduinoJson Library (not Arduino_JSON), which can be downloaded directly from the Arduino IDE. https://github.com/bblanchon/ArduinoJson
