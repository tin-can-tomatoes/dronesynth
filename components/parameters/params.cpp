#include <cmath>
#include <cstdio>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <musical_notes.hpp>
#include <pins.hpp>

#include "params.hpp"

const char* paramnames[] = {
"PARAM_ZERO",
"PARAM_SYNTH_MIDI_CHANNEL",
"PARAM_SYNTH_BPM",
"PARAM_OSC1_PITCH_RAW",
"PARAM_OSC1_PWM_DUTY",
"PARAM_OSC1_PITCH_MODE",
"PARAM_OSC2_PITCH_RAW",
"PARAM_OSC2_PWM_DUTY",
"PARAM_OSC2_PITCH_MODE",
"PARAM_OSC3_PITCH_RAW",
"PARAM_OSC3_PWM_DUTY",
"PARAM_OSC3_PITCH_MODE",
"PARAM_OSC4_PITCH_RAW",
"PARAM_OSC4_PWM_DUTY",
"PARAM_OSC4_PITCH_MODE",
"PARAM_LFO1_PERIOD_MS",
"PARAM_LFO1_PHASE",
"PARAM_LFO1_WAVE_FUNC",
"PARAM_LFO1_WAVE_MIDPOINT_DUTY",
"PARAM_LFO1_FLOOR",
"PARAM_LFO1_CIEL",
"PARAM_LFO2_PERIOD_MS",
"PARAM_LFO2_PHASE",
"PARAM_LFO2_WAVE_FUNC",
"PARAM_LFO2_WAVE_MIDPOINT_DUTY",
"PARAM_LFO2_FLOOR",
"PARAM_LFO2_CIEL",
};

TaskHandle_t input_watcher_task_handle;

