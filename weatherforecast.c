/*
 * weatherforecast.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include "config.h"
#include "setup.h"
#include "libforecastio/forecastio.h"
#include "libforecastio/locator.h"
#include "libskindesigner/services.h"
#include "weatherosd.h"
#include "services.h"

//***************************************************************************
// Constants
//***************************************************************************
static const char *VERSION        = "0.0.3";
static const char *DESCRIPTION    = "Weatherforecast based on forecast.io";
static const char *MAINMENUENTRY  = tr("WeatherForecast");

//***************************************************************************
// Globals
//***************************************************************************
cWeatherforecastConfig weatherConfig;

//***************************************************************************
// cPluginWeatherforecast
//***************************************************************************
class cPluginWeatherforecast : public cPlugin {
private:
    cForecastIO *forecastIO;
public:
    cPluginWeatherforecast(void);
    virtual ~cPluginWeatherforecast();
    virtual const char *Version(void) { return VERSION; }
    virtual const char *Description(void) { return DESCRIPTION; }
    virtual const char *CommandLineHelp(void);
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Housekeeping(void);
    virtual void MainThreadHook(void);
    virtual cString Active(void);
    virtual time_t WakeupTime(void);
    virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
    virtual cOsdObject *MainMenuAction(void);
    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);
    virtual bool Service(const char *Id, void *Data = NULL);
    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

cPluginWeatherforecast::cPluginWeatherforecast(void) {
    forecastIO = NULL;
}

cPluginWeatherforecast::~cPluginWeatherforecast() {
    if (forecastIO) {
        delete forecastIO;
    }
}

const char *cPluginWeatherforecast::CommandLineHelp(void) {
    return NULL;
}

bool cPluginWeatherforecast::ProcessArgs(int argc, char *argv[]) {
    return true;
}

bool cPluginWeatherforecast::Initialize(void) {
    //check geolocation on first start
    if (weatherConfig.city.size() == 0) {
        dsyslog("weatherforecast: location not set, setting from own public IP");
        cForecastLocator locator;
        if (locator.ReadLocationByIP()) {
            locator.WriteToSetup(this);
        }
        dsyslog("weatherforecast: location wrote to setup");
    }
    return true;
}

bool cPluginWeatherforecast::Start(void) {
    string cacheDir = cPlugin::CacheDirectory(PLUGIN_NAME_I18N);
    forecastIO = new cForecastIO(cacheDir);
    forecastIO->Start();

    RegisterPlugin reg;
    reg.name = "weatherforecast";
    reg.SetMenu(meRoot, "weatherforecast.xml");
    reg.SetMenu(meDetailCurrent, "weatherforecastdetailcurrent.xml");
    reg.SetMenu(meDetailHourly, "weatherforecastdetailhourly.xml");
    reg.SetMenu(meDetailDaily, "weatherforecastdetaildaily.xml");
    static cPlugin *pSkinDesigner = cPluginManager::GetPlugin("skindesigner");
    if (pSkinDesigner) {
      pSkinDesigner->Service("RegisterPlugin", &reg);
    } else {
      esyslog("weatherforecast: skindesigner not available");
    }

    return true;
}

void cPluginWeatherforecast::Stop(void) {
    if (forecastIO) {
        forecastIO->Stop();
    }
}

void cPluginWeatherforecast::Housekeeping(void) {

}

void cPluginWeatherforecast::MainThreadHook(void) {

}

cString cPluginWeatherforecast::Active(void) {
    return NULL;
}

time_t cPluginWeatherforecast::WakeupTime(void) {
    return 0;
}

cOsdObject *cPluginWeatherforecast::MainMenuAction(void) {
    cOsdObject *menu = new cWeatherOsd(forecastIO);
    return menu;
}

cMenuSetupPage *cPluginWeatherforecast::SetupMenu(void) {
    return new cWeatherforecastSetup();
}

bool cPluginWeatherforecast::SetupParse(const char *Name, const char *Value) {
    return weatherConfig.SetupParse(Name, Value);
}

bool cPluginWeatherforecast::Service(const char *Id, void *Data) {
    if (Data == NULL)
        return false;
    if (strcmp(Id, "GetCurrentWeather") == 0) {
        cServiceCurrentWeather* call = (cServiceCurrentWeather*) Data;
        bool ok = false;
        forecastIO->LockForecasts();
        ok = forecastIO->SetCurrentWeather(call);
        forecastIO->UnlockForecasts();
        return ok;
    }
    return false;
}

const char **cPluginWeatherforecast::SVDRPHelpPages(void) {
    return NULL;
}

cString cPluginWeatherforecast::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
    return NULL;
}

VDRPLUGINCREATOR(cPluginWeatherforecast); // Don't touch this!
