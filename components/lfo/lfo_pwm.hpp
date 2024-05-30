#ifndef TOMATO_PWM_LFO_H
#define TOMATO_PWM_LFO_H

#include <ledc_channels.hpp>

#include "lfo.hpp"

class PWMLfo : public LFO{
    protected:

        virtual void init_hw();
        virtual void apply_value(uint8_t value);
    private:
        PWMChannel* pwm;
};
#endif