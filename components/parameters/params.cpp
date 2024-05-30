#include <cmath>
#include <cstdio>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <pins.hpp>

#include "params.hpp"

const char* paramnames[] = {
"PARAM_ZERO",
"PARAM_SYNTH_MIDI_CHANNEL",
"PARAM_SYNTH_BPM",
"PARAM_LFO1_PERIOD_CS",
"PARAM_LFO1_PHASE",
"PARAM_LFO1_WAVE_FUNC",
"PARAM_LFO1_WAVE_MIDPOINT_DUTY",
"PARAM_LFO1_FLOOR",
"PARAM_LFO1_CIEL",
"PARAM_LFO2_PERIOD_CS",
"PARAM_LFO2_PHASE",
"PARAM_LFO2_WAVE_FUNC",
"PARAM_LFO2_WAVE_MIDPOINT_DUTY",
"PARAM_LFO2_FLOOR",
"PARAM_LFO2_CIEL",
"PARAM_LFO3_PERIOD_CS",
"PARAM_LFO3_PHASE",
"PARAM_LFO3_WAVE_FUNC",
"PARAM_LFO3_WAVE_MIDPOINT_DUTY",
"PARAM_LFO3_FLOOR",
"PARAM_LFO3_CIEL",
"PARAM_LFO4_PERIOD_CS",
"PARAM_LFO4_PHASE",
"PARAM_LFO4_WAVE_FUNC",
"PARAM_LFO4_WAVE_MIDPOINT_DUTY",
"PARAM_LFO4_FLOOR",
"PARAM_LFO4_CIEL",
};

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Initialize the params:
Param Param::params[PARAM_MAX] = {
    // TODO: Change LFOs to Centiseconds!
    Param( PARAM_ZERO,0,0,1), 
    Param( PARAM_SYNTH_MIDI_CHANNEL,                        0,                       0,                   15, 121),
    Param( PARAM_SYNTH_BPM,                               120,                       1,                  600),
    Param( PARAM_LFO1_PERIOD_CS,                           50,                       10,                 200, 20),
    Param( PARAM_LFO1_PHASE,                                0,                        0,                 360, 21),
    Param( PARAM_LFO1_WAVE_FUNC,          (int)WAVE_FUNC_SINE,                        0,    WAVE_FUNC_SQUARE, 22),
    Param( PARAM_LFO1_WAVE_MIDPOINT_DUTY,                 100,                        0,                1000, 23),
    Param( PARAM_LFO1_FLOOR,                                0,                        0,                 255, 24),
    Param( PARAM_LFO1_CIEL,                               255,                        0,                 255, 25),
    Param( PARAM_LFO2_PERIOD_CS,                           50,                       10,                 200, 26),
    Param( PARAM_LFO2_PHASE,                                0,                        0,                 360, 27),
    Param( PARAM_LFO2_WAVE_FUNC,      (int)WAVE_FUNC_SAWTOOTH,                        0,    WAVE_FUNC_SQUARE, 28),
    Param( PARAM_LFO2_WAVE_MIDPOINT_DUTY,                 200,                        0,                1000, 29),
    Param( PARAM_LFO2_FLOOR,                                0,                        0,                 255, 30),
    Param( PARAM_LFO2_CIEL,                               255,                        0,                 255, 31),
    Param( PARAM_LFO3_PERIOD_CS,                           50,                       10,                 200, 102),
    Param( PARAM_LFO3_PHASE,                                0,                        0,                 360, 103),
    Param( PARAM_LFO3_WAVE_FUNC,        (int)WAVE_FUNC_SQUARE,                        0,    WAVE_FUNC_SQUARE, 104),
    Param( PARAM_LFO3_WAVE_MIDPOINT_DUTY,                 200,                        0,                1000, 105),
    Param( PARAM_LFO3_FLOOR,                                0,                        0,                 255, 106),
    Param( PARAM_LFO3_CIEL,                               255,                        0,                 255, 107),
    Param( PARAM_LFO4_PERIOD_CS,                           50,                       10,                 200, 108),
    Param( PARAM_LFO4_PHASE,                                0,                        0,                 360, 109),
    Param( PARAM_LFO4_WAVE_FUNC,      (int)WAVE_FUNC_SAWTOOTH,                        0,    WAVE_FUNC_SQUARE, 110),
    Param( PARAM_LFO4_WAVE_MIDPOINT_DUTY,                 200,                        0,                1000, 111),
    Param( PARAM_LFO4_FLOOR,                                0,                        0,                 255, 112),
    Param( PARAM_LFO4_CIEL,                               255,                        0,                 255, 113),

};

