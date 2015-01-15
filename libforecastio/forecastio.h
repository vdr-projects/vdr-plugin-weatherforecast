#ifndef __FORECASTIO_H
#define __FORECASTIO_H

#include <string>
#include "forecasts.h"
#include "forecast.h"
#include "../services.h"

using namespace std;

class cForecastIO : public cThread {
private:
    bool ok;
    string cacheDir;
    int cacheDuration;
    string apiKey;
    string language;
    string unit;
    string baseURL;
    double latitude;
    double longitude;
    cForecast *current;
    cForecasts *hourly;
    cForecasts *daily;
    string FetchOnlineForecast(void);
    bool ParseForecast(string &jsonForecast);
    void Action(void);
    bool ReadForecast(void);
public:
    cForecastIO(string cacheDir);
    virtual ~cForecastIO(void);
    cForecast *GetCurrentForecast(void);
    cForecasts *GetHourlyForecast(void) { return hourly; };
    cForecasts *GetDailyForecast(void) { return daily; };
    bool SetCurrentWeather(cServiceCurrentWeather *call);
};


#endif //__FORECASTIO_H