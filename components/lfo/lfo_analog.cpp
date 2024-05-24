#include <driver/dac_oneshot.h>
#include <esp_attr.h>

#include "lfo_analog.hpp"

void AnalogLfo::init_hw(){
    static bool first = true;
    dac_conf.chan_id = first? DAC_CHAN_0 : DAC_CHAN_1;
    first = false;
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac_conf, &dac_handle));

}

void IRAM_ATTR AnalogLfo::apply_value(uint8_t value){
    ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_handle, value));
}