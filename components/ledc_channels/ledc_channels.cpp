#include "driver/ledc.h"
#include "esp_attr.h"
#include "esp_err.h"

#include "ledc_channels.hpp"


PWMChannel::PWMChannel(uint8_t pin){
    static size_t timer_idx = 0;
    if(timer_idx >= LEDC_TIMER_MAX || timer_idx >= LEDC_CHANNEL_MAX)
    {
        exit(EXIT_FAILURE);
    }

    timer_num = (ledc_timer_t)timer_idx;
    channel_num = (ledc_channel_t)timer_idx;
    
    ++ timer_idx;

    timer_config = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = PWM_RESOLUTION,
        .timer_num = timer_num,
        .freq_hz = 440,
        .clk_cfg = LEDC_USE_REF_TICK,
        .deconfigure = false
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    channel_config = {
        .gpio_num = pin,
        .speed_mode = PWM_MODE,
        .channel = channel_num,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer_num,
        .duty = 5,
        .hpoint = 0,
        .flags = { .output_invert = 0 }
    };
    ESP_ERROR_CHECK(ledc_set_pin(pin,LEDC_HIGH_SPEED_MODE, channel_num));
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
}


uint8_t PWMChannel::get_duty(){
    return duty;
}

void IRAM_ATTR PWMChannel::set_duty (uint8_t duty){
    this->duty = duty;

    ESP_ERROR_CHECK(ledc_set_duty(PWM_MODE,channel_num,duty));
    ESP_ERROR_CHECK(ledc_update_duty(PWM_MODE,channel_num));
}

uint16_t PWMChannel::get_freq(){
    return freq;
}

void IRAM_ATTR PWMChannel::set_freq(uint16_t new_freq){
    this->freq = new_freq;
    ESP_ERROR_CHECK(ledc_set_freq(PWM_MODE,timer_num,new_freq));
}