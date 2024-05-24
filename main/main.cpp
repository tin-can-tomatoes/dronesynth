#include <cstddef>
#include <cstdio>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gptimer.h>
#include <driver/uart.h>
#include <esp_system.h>
#include <esp_log.h>

#include <midi.hpp>
#include <musical_notes.hpp>
#include <pins.hpp>
#include <synth.hpp>


TomatoSynth synth = TomatoSynth();

gptimer_handle_t timer_handle;
gptimer_config_t timer_config;
gptimer_event_callbacks_t timer_callbacks;
gptimer_alarm_config_t timer_alarm_config;


#define BUF_SIZE (1024)

/// @brief Timer callback for the synth. Called during interrupt every 1/1000th of a second, this is what actually makes the LFOs and ADSR work.
/// @attention Friendly reminder that the FPU does not work during interrupt service routines, therefore avoid "float" data types in favor of "double"
static bool IRAM_ATTR  timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void* user_data){
    synth.tick();
    return false;
}

extern "C" {

    void app_main(void)
    {
        // Init timers
        init_pins();
        synth.init();
        

        // Init timers
        timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;
        timer_config.direction = GPTIMER_COUNT_UP;
        timer_config.resolution_hz = 10000; // 10kHz, 0.1ms
        timer_config.flags.intr_shared = 1;
        timer_config.intr_priority = 0;
        ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer_handle));

        timer_callbacks.on_alarm = timer_cb;

        ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_handle,&timer_callbacks, NULL));

        ESP_ERROR_CHECK(gptimer_enable(timer_handle));
        
        timer_alarm_config.alarm_count = 5; // alarm is every 10 timer ticks, so 10kHz/10 = 1kHz = 1ms
        timer_alarm_config.flags.auto_reload_on_alarm=true;

        ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_handle, &timer_alarm_config));
        ESP_ERROR_CHECK(gptimer_start(timer_handle));
        
        //vTaskDelete(NULL);
        while (true)    
        {
            Param::dump();
            vTaskDelay(10000/portTICK_PERIOD_MS);
        }

        
    }

}
