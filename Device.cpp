#include "Device.h"

DeviceMain::DeviceMain(int switchpin)
{
    pinMode(switchpin, OUTPUT);
    _switchpin = switchpin;
    digitalWrite(_switchpin, LOW);
    _worktime = 0;
    _totalworktime = 0;
    _starttime = 0;
    _controlstate = 1;
    _switchstate = 0;
    
}


void DeviceMain::Switch(int state)
{   
    if (state == 1)
    {
        digitalWrite(_switchpin, HIGH);
        if (_switchstate == 0) _starttime = Time.now();
        _switchstate = 1;
    }
    else if (state == 0)
    {
        digitalWrite(_switchpin, LOW);
        if (_switchstate == 1) {
            _totalworktime = _totalworktime + (Time.now() - _starttime);
            _worktime = 0;
        }
        _switchstate = 0;
    }
}


bool DeviceMain::isSwitchOn()
{
    return _switchstate;
}

void DeviceMain::Manual(int state) //1 for manual 0 for automatic control
{   
    if (state == 1)
    {
        _controlstate = 1;
    }
    else if (state == 0)
    {
        _controlstate = 0;
    }
}


bool DeviceMain::isManual()
{
    return _controlstate;
}


int DeviceMain::WorkTime()
{
    if (_switchstate == 1) _worktime =  Time.now() - _starttime;
    return _worktime;
}

int DeviceMain::TotalWorkTime()
{
    return _totalworktime;
}
Homeostat::Homeostat(int mode, double target, double hyst)
{
    _mode = mode;
    _target = target;
    _hyst = hyst;
    _low = _target * (1 - _hyst/100);
    _high = _target * (1+ _hyst/100);
    
    _output = 0;
}

void Homeostat::Target(double target)
{
    _target = target;
    _low = _target * (1 - _hyst/100);
    _high = _target * (1+ _hyst/100);
}

void Homeostat::Hysteresis(double hyst)
{
    _hyst = hyst;
    _low = _target * (1 - _hyst/100);
    _high = _target * (1+ _hyst/100);
}

double Homeostat::ReadTarget()
{
    return _target;
}

double Homeostat::ReadHysteresis()
{
    return _hyst;
}

double Homeostat::ReadLow()
{
    return _low;
}

double Homeostat::ReadHigh()
{
    return _high;
}

int Homeostat::Output(double SensorValue)
{
    _SensorValue = SensorValue;
    if (_mode == 1) //1 is supression mode
    {
        if (_SensorValue >= _high) _output = 1;
        else if (_SensorValue < _low) _output = 0;
        else _output = -1;
        return _output;
        
    }
    else if (_mode == 0) 
    {
        if (_SensorValue > _high) _output = 0;
        else if (_SensorValue <= _low) _output = 1;
        else _output = -1;
        return _output;
    }
    else return -1;
}
