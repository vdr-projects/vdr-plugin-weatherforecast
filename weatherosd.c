#include "config.h"
#include "tools/jsonhelpers.h"
#include "weatherosd.h"

extern cWeatherforecastConfig weatherConfig;

cWeatherOsd::cWeatherOsd(cForecastIO *forecastIO) : cSkindesignerOsdMenu("Weather Forecast") {
    isDetailedView = false;
    lastRootMenuElement = 0;
    SetPluginName("weatherforecast");
    this->forecastIO = forecastIO;
    SetRootMenu();
}

cWeatherOsd::~cWeatherOsd(void) {
    
}

eOSState cWeatherOsd::ProcessKey(eKeys key) {
    eOSState state = cOsdMenu::ProcessKey(key);
    switch (key) {
        case kOk: {
            if (isDetailedView) {
                SetRootMenu();
            } else {
                int element = Current();
                lastRootMenuElement = element;
                if (element == 0)
                    SetDetailViewCurrent();
                else if (element == 1)
                    SetDetailViewHourly();
                else if (element == 2)
                    SetDetailViewDaily();
            }
            state = osContinue;
            break;
        } case kLeft: {
            TextKeyLeft();
            state = osContinue;
            break;
        } case kRight: {
            TextKeyRight();
            state = osContinue;            
            break;
        } case kUp: {
            TextKeyUp();
            state = osContinue;            
            break;
        } case kDown: {
            TextKeyDown();
            state = osContinue;
            break;
        } case kBack: {
            if (isDetailedView) {
                SetRootMenu();
                state = osContinue;
            }
            break;
        } default:
            break;
    }
    return state;
}

