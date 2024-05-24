#ifndef TOMATO_ANALOG_LFO_H
#define TOMATO_ANALOG_LFO_H

#include <driver/dac_oneshot.h>

#include "lfo.hpp"

class AnalogLfo : public LFO{
    public:


    protected:

        virtual void init_hw();
        virtual void apply_value(uint8_t value);
    private:
        dac_oneshot_handle_t dac_handle;
        dac_oneshot_config_t dac_conf;
        
};
#endif