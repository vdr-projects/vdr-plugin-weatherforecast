#ifndef __WEATHEROSD_H
#define __WEATHEROSD_H

#include <vdr/osdbase.h>
#include <string>
#include "libskindesigner/skindesignerosdbase.h"
#include "libforecastio/forecastio.h"

enum eMenus {
    meRoot,
    meDetailCurrent,
    meDetailHourly,
    meDetailDaily
};

class cWeatherOsd : public cSkindesignerOsdMenu {
private:
    bool isDetailedView;
    int lastRootMenuElement;
    cForecastIO *forecastIO;
    void SetRootMenu(void);
    void SetDetailViewCurrent(void);
    void SetDetailViewHourly(void);
    void SetDetailViewDaily(void);
public:
    cWeatherOsd(cForecastIO *forecastIO);
    virtual ~cWeatherOsd();
    virtual eOSState ProcessKey(eKeys key);
};

#endif //__WEATHEROSD_H