void Param::dump(){
    for(int i = 0; i< PARAM_MAX; ++i){
        ESP_LOGI("Param",
        "%32s: int: %6d double: %f, min: %6d, max: %6d",
        paramnames[i],params[i].value_int,params[i].value_double,
        params[i].value_min,params[i].value_max);
    }
}

int Param::get(SynthParams param){
    return Param::params[param].value_int;
}

double Param::getd(SynthParams param){
    return Param::params[param].value_double;
}

void Param::set(SynthParams param, int value){
    Param::params[param].set_int(value);
}
void Param::set(SynthParams param, double value){
    Param::params[param].set_double(value);
}

void Param::set_midi(SynthParams param, signed char value){
    Param::params[param].set_midi(value);
}

void Param::set_from_midi(signed char midi_cc, signed char midi_value){
    if(midi_cc < 0) return;
    if(midi_value < 0) return;

    for(int p = 0; p < PARAM_MAX; ++p){
        if(params[p].midi_cc == midi_cc){
            params[p].set_midi(midi_value);
        }
    }
}


Param::Param    (SynthParams        param,
                 int          default_val,
                 int            value_min,
                 int            value_max)
{
    this->midi_cc = -1;
    this->value_int = default_val;
    this->value_double = double(default_val-value_min) / double(value_max - value_min);
    this->param = param;
    this->value_min = value_min;
    this->value_max = value_max;
    this->last_cc_value = 255u;
    this->default_value = default_val;
    this->callback = NULL;
}


Param::Param    (SynthParams       param,
                 int         default_val,
                 int           value_min,
                 int           value_max,
                 signed char     midi_cc)
{
    this->midi_cc = midi_cc;
    this->value_int = default_val;
    this->value_double =  double(default_val-value_min) / double(value_max - value_min);
    this->param = param;
    this->value_min = value_min;
    this->value_max = value_max;
    this->last_cc_value = 255u;
    this->default_value = default_val;
    this->callback = NULL;
}

int Param::get_int(){
    return value_int;
}

double Param::get_double(){
    return value_double;
}

void Param::set_int(int value)
{
    if(this->value_int != value)
    {
        double old_double = this->value_double;
        int old_int = this->value_int;
        if(value > value_max) value = value_max;
        if(value < value_min) value = value_min;
        this->value_int = value;
        this->value_double = ((double)(value - value_min)) / ((double)(value_max - value_min));
        if(callback != NULL){
            callback->param_changed(this->param, old_int, value, old_double , this->value_double);
        }
    }
}

void Param::set_double(double value){
    if(this->value_double != value){
        if(value < 0.0f) value = 0.0f;
        if(value > 1.0f) value = 1.0f;
        int scale = value_max - value_min;
        double old_double = this->value_double;
        int old_int = this->value_int;
        this->value_double = value;
        this->value_int = ((int)roundf(value * ((double)scale))) + value_min;
        if(callback != NULL){
            callback->param_changed(this->param, old_int, this->value_int, old_double, value);
        }
    }
}

void Param::set_midi(signed char value){
    if(value >= 0 && value != last_cc_value){
        set_int(map(value,0,127, value_min, value_max));
        last_cc_value = (unsigned char) value; 
    }
}

void Param::set_callback(ParamCallback* cb){
    this->callback = cb;
}

