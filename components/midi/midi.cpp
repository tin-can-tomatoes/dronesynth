#include <driver/uart.h>
#include <esp_system.h>
#include <esp_log.h>
#include "midi.hpp"
#define MIDI_BUF_SIZE 4
#define MIDI_UART_FIFO_SIZE 2048
#define MIDI_UART UART_NUM_1

uint8_t Midi::rx_pin = 0;
uint8_t Midi::tx_pin = 0;
MidiCallbackHandler* Midi::handler = nullptr;
TaskHandle_t Midi::midi_task_handle = 0;

static void midi_rx_task(void* params){
    Midi::rx_loop();

}



void Midi::init(uint8_t rx_pin, uint8_t tx_pin, MidiCallbackHandler* handler){
    Midi::handler = handler;
    Midi::rx_pin = rx_pin;
    Midi::tx_pin = tx_pin;

    uart_config_t uart_config = {
        .baud_rate = 31250,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 3,
        .source_clk = UART_SCLK_DEFAULT
    };

    int intr_alloc_flags = 0;
#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(MIDI_UART, MIDI_UART_FIFO_SIZE, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(MIDI_UART, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(MIDI_UART, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    xTaskCreatePinnedToCore(&midi_rx_task, "midi_rx_task", 1024u, NULL, 3, &midi_task_handle, 1);
    // xTaskCreate(&midi_rx_task, "midi_rx_task", 2048, NULL, 5, &midi_task_handle);
}



void Midi::rx_loop(){
    uint8_t*  buf =  (uint8_t *) malloc(MIDI_BUF_SIZE);
    size_t bytes_to_read = 0;
    int bytes_read;
    size_t bytes_left_to_read = 0;
    uint8_t last_status_byte;
    while(true){
        ESP_ERROR_CHECK(uart_get_buffered_data_len(MIDI_UART, &bytes_to_read));

        while(bytes_to_read){
            bytes_read = uart_read_bytes(MIDI_UART,buf, 1,500);
            if(bytes_read <= 0){
                ESP_ERROR_CHECK(uart_get_buffered_data_len(MIDI_UART, &bytes_to_read));
                continue;
            }

            if(Midi::is_status_byte(buf[0])){  
                last_status_byte = buf[0];
                if(Midi::is_sysex_start(last_status_byte)){
                    Midi::ignore_sysex();
                } else {
                    bytes_left_to_read = get_expected_bytes_count(last_status_byte);
                    bytes_read = uart_read_bytes(MIDI_UART,buf, bytes_left_to_read,500);
                    if(bytes_read == bytes_left_to_read){
                        dispatch_message(last_status_byte, buf[0], buf[1]);
                    } else {
                        ESP_LOGW("MIDI", "Expected to read %d bytes, only received %d", bytes_read, bytes_left_to_read);
                    }
                }
            } 
            
            ESP_ERROR_CHECK(uart_get_buffered_data_len(MIDI_UART, &bytes_to_read));
        }
        vTaskDelay(20/portTICK_PERIOD_MS);

    }
}



