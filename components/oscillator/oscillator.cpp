#include <esp_log.h>

#include <musical_notes.hpp>
#include <pins.hpp>

#include "oscillator.hpp"

#define OSCILLATOR_MIN_HZ 40
#define OSCILLTATOR_MIN_DUTY 2

void Oscillator::init(OscillatorNum n, uint8_t pin){
    // Set up the output PWM channel
    this->pin = pin;
    pwm_channel = new PWMChannel(pin);

    // Figure out which params apply to this numbered oscillator
    param_pitch_raw  = (SynthParams) ((int)OSC_PARAM_START + (int)OSC_PARAM_PITCH_RAW  + (OSC_PARAM_MAX * (n-1)));
    param_duty       = (SynthParams) ((int)OSC_PARAM_START + (int)OSC_PARAM_PWM_DUTY   + (OSC_PARAM_MAX * (n-1)));
    param_pitch_mode = (SynthParams) ((int)OSC_PARAM_START + (int)OSC_PARAM_PITCH_MODE + (OSC_PARAM_MAX * (n-1)));

    // Read the values of the params (should be the defaults at this point)
    pitch_raw = Param::get(param_pitch_raw);
    duty = Param::get(param_duty);
    pitch_mode = (OscillatorPitchMode) Param::get(param_pitch_mode);


    last_value = 0;

    // Get some sound going
    apply_pitch();
    apply_duty();

    // Attach callbacks to the parameters so that this oscillator gets notified 
    // when things change
    Param::params[param_pitch_mode].set_callback(this);
    Param::params[param_duty].set_callback(this);
    Param::params[param_pitch_raw].set_callback(this);
}


void Oscillator::param_changed(SynthParams param, int old_int, int new_int, double old_double, double new_double){

   if(param == param_duty){
        if(new_int < OSCILLTATOR_MIN_DUTY) new_int = OSCILLTATOR_MIN_DUTY;
        duty = new_int;
        apply_duty();
   } else if (param == param_pitch_raw) {
        pitch_raw = new_int;
        apply_pitch();
   } else if (param == param_pitch_mode) {
        pitch_mode = (OscillatorPitchMode) new_int;
        apply_pitch();
   }
}

void Oscillator::apply_duty(){
    pwm_channel->set_duty(duty);
}

void Oscillator::apply_pitch(){
    uint16_t freq;
    switch(pitch_mode){
        case OSCILLATOR_MODE_NOTE:
            freq = musical_notes[map(pitch_raw, 0, 4095, MUSICAL_NOTE_MIN, MUSICAL_NOTE_MAX)];
            break;
        case OSCILLATOR_MODE_HALFHZ:
            freq = (pitch_raw / 2) + OSCILLATOR_MIN_HZ;
            break;
        case OSCILLATOR_MODE_1HZ:
            freq = pitch_raw + OSCILLATOR_MIN_HZ;
            break;
        case OSCILLATOR_MODE_2HZ:
            freq = (pitch_raw * 2) + OSCILLATOR_MIN_HZ;
            break;
        default:
            freq = (pitch_raw * 3) + OSCILLATOR_MIN_HZ;
            break;
    }

    //if(freq != last_value){
        pwm_channel->set_freq(freq);
        last_value = freq;
    //}
}