void cWeatherOsd::SetRootMenu(void) {
    isDetailedView = false;
    SetPluginMenu(meRoot, skindesignerapi::mtList);
    Clear();
    SetTitle(tr("Weather Forecast"));
    
    forecastIO->LockForecasts();

    skindesignerapi::cSkindesignerOsdItem *currentWeather = new skindesignerapi::cSkindesignerOsdItem();
    string itemLabelCurrent = tr("Current Weather");
    currentWeather->SetText(itemLabelCurrent.c_str());
    currentWeather->AddStringToken("menuitemtext", itemLabelCurrent.c_str());
    currentWeather->AddIntToken("iscurrent", 1);
    currentWeather->AddIntToken("ishourly", 0);
    currentWeather->AddIntToken("isdaily", 0);
    currentWeather->AddStringToken("city", weatherConfig.city);
    currentWeather->AddStringToken("latitude", FloatToString(weatherConfig.lat));
    currentWeather->AddStringToken("longitude", FloatToString(weatherConfig.lon));

    cForecast *currentForecast = forecastIO->GetCurrentForecast();
    if (currentForecast) {
        currentWeather->AddStringToken("timestamp", currentForecast->GetDateTimeString());
        currentWeather->AddStringToken("temperature", currentForecast->GetTemperatureString());
        currentWeather->AddStringToken("apparenttemperature", currentForecast->GetApparentTemperatureString());
        currentWeather->AddStringToken("summary", currentForecast->GetSummary());
        currentWeather->AddStringToken("icon", currentForecast->GetIcon());
        currentWeather->AddStringToken("precipitationintensity", currentForecast->GetPrecipIntensityString());
        currentWeather->AddIntToken("precipitationprobability", currentForecast->GetPrecipProbabilityPercent());
        currentWeather->AddStringToken("precipitationtype", currentForecast->GetPercipType());
        currentWeather->AddIntToken("humidity", currentForecast->GetHumidityPercent());
        currentWeather->AddStringToken("windspeed", currentForecast->GetWindSpeedString());
        currentWeather->AddIntToken("windbearing", currentForecast->GetWindBearing());
        currentWeather->AddStringToken("windbearingstring", currentForecast->GetWindBearingString());
        currentWeather->AddStringToken("visibility", currentForecast->GetVisibilityString());
        currentWeather->AddIntToken("cloudcover", currentForecast->GetCloudCoverPercent());
        currentWeather->AddStringToken("pressure", currentForecast->GetPressureString());
        currentWeather->AddStringToken("ozone", currentForecast->GetOzoneString());
    }
    Add(currentWeather, (lastRootMenuElement == 0)?true:false);

    skindesignerapi::cSkindesignerOsdItem *nextHours = new skindesignerapi::cSkindesignerOsdItem();
    string itemLabelNextHours = tr("Next 48 Hours");
    nextHours->SetText(itemLabelNextHours.c_str());
    nextHours->AddStringToken("menuitemtext", itemLabelNextHours.c_str());
    nextHours->AddIntToken("iscurrent", 0);
    nextHours->AddIntToken("ishourly", 1);
    nextHours->AddIntToken("isdaily", 0);
    nextHours->AddStringToken("city", weatherConfig.city);
    nextHours->AddStringToken("latitude", FloatToString(weatherConfig.lat));
    nextHours->AddStringToken("longitude", FloatToString(weatherConfig.lon));

    cForecasts *hourlyForecast = forecastIO->GetHourlyForecast();
    if (hourlyForecast) {
        nextHours->AddStringToken("summary", hourlyForecast->GetSummary());
        nextHours->AddStringToken("icon", hourlyForecast->GetIcon());

        cForecast *fc = hourlyForecast->GetFirstHourly();
        int num = 1;
        while (fc) {
            map<string, string> tokens;
            tokens.insert(pair<string,string>("hourly[num]", IntToString(num++)));
            tokens.insert(pair<string,string>("hourly[timestamp]", fc->GetTimeString()));
            tokens.insert(pair<string,string>("hourly[temperature]", fc->GetTemperatureString()));
            tokens.insert(pair<string,string>("hourly[apparenttemperature]", fc->GetApparentTemperatureString()));
            tokens.insert(pair<string,string>("hourly[summary]", fc->GetSummary()));
            tokens.insert(pair<string,string>("hourly[icon]", fc->GetIcon()));
            tokens.insert(pair<string,string>("hourly[precipitationintensity]", fc->GetPrecipIntensityString()));
            tokens.insert(pair<string,string>("hourly[precipitationprobability]", IntToString(fc->GetPrecipProbabilityPercent())));
            tokens.insert(pair<string,string>("hourly[precipitationtype]", fc->GetPercipType()));
            tokens.insert(pair<string,string>("hourly[humidity]", IntToString(fc->GetHumidityPercent())));
            tokens.insert(pair<string,string>("hourly[windspeed]", fc->GetWindSpeedString()));
            tokens.insert(pair<string,string>("hourly[windbearing]", IntToString(fc->GetWindBearing())));
            tokens.insert(pair<string,string>("hourly[windbearingstring]", fc->GetWindBearingString()));
            tokens.insert(pair<string,string>("hourly[visibility]", fc->GetVisibilityString()));
            tokens.insert(pair<string,string>("hourly[cloudcover]", IntToString(fc->GetCloudCoverPercent())));
            tokens.insert(pair<string,string>("hourly[pressure]", fc->GetPressureString()));
            tokens.insert(pair<string,string>("hourly[ozone]", fc->GetOzoneString()));
            nextHours->AddLoopToken("hourly", tokens);
            fc = hourlyForecast->GetNext();
        }
    }

    Add(nextHours, (lastRootMenuElement == 1)?true:false);

    skindesignerapi::cSkindesignerOsdItem *nextDays = new skindesignerapi::cSkindesignerOsdItem();
    string itemLabelNextDays = tr("Next 7 Days");
    nextDays->SetText(itemLabelNextDays.c_str());
    nextDays->AddStringToken("menuitemtext", itemLabelNextDays.c_str());
    nextDays->AddIntToken("iscurrent", 0);
    nextDays->AddIntToken("ishourly", 0);
    nextDays->AddIntToken("isdaily", 1);
    nextDays->AddStringToken("city", weatherConfig.city);
    nextDays->AddStringToken("latitude", FloatToString(weatherConfig.lat));
    nextDays->AddStringToken("longitude", FloatToString(weatherConfig.lon));

    cForecasts *dailyForecast = forecastIO->GetDailyForecast();
    if (dailyForecast) {
        nextDays->AddStringToken("summary", dailyForecast->GetSummary());
        nextDays->AddStringToken("icon", dailyForecast->GetIcon());

        cForecast *fc = dailyForecast->GetFirstDaily();
        while (fc) {
            map<string, string> tokens;
            tokens.insert(pair<string,string>("daily[day]", fc->GetDateString()));
            tokens.insert(pair<string,string>("daily[dayname]", fc->GetDayName()));
            tokens.insert(pair<string,string>("daily[temperaturemin]", fc->GetTemperatureMinString()));
            tokens.insert(pair<string,string>("daily[temperaturemintime]", fc->GetTemperatureMinTimeString()));
            tokens.insert(pair<string,string>("daily[temperaturemax]", fc->GetTemperatureMaxString()));
            tokens.insert(pair<string,string>("daily[temperaturemaxtime]", fc->GetTemperatureMaxTimeString()));
            tokens.insert(pair<string,string>("daily[summary]", fc->GetSummary()));
            tokens.insert(pair<string,string>("daily[icon]", fc->GetIcon()));
            tokens.insert(pair<string,string>("daily[precipitationintensity]", fc->GetPrecipIntensityString()));
            tokens.insert(pair<string,string>("daily[precipitationprobability]", IntToString(fc->GetPrecipProbabilityPercent())));
            tokens.insert(pair<string,string>("daily[precipitationtype]", fc->GetPercipType()));
            tokens.insert(pair<string,string>("daily[humidity]", IntToString(fc->GetHumidityPercent())));
            tokens.insert(pair<string,string>("daily[windspeed]", fc->GetWindSpeedString()));
            tokens.insert(pair<string,string>("daily[windbearing]", IntToString(fc->GetWindBearing())));
            tokens.insert(pair<string,string>("daily[windbearingstring]", fc->GetWindBearingString()));
            tokens.insert(pair<string,string>("daily[visibility]", fc->GetVisibilityString()));
            tokens.insert(pair<string,string>("daily[cloudcover]", IntToString(fc->GetCloudCoverPercent())));
            tokens.insert(pair<string,string>("daily[pressure]", fc->GetPressureString()));
            tokens.insert(pair<string,string>("daily[ozone]", fc->GetOzoneString()));
            nextDays->AddLoopToken("daily", tokens);
            fc = dailyForecast->GetNext();
        }
    }

    forecastIO->UnlockForecasts();

    Add(nextDays, (lastRootMenuElement == 2)?true:false);

    Display();
}

