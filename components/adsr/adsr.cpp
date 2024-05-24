/// @attention THIS IS COMPLETELY UNFINISHED!!!!!!

#include <driver/dac_oneshot.h>


#include "adsr.hpp"


#define DAC_CH DAC_CHANNEL_2


void
ADSR::init             (uint8_t         pin)
{
    this->pin = pin;


    dac_conf.chan_id = DAC_CHAN_0;
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac_conf, &dac_handle));
}


void IRAM_ATTR ADSR::tick(uint32_t ticks){
    ;
}

void IRAM_ATTR ADSR::trigger_on(uint8_t velocity){
        ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_handle, (uint8_t) 255));

}

void IRAM_ATTR ADSR::trigger_off(){
        ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_handle, (uint8_t) 0));

}

