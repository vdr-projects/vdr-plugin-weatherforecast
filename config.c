#include "config.h"

cWeatherforecastConfig::cWeatherforecastConfig() {
    //number of hours to wait till next update from forecast.io is fetched 
    hoursToUpdate = 20;
    //city to display in menus
    city = "";
    //latitude and longitude of location for forecast.io query
    lat = 0.0;
    lon = 0.0;
}

cWeatherforecastConfig::~cWeatherforecastConfig() {
}

bool cWeatherforecastConfig::SetupParse(const char *Name, const char *Value) {
    if      (!strcasecmp(Name, "city"))           city = Value;
    else if (!strcasecmp(Name, "lat"))            lat = atod(Value);
    else if (!strcasecmp(Name, "lon"))            lon = atod(Value);
    else return false;
    return true;
}
