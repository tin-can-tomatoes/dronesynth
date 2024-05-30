#ifndef TOMATO_LEDC_CHANNELS_H
#define TOMATO_LEDC_CHANNELS_H

#include <stdint.h>
#include "driver/ledc.h"

#define PWM_MODE LEDC_HIGH_SPEED_MODE
#define PWM_RESOLUTION LEDC_TIMER_8_BIT

/// @brief Class to abstract PWM pin functionality
/// For this LFO synth, all PWMChanels share the same timer
class PWMChannel{
    public:
        PWMChannel(uint8_t pin);
        uint8_t get_duty();
        void set_duty (uint8_t duty);
        
        uint16_t get_freq();
        void set_freq(uint16_t new_freq);
    protected: 
        volatile uint8_t duty;
        volatile uint16_t freq;

        ledc_timer_t timer_num;
        ledc_channel_t channel_num;
        
        ledc_timer_config_t timer_config;
        ledc_channel_config_t channel_config;

        
};

#endif