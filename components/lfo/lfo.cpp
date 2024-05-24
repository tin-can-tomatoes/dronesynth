#include <cmath>

#include <esp_attr.h>
#include <esp_log.h>

#include <pins.hpp>

#include "lfo.hpp"



void LFO::init(uint8_t pin, uint8_t lfo_num){
    this->pin = pin;
    this->lfo_num = lfo_num;

    // Map params

    param_period_ms =           (SynthParams) ((int)LFO_PARAM_START + (int)LFO_PARAM_PERIOD_MS  + (LFO_PARAM_MAX * (lfo_num-1)));
    param_phase =               (SynthParams) ((int)LFO_PARAM_START + (int)LFO_PARAM_PHASE      + (LFO_PARAM_MAX * (lfo_num-1)));
    param_wave_func =           (SynthParams) ((int)LFO_PARAM_START + (int)LFO_PARAM_WAVE_FUNC  + (LFO_PARAM_MAX * (lfo_num-1)));
    param_wave_midpoint_duty =  (SynthParams) ((int)LFO_PARAM_START + (int)LFO_PARAM_WAVE_MIDPOINT_DUTY + (LFO_PARAM_MAX * (lfo_num-1)));
    param_floor =               (SynthParams) ((int)LFO_PARAM_START + (int)LFO_PARAM_FLOOR              + (LFO_PARAM_MAX * (lfo_num-1)));
    param_ciel =                (SynthParams) ((int)LFO_PARAM_START + (int)LFO_PARAM_CIEL               + (LFO_PARAM_MAX * (lfo_num-1)));
    
    // Attach callbacks to params
    Param::params[param_period_ms].set_callback(this);
    Param::params[param_phase].set_callback(this);
    Param::params[param_wave_func].set_callback(this);
    Param::params[param_wave_midpoint_duty].set_callback(this);
    Param::params[param_floor].set_callback(this);
    Param::params[param_ciel].set_callback(this);


    apply_params();
    init_hw();
}

void LFO::apply_params(){

    period_ms = Param::get(param_period_ms);
    phase_ticks = round(((double) period_ms) * Param::getd(PARAM_LFO1_PHASE));
    wave_func = (LFOWaveFunc) Param::get(param_wave_func);
    wave_midpoint_duty = Param::getd(param_wave_midpoint_duty);
    wave_floor = Param::get(param_floor);
    wave_ciel = Param::get(param_ciel);
    mid_point_ms = round(((double) period_ms) * Param::getd(param_wave_midpoint_duty));

    slope_up = ((double)(wave_ciel - wave_floor)) / ((double)mid_point_ms);
    b_up = (double) wave_floor;

    slope_down = ((double)(wave_ciel - wave_floor)) / ((double)(period_ms - mid_point_ms));
    b_down = ((double)wave_ciel) - (slope_down * ((double)mid_point_ms));

    double range = ((double)(wave_ciel - wave_floor)) / 2;
    sine_x_divider = period_ms / (M_TWOPI);
    if(sine_x_divider == 0) sine_x_divider = 0.0001f;
    sine_y_multiplier = range;
    sine_add = range + wave_floor;
    
     /*
    ESP_LOGI("LFO", "LFO%d param_period_ms    : %d", lfo_num, param_period_ms);
    ESP_LOGI("LFO", "LFO%d param_wavelength   : %d", lfo_num, param_wavelength);
    ESP_LOGI("LFO", "LFO%d param_phase        : %d", lfo_num, param_phase);
    ESP_LOGI("LFO", "LFO%d param_wave_func    : %d", lfo_num, param_wave_func);
    ESP_LOGI("LFO", "LFO%d param_wave_mid...  : %d", lfo_num, param_wave_midpoint_duty);
    ESP_LOGI("LFO", "LFO%d param_floor        : %d", lfo_num, param_floor);
    ESP_LOGI("LFO", "LFO%d param_ciel         : %d", lfo_num, param_ciel);
    ESP_LOGI("LFO", "LFO%d period_ms          : %d", lfo_num, period_ms);
    ESP_LOGI("LFO", "LFO%d phase_ticks        : %d", lfo_num, phase_ticks);
    ESP_LOGI("LFO", "LFO%d actual_period_ms   : %d", lfo_num, actual_period_ms);
    ESP_LOGI("LFO", "LFO%d wave_func          : %d", lfo_num, wave_func);
    ESP_LOGI("LFO", "LFO%d wave_midpoint_duty : %f", lfo_num, wave_midpoint_duty);
    ESP_LOGI("LFO", "LFO%d wave_floor         : %d", lfo_num, wave_floor);
    ESP_LOGI("LFO", "LFO%d wave_ciel          : %d", lfo_num, wave_ciel);*/
}

void LFO::param_changed(SynthParams param, int old_int, int new_int, double old_double, double new_double){
    apply_params(); //lazy but effective
}


void IRAM_ATTR LFO::tick(uint32_t ticks){

    uint8_t y;
    switch(wave_func){
        case WAVE_FUNC_SAWTOOTH:
            y = get_sawtooth_ramp(ticks + phase_ticks);
            break;
        case WAVE_FUNC_SINE:
            y = get_sine(ticks + phase_ticks);
            break;
        default:
            y = get_square_pwm(ticks + phase_ticks);
            break;
    }
    apply_value((uint8_t)y);
}

int LFO::get_sawtooth_ramp(int x){
    x += phase_ticks;
    x = x % (period_ms);

    double fx = double(x);
    int y = 0u;
    if(x <= mid_point_ms){
        y = round((double(slope_up) * fx) + double(b_up));
    } else if (x <= period_ms){
        y = 0 - round((double(slope_down) * fx) + double(b_down));
    }
    return y;
}


int LFO::get_sine(int x){
    x += phase_ticks;
    x = x % period_ms;
    return round((sin(double(x) / sine_x_divider) * sine_y_multiplier)+sine_add);
}


int LFO::get_square_pwm(int x){
    x += phase_ticks;
    x = x % period_ms;
    return x >= mid_point_ms? wave_floor : wave_ciel;
}