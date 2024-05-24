#ifndef TOMATO_LFO_H
#define TOMATO_LFO_H

#include <cstdint>

#include "params.hpp"

#define LFO_PARAM_START PARAM_LFO1_PERIOD_MS

enum LfoParam {
    LFO_PARAM_PERIOD_MS = 0,
    LFO_PARAM_PHASE,
    LFO_PARAM_WAVE_FUNC,
    LFO_PARAM_WAVE_MIDPOINT_DUTY,
    LFO_PARAM_FLOOR,
    LFO_PARAM_CIEL,
    LFO_PARAM_MAX
};

#define LFO_PARAM_MULTIPLIER ((int)LFO_PARAM_MAX)

class LFO : public ParamCallback {
    public:

        void param_changed(SynthParams param, int old_int, int new_int, double old_double, double new_double);

        void init(uint8_t pin, uint8_t lfo_num);
        void tick(uint32_t ticks);

    protected:
        virtual void apply_value(uint8_t value) = 0;
        virtual void init_hw() = 0;

        void apply_params();
        int get_sawtooth_ramp(int x);
        int get_sine(int x);
        int get_square_pwm(int x);

        uint8_t pin;
        uint8_t lfo_num;
        int period_ms;
        LFOWaveFunc wave_func;
        double wave_midpoint_duty;
        int wave_floor;
        int wave_ciel;

        // Internal mechanics:
        int phase_ticks;
        int mid_point_ms;
        double slope_up;
        double slope_down;
        double b_up;
        double b_down;
        double sine_x_divider;
        double sine_y_multiplier;
        double sine_add;
        

        SynthParams param_period_ms;
        SynthParams param_phase;
        SynthParams param_wave_func;
        SynthParams param_wave_midpoint_duty;
        SynthParams param_floor;
        SynthParams param_ciel;
};
#endif