void cWeatherOsd::SetDetailViewCurrent(void) {
    isDetailedView = true;
    SetPluginMenu(meDetailCurrent, skindesignerapi::mtText);
    ClearTokens();
    Clear();
    SetTitle(tr("Current Weather"));

    cForecast *current = NULL;
    cForecasts *daily = NULL;
    cForecast *today = NULL;

    forecastIO->LockForecasts();
    bool ok = true;
    
    current = forecastIO->GetCurrentForecast();
    if (!current) ok = false;
    
    if (ok)
        daily = forecastIO->GetDailyForecast();
    if (!daily) ok = false;
    
    if (ok)
        today = daily->GetFirstDaily();
    if (!today) ok = false;
    
    if (!ok) {
        forecastIO->UnlockForecasts();
        return;
    }

    stringstream plainText;
    plainText << tr("Weather for") << " " << weatherConfig.city << " " << current->GetDateTimeString() << ": " <<  current->GetSummary() << "\n";
    plainText << tr("Current Temperature") << ": " << current->GetTemperatureString() << "°C" << "\n";
    plainText << tr("Apparent Temperature") << ": " << current->GetApparentTemperatureString() << "°C" << "\n";
    plainText << tr("Minimum Temperature today") << ": " << today->GetTemperatureMinString() << "°C" << "\n";
    plainText << tr("Maximum Temperature today") << ": " << today->GetTemperatureMaxString() << "°C" << "\n";
    plainText << tr("Precipitation Probability") << ": " << current->GetPrecipProbabilityPercent() << " %" << "\n";
    plainText << tr("Precipitation Intensity") << ": " << current->GetPrecipIntensityString() << " l/qm" << "\n";
    plainText << tr("Humidity") << ": " << current->GetHumidityPercent() << " %" << "\n";
    plainText << tr("Wind Speed") << ": " << current->GetWindSpeedString() << " km/h" << "\n";
    plainText << tr("Wind Bearing") << ": " << current->GetWindBearing() << "° " << current->GetWindBearingString() << "\n";
    plainText << tr("Visibility") << ": " << current->GetVisibilityString() << " km" << "\n";
    plainText << tr("Cloud Cover") << ": " << current->GetCloudCoverPercent() << " %" << "\n";
    plainText << tr("Pressure") << ": " << current->GetPressureString() << " HPa" << "\n";
    plainText << tr("Ozone") << ": " << current->GetOzoneString() << " DU" << "\n";
    SetText(plainText.str().c_str());


    AddStringToken("menuheader", tr("Current Weather"));
    AddStringToken("city", weatherConfig.city);
    AddStringToken("latitude", FloatToString(weatherConfig.lat));
    AddStringToken("longitude", FloatToString(weatherConfig.lon));
    AddStringToken("timestamp", current->GetDateTimeString());
    AddStringToken("temperature", current->GetTemperatureString());
    AddStringToken("apparenttemperature", current->GetApparentTemperatureString());
    AddStringToken("mintemperature", today->GetTemperatureMinString());
    AddStringToken("maxtemperature", today->GetTemperatureMaxString());
    AddStringToken("summary", current->GetSummary());
    AddStringToken("icon", current->GetIcon());
    AddStringToken("precipitationintensity", current->GetPrecipIntensityString());
    AddIntToken("precipitationprobability", current->GetPrecipProbabilityPercent());
    AddStringToken("precipitationtype", current->GetPercipType());
    AddIntToken("humidity", current->GetHumidityPercent());
    AddStringToken("windspeed", current->GetWindSpeedString());
    AddIntToken("windbearing", current->GetWindBearing());
    AddStringToken("windbearingstring", current->GetWindBearingString());
    AddStringToken("visibility", current->GetVisibilityString());
    AddIntToken("cloudcover", current->GetCloudCoverPercent());
    AddStringToken("pressure", current->GetPressureString());
    AddStringToken("ozone", current->GetOzoneString());

    forecastIO->UnlockForecasts();
    
    Display();
}

