#include <vdr/plugin.h>
#include <string>
#include <sstream>
#include <jansson.h>
#include "../config.h"
#include "../tools/curlfuncs.h"
#include "../tools/filesystem.h"
#include "forecastio.h"

using namespace std;
extern cWeatherforecastConfig weatherConfig;

cForecastIO::cForecastIO(string cacheDir) {
    this->cacheDir = cacheDir;
    ok = false;
    cacheDuration = 60 * 60 * weatherConfig.hoursToUpdate;
    apiKey = "9830052ef63efbec84ec0639e9a205d2";
    string osdLanguage = Setup.OSDLanguage;
    language = osdLanguage.substr(0,2);
    unit = "si";
    baseURL = "https://api.forecast.io/forecast";
    latitude = weatherConfig.lat;
    longitude = weatherConfig.lon;
    current = NULL;
    hourly = NULL;
    daily = NULL;
}

cForecastIO::~cForecastIO() {
    if (current)
        delete current;
    if (hourly)
        delete hourly;
    if (daily)
        delete daily;
}

/*****************************************************************
* PUBLIC FUNCTIONS
*****************************************************************/
void cForecastIO::Action(void) {
    ok = ReadForecast();
}

cForecast *cForecastIO::GetCurrentForecast(void) {
    time_t now = time(0);
    if (current && current->TimeMatch(now))
        return current;
    if (hourly)
        return hourly->GetCurrent();
    return NULL;
}

bool cForecastIO::SetCurrentWeather(cServiceCurrentWeather *call) {
    cForecast *currentForecast = GetCurrentForecast();
    if (!currentForecast)
        return false;
    call->timeStamp = currentForecast->GetDateTimeString();
    call->temperature = currentForecast->GetTemperatureString();
    call->apparentTemperature = currentForecast->GetApparentTemperatureString();
    call->summary = currentForecast->GetSummary();
    call->icon = currentForecast->GetIcon();
    call->precipitationIntensity = currentForecast->GetPrecipIntensityString();
    call->precipitationProbability = currentForecast->GetPrecipProbabilityPercent();
    call->precipitationType = currentForecast->GetPercipType();
    call->humidity = currentForecast->GetHumidityPercent();
    call->windSpeed = currentForecast->GetWindSpeedString();
    call->windBearing = currentForecast->GetWindBearing();
    call->windBearingString = currentForecast->GetWindBearingString();
    call->visibility = currentForecast->GetVisibilityString();
    call->cloudCover = currentForecast->GetCloudCoverPercent();
    call->pressure = currentForecast->GetPressureString();
    call->ozone = currentForecast->GetOzoneString();

    cForecast *dailyForecast = daily->GetFirstDaily();
    if (!dailyForecast)
        return true;
    call->minTemperature = dailyForecast->GetTemperatureMinString();
    call->maxTemperature = dailyForecast->GetTemperatureMaxString();

    return true;
}

/*****************************************************************
* PRIVATE FUNCTIONS
*****************************************************************/
bool cForecastIO::ReadForecast(void) {
    string forecastFile = *cString::sprintf("%s/%s", cacheDir.c_str(), "weather.json");
    dsyslog("weatherforecast: trying to read cached forecast from %s", forecastFile.c_str());
    string forecastJson = "";
    if (!FileExists(forecastFile)) {
        //get new from forecast.io
        dsyslog("weatherforecast: no cached forecast, fetching newly from forecast.io");
        forecastJson = FetchOnlineForecast();
        WriteIntoFile(forecastFile, forecastJson);
    } else {
        //check if cached file is too old
        time_t fileCreation = FileCreationTime(forecastFile);
        time_t now = time(0);
        int age = now - fileCreation;
        int ageHours = age / 3600;
        int ageMinutes = (age%3600) / 60;
        if (age > cacheDuration) {
            //get new from forecast.io
            dsyslog("weatherforecast: cached forecast is with %dh %dmin too old, fetching newly from forecast.io", ageHours, ageMinutes);
            forecastJson = FetchOnlineForecast();
            WriteIntoFile(forecastFile, forecastJson);
        } else {
            //using cached data
            dsyslog("weatherforecast: cached forecast is only %dh %dmin old, using cached forecast", ageHours, ageMinutes);
            forecastJson = ReadFromFile(forecastFile);
        }
    }
    ParseForecast(forecastJson);

    return true;
}

string cForecastIO::FetchOnlineForecast(void) {
    stringstream url;
    url << baseURL << "/" << apiKey;
    url << "/" << latitude << "," << longitude;
    url << "?lang=" << language << "&units=" << unit;
    string outputForecastIO;
    esyslog("weatherforecast: calling URL %s", url.str().c_str());
    CurlGetUrl(url.str().c_str(), &outputForecastIO);
    return outputForecastIO;
}

bool cForecastIO::ParseForecast(string &jsonForecast) {
    json_t *root;
    json_error_t error;

    root = json_loads(jsonForecast.c_str(), 0, &error);
    if ( !root ) {
        return false;
    }
    if ( !json_is_object(root) ) {
        return false;
    }

    //reading current weather
    json_t *currentWeather = json_object_get(root, "currently");
    if (!json_is_object(currentWeather)) {
        return false;
    }
    current = new cForecast();
    current->SetForecastType(ftCurrent);
    current->SetForecast(currentWeather);
    //current->Debug();

    //reading 48 hour forecast
    json_t *hourlyWeather = json_object_get(root, "hourly");
    if (!json_is_object(hourlyWeather)) {
        return false;
    }
    hourly = new cForecasts();
    hourly->SetForecastType(ftHourly);
    hourly->SetForecast(hourlyWeather);
    //hourly->Debug();

    //reading 7 day forecast
    json_t *dailyWeather = json_object_get(root, "daily");
    if (!json_is_object(dailyWeather)) {
        return false;
    }
    daily = new cForecasts();
    daily->SetForecastType(ftDaily);
    daily->SetForecast(dailyWeather);
    //daily->Debug();

    return true;
}
