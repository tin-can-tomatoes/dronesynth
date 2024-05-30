#include <esp_attr.h>

#include "lfo_pwm.hpp"

void PWMLfo::init_hw(){
    pwm = new PWMChannel(pin);
    pwm->set_duty(120);
    pwm->set_freq(2000);
}

void IRAM_ATTR PWMLfo::apply_value(uint8_t value){
    pwm->set_duty(value);
}