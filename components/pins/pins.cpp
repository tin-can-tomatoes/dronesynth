
#include <cstdint>

#include <esp_adc/adc_oneshot.h>

#include "pins.hpp"

static adc_oneshot_unit_handle_t adc1_handle;
static adc_oneshot_unit_handle_t adc2_handle;

/// @brief Utility function to retreive the ADC channel number for any given physical pin
/// @param pin_number GPIO pin number
/// @return ADC Channel Number
/// @see get_unit_for_pin
static adc_channel_t get_channel_for_pin(uint8_t pin_number){
    switch (pin_number)
    {
        case ADC1_CH0_PIN: return ADC_CHANNEL_0;
        case ADC1_CH3_PIN: return ADC_CHANNEL_3;
        case ADC1_CH4_PIN: return ADC_CHANNEL_4;
        case ADC1_CH5_PIN: return ADC_CHANNEL_5;
        case ADC1_CH6_PIN: return ADC_CHANNEL_6;
        case ADC1_CH7_PIN: return ADC_CHANNEL_7;
        case ADC2_CH0_PIN: return ADC_CHANNEL_0;
        case ADC2_CH1_PIN: return ADC_CHANNEL_1;
        case ADC2_CH2_PIN: return ADC_CHANNEL_2;
        case ADC2_CH3_PIN: return ADC_CHANNEL_3;
        case ADC2_CH4_PIN: return ADC_CHANNEL_4;
        case ADC2_CH5_PIN: return ADC_CHANNEL_5;
        case ADC2_CH6_PIN: return ADC_CHANNEL_6;
        case ADC2_CH7_PIN: return ADC_CHANNEL_7;
        default: return ADC_CHANNEL_8;// oops lol
    }
}

/// @brief Utility function to retreive the ADC unit number for any given physical pin
/// @param pin_number GPIO pin number
/// @return ADC Unit number
static adc_oneshot_unit_handle_t get_unit_for_pin(uint8_t pin_number){
    switch (pin_number)
    {
        case ADC1_CH0_PIN: 
        case ADC1_CH3_PIN: 
        case ADC1_CH4_PIN: 
        case ADC1_CH5_PIN: 
        case ADC1_CH6_PIN: 
        case ADC1_CH7_PIN: 
            return adc1_handle;
        case ADC2_CH0_PIN: 
        case ADC2_CH1_PIN: 
        case ADC2_CH2_PIN: 
        case ADC2_CH3_PIN: 
        case ADC2_CH4_PIN: 
        case ADC2_CH5_PIN: 
        case ADC2_CH6_PIN: 
        case ADC2_CH7_PIN: 
            return adc2_handle;
        default: return NULL;// oops lol
    }
}


void init_pins(){
    adc_oneshot_unit_init_cfg_t init_config1 = {
            .unit_id = ADC_UNIT_1,
            .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
            .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    adc_oneshot_unit_init_cfg_t init_config2 = {
            .unit_id = ADC_UNIT_2,
            .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
            .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    adc_oneshot_chan_cfg_t chanconfig = {
        
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1,&adc1_handle));
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config2,&adc2_handle));
    // ADC1 Channel Init
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, (adc_channel_t)KNOB_CH_OSC1_PITCH, &chanconfig));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, (adc_channel_t)KNOB_CH_OSC1_DUTY, &chanconfig));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, (adc_channel_t)KNOB_CH_OSC2_PITCH, &chanconfig));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, (adc_channel_t)KNOB_CH_OSC2_DUTY, &chanconfig));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, (adc_channel_t)KNOB_CH_OSC3_PITCH, &chanconfig));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, (adc_channel_t)KNOB_CH_OSC3_DUTY, &chanconfig));
    // ADC2 Channel Init
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, (adc_channel_t)KNOB_CH_OSC4_PITCH, &chanconfig));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, (adc_channel_t)KNOB_CH_OSC4_DUTY, &chanconfig));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, (adc_channel_t)KNOB_CH_LFO1_RATE, &chanconfig));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, (adc_channel_t)KNOB_CH_LFO2_RATE, &chanconfig));
}

int analogRead(unsigned char pin){
    adc_channel_t channel = get_channel_for_pin(pin);
    adc_oneshot_unit_handle_t handle = get_unit_for_pin(pin);
    int adc_raw = -1;
    ESP_ERROR_CHECK(adc_oneshot_read(handle,channel, &adc_raw));
    return adc_raw;
}
