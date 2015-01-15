#include "setup.h"
#include "tools/jsonhelpers.h"

cWeatherforecastSetup::cWeatherforecastSetup() {
    data = weatherConfig;
    strn0cpy(city, data.city.c_str(), sizeof(city));
    string strLat = FloatToString(data.lat);
    string strLon = FloatToString(data.lon);
    strn0cpy(lat, strLat.c_str(), sizeof(lat));
    strn0cpy(lon, strLon.c_str(), sizeof(lon));
    Setup();
}

cWeatherforecastSetup::~cWeatherforecastSetup() {
}


void cWeatherforecastSetup::Setup(void) {
    int current = Current();
    Clear();

    Add(new cMenuEditStrItem(tr("City"), city, sizeof(city), trVDR(FileNameChars)));
    Add(new cMenuEditStrItem(tr("Latitude"), lat, sizeof(lat), trVDR(FileNameChars)));
    Add(new cMenuEditStrItem(tr("Longitude"), lon, sizeof(lon), trVDR(FileNameChars)));

    SetCurrent(Get(current));
    Display();
}

eOSState cWeatherforecastSetup::ProcessKey(eKeys Key) {
    eOSState state = cMenuSetupPage::ProcessKey(Key);
    switch (state) {
        default: break;
    }
    return state;
}

void cWeatherforecastSetup::Store(void) {
    weatherConfig = data;
    SetupStore("city", city);
    SetupStore("lat", lat);
    SetupStore("lon", lon);

    weatherConfig.city = city;
    weatherConfig.lat = atod(lat);
    weatherConfig.lon = atod(lon);
}