void cWeatherOsd::SetDetailViewHourly(void) {
    isDetailedView = true;
    SetPluginMenu(meDetailHourly, skindesignerapi::mtText);
    Clear();
    ClearTokens();
    SetTitle(tr("Weather in the next 48 Hours"));

    forecastIO->LockForecasts();
    cForecasts *hourly = forecastIO->GetHourlyForecast();
    if (!hourly) {
        forecastIO->UnlockForecasts();
        return;
    }

    stringstream plainText;
    cForecast *fc = hourly->GetFirstHourly();

    AddStringToken("menuheader", tr("Weather in the next 48 Hours"));
    AddStringToken("summary", hourly->GetSummary());
    AddStringToken("icon", hourly->GetIcon());
    AddStringToken("city", weatherConfig.city);
    AddStringToken("latitude", FloatToString(weatherConfig.lat));
    AddStringToken("longitude", FloatToString(weatherConfig.lon));

    int num = 1;
    while (fc) {
        map<string, string> tokens;
        tokens.insert(pair<string,string>("hourly[num]", IntToString(num++)));
        tokens.insert(pair<string,string>("hourly[timestamp]", fc->GetTimeString()));
        tokens.insert(pair<string,string>("hourly[temperature]", fc->GetTemperatureString()));
        tokens.insert(pair<string,string>("hourly[apparenttemperature]", fc->GetApparentTemperatureString()));
        tokens.insert(pair<string,string>("hourly[summary]", fc->GetSummary()));
        tokens.insert(pair<string,string>("hourly[icon]", fc->GetIcon()));
        tokens.insert(pair<string,string>("hourly[precipitationintensity]", fc->GetPrecipIntensityString()));
        tokens.insert(pair<string,string>("hourly[precipitationprobability]", IntToString(fc->GetPrecipProbabilityPercent())));
        tokens.insert(pair<string,string>("hourly[precipitationtype]", fc->GetPercipType()));
        tokens.insert(pair<string,string>("hourly[humidity]", IntToString(fc->GetHumidityPercent())));
        tokens.insert(pair<string,string>("hourly[windspeed]", fc->GetWindSpeedString()));
        tokens.insert(pair<string,string>("hourly[windbearing]", IntToString(fc->GetWindBearing())));
        tokens.insert(pair<string,string>("hourly[windbearingstring]", fc->GetWindBearingString()));
        tokens.insert(pair<string,string>("hourly[visibility]", fc->GetVisibilityString()));
        tokens.insert(pair<string,string>("hourly[cloudcover]", IntToString(fc->GetCloudCoverPercent())));
        tokens.insert(pair<string,string>("hourly[pressure]", fc->GetPressureString()));
        tokens.insert(pair<string,string>("hourly[ozone]", fc->GetOzoneString()));
        AddLoopToken("hourly", tokens);
    
        plainText << fc->GetDateTimeString() << ": " <<  fc->GetSummary() << "\n";
        plainText << tr("Temperature") << ": " << fc->GetTemperatureString() << "°C, " << tr("felt") << " " << fc->GetApparentTemperatureString() << "°C""\n";
        plainText << tr("Precipitation Probability") << ": " << fc->GetPrecipProbabilityPercent() << " %, " << tr("Intensity") << ": " << fc->GetPrecipIntensityString() << " l/qm""\n";
        plainText << tr("Wind") << ": " << fc->GetWindSpeedString() << " km/h, " << fc->GetWindBearing() << "° " << fc->GetWindBearingString() << "\n";
        plainText << tr("Cloud Cover") << ": " << fc->GetCloudCoverPercent() << " %" << "\n";
        plainText << "\n";

        fc = hourly->GetNext();
    }
    forecastIO->UnlockForecasts();

    SetText(plainText.str().c_str());
    Display();
}

