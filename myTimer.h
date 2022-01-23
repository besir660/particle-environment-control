#include "Particle.h"

#ifndef Timer_h
#define Timer_h


class myTimer
{
    public:
        myTimer(int interval);
        bool trigger();
    private:
        int _interval;
        int _trigger_sent;
};

#endif
