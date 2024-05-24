#ifndef TOMATO_OSCILLATOR_H
#define TOMATO_OSCILLATOR_H

#include <stddef.h>
#include <stdint.h>

#include <ledc_channels.hpp>
#include <params.hpp>

#define OSC_PARAM_START PARAM_OSC1_PITCH_RAW

enum OscillatorParam {
    OSC_PARAM_PITCH_RAW = 0,
    OSC_PARAM_PWM_DUTY = 1,
    OSC_PARAM_PITCH_MODE = 2,
    OSC_PARAM_MAX
};

#define OSC_PARAM_MULTIPLIER ((int)OSC_PARAM_MAX)

enum OscillatorNum{
    OSCILLATOR_1 = 1,
    OSCILLATOR_2 = 2,
    OSCILLATOR_3 = 3,
    OSCILLATOR_4 = 4
};

class Oscillator : public ParamCallback{
    public:
        virtual void param_changed(SynthParams param, int old_int, int new_int, double old_double, double new_double);

        void init(OscillatorNum osc_num, uint8_t pin);
    private:
        uint8_t pin;

        PWMChannel* pwm_channel;

        volatile uint8_t duty;
        volatile int16_t pitch_raw;
        volatile OscillatorPitchMode pitch_mode;
        volatile size_t last_value;

        SynthParams param_duty;
        SynthParams param_pitch_raw;
        SynthParams param_pitch_mode;

        void apply_pitch();
        void apply_duty();
};

#endif