static void input_watcher_task(void* ptr){
    Param::poll_inputs();
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Initialize the params:
Param Param::params[PARAM_MAX] = {
    Param( PARAM_ZERO,0,0,1), 
    Param( PARAM_SYNTH_MIDI_CHANNEL,                        0,                       0,                   15),
    Param( PARAM_SYNTH_BPM,                               120,                       1,                  600),
    Param( PARAM_OSC1_PITCH_RAW,                           80,                       0,                 4095, -1, KNOB_OSC1_PITCH),
    Param( PARAM_OSC1_PWM_DUTY,                            15,                       0,                   30, -1, KNOB_OSC1_DUTY),
    Param( PARAM_OSC1_PITCH_MODE,        OSCILLATOR_MODE_NOTE,    OSCILLATOR_MODE_NOTE,  OSCILLATOR_MODE_3HZ),
    Param( PARAM_OSC2_PITCH_RAW,                           80,                        0,                4095, -1, KNOB_OSC2_PITCH),
    Param( PARAM_OSC2_PWM_DUTY,                            15,                        0,                  30, -1, KNOB_OSC2_DUTY),
    Param( PARAM_OSC2_PITCH_MODE,        OSCILLATOR_MODE_NOTE,    OSCILLATOR_MODE_NOTE,  OSCILLATOR_MODE_3HZ),
    Param( PARAM_OSC3_PITCH_RAW,                           80,                        0,                4095, -1, KNOB_OSC3_PITCH),
    Param( PARAM_OSC3_PWM_DUTY,                            15,                        0,                  30, -1, KNOB_OSC3_DUTY),
    Param( PARAM_OSC3_PITCH_MODE,        OSCILLATOR_MODE_NOTE,    OSCILLATOR_MODE_NOTE,  OSCILLATOR_MODE_3HZ),
    Param( PARAM_OSC4_PITCH_RAW,                           80,                        0,                4095, -1, KNOB_OSC4_PITCH),
    Param( PARAM_OSC4_PWM_DUTY,                            15,                        0,                  30, -1, KNOB_OSC4_DUTY),
    Param( PARAM_OSC4_PITCH_MODE,        OSCILLATOR_MODE_NOTE,    OSCILLATOR_MODE_NOTE,  OSCILLATOR_MODE_3HZ),
    Param( PARAM_LFO1_PERIOD_MS,                         1000,                      500,                 3000, -1, KNOB_LFO1_RATE),
    Param( PARAM_LFO1_PHASE,                                0,                        0,                 360),
    Param( PARAM_LFO1_WAVE_FUNC,      (int)WAVE_FUNC_SINE,                        0,    WAVE_FUNC_SQUARE),
    Param( PARAM_LFO1_WAVE_MIDPOINT_DUTY,                 100,                        0,                1000),
    Param( PARAM_LFO1_FLOOR,                                0,                        0,                 255),
    Param( PARAM_LFO1_CIEL,                               255,                        0,                 255),
    Param( PARAM_LFO2_PERIOD_MS,                         1000,                      500,                 3000, -1, KNOB_LFO2_RATE),
    Param( PARAM_LFO2_PHASE,                                0,                        0,                 360),
    Param( PARAM_LFO2_WAVE_FUNC,      (int)WAVE_FUNC_SINE,                        0,    WAVE_FUNC_SQUARE),
    Param( PARAM_LFO2_WAVE_MIDPOINT_DUTY,                 500,                        0,                1000),
    Param( PARAM_LFO2_FLOOR,                                0,                        0,                 255),
    Param( PARAM_LFO2_CIEL,                               255,                        0,                 255),

};

void Param::poll_inputs(){
    unsigned int new_raw;
    int i;
    while(true){
        for(i = 0; i < PARAM_MAX; ++i){
            if(!params[i].input_pin) continue;

            new_raw = analogRead(params[i].input_pin);

            if(new_raw != params[i].last_input_value){
              
                params[i].last_input_value = new_raw;
                if(params[i].value_min == 0 && params[i].value_max == 4095){
                    params[i].set_int(new_raw);
                } else {
                    int new_value = map(new_raw,0,4095,params[i].value_min, params[i].value_max);
                    if(params[i].value_int != new_value){
                        params[i].set_int(new_value);
                    }
                }
            }
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void Param::start_watching_inputs(){

    xTaskCreatePinnedToCore(&input_watcher_task, "input_watcher_task", 1024u, NULL, 3, &input_watcher_task_handle, 1);
}

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





Param::Param    (SynthParams        param,
                 int          default_val,
                 int            value_min,
                 int            value_max)
{
    this->input_pin = 0;
    this->midi_cc = -1;
    this->value_int = default_val;
    this->value_double = double(default_val-value_min) / double(value_max - value_min);
    this->param = param;
    this->value_min = value_min;
    this->value_max = value_max;
    this->last_input_value = 0;
    this->default_value = default_val;
    this->callback = NULL;
}


Param::Param    (SynthParams       param,
                 int         default_val,
                 int           value_min,
                 int           value_max,
                 signed char     midi_cc)
{
    this->input_pin = 0;
    this->midi_cc = midi_cc;
    this->value_int = default_val;
    this->value_double =  double(default_val-value_min) / double(value_max - value_min);
    this->param = param;
    this->value_min = value_min;
    this->value_max = value_max;
    this->last_input_value = 0;
    this->default_value = default_val;
    this->callback = NULL;
}

Param::Param    (SynthParams       param,
                 int         default_val,
                 int           value_min,
                 int           value_max,
                 signed char     midi_cc,
                 unsigned char input_pin)
{
    this->input_pin = input_pin;
    this->midi_cc = midi_cc;
    this->value_int = default_val;
    this->value_double =  double(default_val-value_min) / double(value_max - value_min);
    this->param = param;
    this->value_min = value_min;
    this->value_max = value_max;
    this->last_input_value = 0;
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
    if(value >= 0){
        set_int(map(value,0,127, value_min, value_max));
    }
}

void Param::set_callback(ParamCallback* cb){
    this->callback = cb;
}