void Midi::dispatch_message(uint8_t status, uint8_t data1, uint8_t data2){

    uint8_t command = status & MIDI_STATUS_COMMAND_MASK;
    uint8_t low_nibble  = status & MIDI_STATUS_CHANNEL_MASK;
    MidiChannel channel = (MidiChannel)(low_nibble);
    MidiAdditionalStatusType  st = (MidiAdditionalStatusType) low_nibble;
    uint16_t wide_char;
    switch(command){
        case MIDI_NOTE_OFF:
            handler->handle_note_off(channel, data1, data2);
            return;
        case MIDI_NOTE_ON:
            handler->handle_note_on(channel, data1, data2);
            return;
        case MIDI_KEY_PRESSURE:
            handler->handle_key_pressue(channel, data1, data2);
            return;
        case MIDI_CONTROLLER_CHANGE:
            handler->handle_controller_change(channel, data1, data2);
            return;
        case MIDI_PROGRAM_CHANGE:
            handler->handle_program_change(channel,data1);
            return;
        case MIDI_CHANNEL_PRESSURE:
            handler->handle_channel_pressure(channel, data1);
            return;
        case MIDI_PITCH_BEND:
            wide_char = (((uint16_t)data2) << 7) | ((uint16_t) data1);
            handler->handle_pitch_bend(channel, wide_char);
            return;
    }
    // If we're still here it's one of the F-prefixed status bytes.
    switch(st){
        case MIDI_F_SYSEX: // ignore
        case MIDI_F_END_OF_SYSEX:
            return;
        case MIDI_F_SONG_POSITION:
            wide_char = (((uint16_t)data2) << 4) | ((uint16_t) data1);
            handler->handle_song_position(wide_char);
            return;
        case MIDI_F_SONG_SELECT:
            handler->handle_song_select(data1);
            return;
        case MIDI_F_BUS_SELECT_UNOFFICIAL:
            handler->handle_bus_select(data1);
            return;
        case MIDI_F_TUNE_REQUEST:
            handler->handle_tune_request();
            return;
        case MIDI_F_TIMING_TICK:
            handler->handle_timing_tick();
            return;
        case MIDI_F_START_SONG:
            handler->handle_start_song();
            return;
        case MIDI_F_CONTINUE_SONG:
            handler->handle_continue_song();
            return;
        case MIDI_F_STOP_SONG:
            handler->handle_stop_song();
            return;
        case MIDI_F_ACTIVE_SENSING:
            handler->handle_active_sensing();
            return;
        case MIDI_F_SYSTEM_RESET:
            handler->handle_system_reset();
            return;
    }
}



size_t Midi::get_expected_bytes_count(uint8_t status){
    uint8_t high_nibble = status & MIDI_STATUS_COMMAND_MASK;
    uint8_t low_nibble = status & MIDI_STATUS_CHANNEL_MASK;
    switch(high_nibble){
        case MIDI_NOTE_OFF:
        case MIDI_NOTE_ON:
        case MIDI_KEY_PRESSURE:
        case MIDI_CONTROLLER_CHANGE:
        case MIDI_PITCH_BEND:
            return 2;
        case MIDI_PROGRAM_CHANGE:
        case MIDI_CHANNEL_PRESSURE:
            return 1;
    }
    if(high_nibble != MIDI_ADDITIONAL_STATUS) return 0;
    switch (low_nibble)
    {
        case MIDI_F_SONG_POSITION:
            return 2;
        case MIDI_F_SONG_SELECT:
        case MIDI_F_BUS_SELECT_UNOFFICIAL:
            return 1;
        default:
            return 0;
    }
}



void Midi::ignore_sysex(){
    bool reached_end = false;
    
    size_t bytes_to_read = 0;
    int bytes_read;
    uint8_t buf;
    while(!reached_end){
        //vTaskDelay(1);
        ESP_ERROR_CHECK(uart_get_buffered_data_len(MIDI_UART, &bytes_to_read));
        while(bytes_to_read){
            bytes_read = uart_read_bytes(MIDI_UART, &buf, 1,500);
            if(bytes_read < 0){
                ESP_ERROR_CHECK(uart_get_buffered_data_len(MIDI_UART, &bytes_to_read));
                continue;
                
            } else if (bytes_read == 1){
                if(Midi::is_sysex_end(buf)){
                    reached_end = true;
                    break;
                }
            }
        }
    }
}



bool Midi::is_status_byte(uint8_t byte){
    return (byte & MIDI_STATUS_BYTE_MASK) ? true : false;
}



bool Midi::is_sysex_start(uint8_t byte){
    return byte == MIDI_SYSEX_BEGIN;
}



bool Midi::is_sysex_end(uint8_t byte){
    return byte == MIDI_SYSEX_END;
}
