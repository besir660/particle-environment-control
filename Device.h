#include "Particle.h"


#ifndef Device_h
#define Device_h


class DeviceMain
{
    public:
        DeviceMain(int switchpin);
        void Switch(int state);
        void Manual(int state); //1 for manual 0 for automatic control
        bool isSwitchOn();
        bool isManual();
        int WorkTime();
        int TotalWorkTime();
    private:
        int _switchpin;
        int _switchstate;
        int _controlstate;
        int _starttime;
        int _worktime;
        int _totalworktime;
        
};

class Homeostat
{
    public:
    Homeostat(int mode, double target, double hyst);
    void Target(double target);
    void Hysteresis(double hyst);
    int Output(double SensorValue);
    double ReadTarget();
    double ReadHysteresis();
    double ReadLow();
    double ReadHigh();
    private:
        int _mode;
        double _target;
        double _hyst;
        double _high;
        double _low;
        double _SensorValue;
        int _output;
};

#endif