void cWeatherOsd::SetDetailViewDaily(void) {
    isDetailedView = true;
    SetPluginMenu(meDetailDaily, skindesignerapi::mtText);
    Clear();
    ClearTokens();
    SetTitle(tr("Weather the next 7 days"));

    forecastIO->LockForecasts();
    cForecasts *daily = forecastIO->GetDailyForecast();
    if (!daily) {
        forecastIO->UnlockForecasts();
        return;
    }

    stringstream plainText;
    cForecast *fc = daily->GetFirstDaily();

    AddStringToken("menuheader", tr("Weather the next 7 days"));
    AddStringToken("summary", daily->GetSummary());
    AddStringToken("icon", daily->GetIcon());
    AddStringToken("city", weatherConfig.city);
    AddStringToken("latitude", FloatToString(weatherConfig.lat));
    AddStringToken("longitude", FloatToString(weatherConfig.lon));

    while (fc) {
        map<string, string> tokens;
        tokens.insert(pair<string,string>("daily[day]", fc->GetDateString()));
        tokens.insert(pair<string,string>("daily[dayname]", fc->GetDayName()));
        tokens.insert(pair<string,string>("daily[temperaturemin]", fc->GetTemperatureMinString()));
        tokens.insert(pair<string,string>("daily[temperaturemintime]", fc->GetTemperatureMinTimeString()));
        tokens.insert(pair<string,string>("daily[temperaturemax]", fc->GetTemperatureMaxString()));
        tokens.insert(pair<string,string>("daily[temperaturemaxtime]", fc->GetTemperatureMaxTimeString()));
        tokens.insert(pair<string,string>("daily[summary]", fc->GetSummary()));
        tokens.insert(pair<string,string>("daily[icon]", fc->GetIcon()));
        tokens.insert(pair<string,string>("daily[precipitationintensity]", fc->GetPrecipIntensityString()));
        tokens.insert(pair<string,string>("daily[precipitationprobability]", IntToString(fc->GetPrecipProbabilityPercent())));
        tokens.insert(pair<string,string>("daily[precipitationtype]", fc->GetPercipType()));
        tokens.insert(pair<string,string>("daily[humidity]", IntToString(fc->GetHumidityPercent())));
        tokens.insert(pair<string,string>("daily[windspeed]", fc->GetWindSpeedString()));
        tokens.insert(pair<string,string>("daily[windbearing]", IntToString(fc->GetWindBearing())));
        tokens.insert(pair<string,string>("daily[windbearingstring]", fc->GetWindBearingString()));
        tokens.insert(pair<string,string>("daily[visibility]", fc->GetVisibilityString()));
        tokens.insert(pair<string,string>("daily[cloudcover]", IntToString(fc->GetCloudCoverPercent())));
        tokens.insert(pair<string,string>("daily[pressure]", fc->GetPressureString()));
        tokens.insert(pair<string,string>("daily[ozone]", fc->GetOzoneString()));
        AddLoopToken("daily", tokens);
    
        plainText << fc->GetDayName() << " " << fc->GetDateString() << ": " <<  fc->GetSummary() << "\n";
        plainText << tr("Minimum Temperature") << ": " << fc->GetTemperatureMinString() << "°C " << tr("at") << " " << fc->GetTemperatureMinTimeString() << " ,";
        plainText << tr("Maximum Temperature") << ": " << fc->GetTemperatureMaxString() << "°C " << tr("at") << " " << fc->GetTemperatureMaxTimeString() << "\n";
        plainText << tr("Precipitation Probability") << ": " << fc->GetPrecipProbabilityPercent() << " %, " << tr("Intensity") << ": " << fc->GetPrecipIntensityString() << " l/qm""\n";
        plainText << tr("Wind") << ": " << fc->GetWindSpeedString() << " km/h, " << fc->GetWindBearing() << "° " << fc->GetWindBearingString() << "\n";
        plainText << tr("Cloud Cover") << ": " << fc->GetCloudCoverPercent() << " %" << "\n";
        plainText << tr("Pressure") << ": " << fc->GetPressureString() << " HPa" << "\n";
        plainText << tr("Ozone") << ": " << fc->GetOzoneString() << " DU" << "\n";
        plainText << "\n";

        fc = daily->GetNext();
    }
    forecastIO->UnlockForecasts();
    
    SetText(plainText.str().c_str());
    Display();
}
