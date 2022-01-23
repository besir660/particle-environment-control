#include "myTimer.h"


myTimer::myTimer(int interval)
{
    _interval = interval;
    _trigger_sent = 0;
}

bool myTimer::trigger()
{
    if (millis() - _trigger_sent>_interval)
    {
        _trigger_sent = millis();
        return(1);
    }
    else return (0);
}
