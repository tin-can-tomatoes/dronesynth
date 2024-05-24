#ifndef TOMATO_ADSR_H
#define TOMATO_ADSR_H


#include <stdint.h>

#include <driver/dac_oneshot.h>

#define TOMATO_ADSR_IDLE 0u
#define TOMATO_ADSR_ATTACK 1u
#define TOMATO_ADSR_DECAY 2u
#define TOMATO_ADSR_SUSTAIN 3u
#define TOMATO_ADSR_RELEASE 4u


/// @brief ADSR class
/// @attention THIS IS COMPLETELY UNFINISHED!!!!!!
class ADSR {
    public:
        void init                  (uint8_t           pin);
        void tick(uint32_t ticks);
        void trigger_on            (uint8_t      velocity);
        void trigger_off();


    private:
        uint8_t                      pin;

        // Internal mechanics
        volatile uint8_t           state; // 0 - idle, 1 - attack, 2 - decay, 3 - sustain, 4 - release
        volatile uint16_t  current_level;
        volatile uint8_t        velocity;


        dac_oneshot_handle_t dac_handle;
        dac_oneshot_config_t dac_conf;
};

#endif