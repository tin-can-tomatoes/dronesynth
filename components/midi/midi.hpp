#ifndef TOMATO_MIDI_H
#define TOMATO_MIDI_H
#include <cstdint>
#include <cstddef>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define MIDI_STATUS_BYTE_MASK 0b10000000
#define MIDI_STATUS_COMMAND_MASK 0xF0
#define MIDI_STATUS_CHANNEL_MASK 0x0F

enum MidiStatusType : uint8_t{
    MIDI_NOTE_OFF           = 0x80,
    MIDI_NOTE_ON            = 0x90,
    MIDI_KEY_PRESSURE       = 0xA0,
    MIDI_CONTROLLER_CHANGE  = 0xB0,
    MIDI_PROGRAM_CHANGE     = 0xC0,
    MIDI_CHANNEL_PRESSURE   = 0xD0,
    MIDI_PITCH_BEND         = 0xE0,
    MIDI_ADDITIONAL_STATUS  = 0xF0
};

enum MidiAdditionalStatusType : uint8_t {
    MIDI_F_SYSEX = 0x00,
    MIDI_F_SONG_POSITION = 0x02,
    MIDI_F_SONG_SELECT = 0x03,
    MIDI_F_BUS_SELECT_UNOFFICIAL = 0x05,
    MIDI_F_TUNE_REQUEST = 0x06,
    MIDI_F_END_OF_SYSEX = 0x07,
    MIDI_F_TIMING_TICK = 0x08,
    MIDI_F_START_SONG = 0x0A,
    MIDI_F_CONTINUE_SONG = 0x0B,
    MIDI_F_STOP_SONG = 0x0C,
    MIDI_F_ACTIVE_SENSING = 0x0E,
    MIDI_F_SYSTEM_RESET = 0x0F

};

enum MidiChannel : uint8_t{
    MIDI_CHANNEL_1 = 0,
    MIDI_CHANNEL_2,
    MIDI_CHANNEL_3,
    MIDI_CHANNEL_4,
    MIDI_CHANNEL_5,
    MIDI_CHANNEL_6,
    MIDI_CHANNEL_7,
    MIDI_CHANNEL_8,
    MIDI_CHANNEL_9,
    MIDI_CHANNEL_10,
    MIDI_CHANNEL_11,
    MIDI_CHANNEL_12,
    MIDI_CHANNEL_13,
    MIDI_CHANNEL_14,
    MIDI_CHANNEL_15,
    MIDI_CHANNEL_16
};

#define MIDI_SYSEX_BEGIN 0xF0
#define MIDI_SYSEX_END 0xF7

class MidiCallbackHandler{
    public: 
        virtual void handle_note_on(MidiChannel midi_channel, uint8_t note, uint8_t velocity) = 0;
        virtual void handle_note_off(MidiChannel midi_channel, uint8_t note, uint8_t velocity) = 0;
        virtual void handle_key_pressue(MidiChannel midi_channel, uint8_t key, uint8_t pressure) = 0;
        virtual void handle_controller_change(MidiChannel midi_channel, uint8_t controller, uint8_t value) = 0;
        virtual void handle_program_change(MidiChannel midi_channel, uint8_t preset) = 0;
        virtual void handle_channel_pressure(MidiChannel midi_channel, uint8_t pressure) = 0;
        virtual void handle_pitch_bend(MidiChannel midi_channel, uint16_t bend) = 0;

        virtual void handle_song_position(uint16_t position) = 0;
        virtual void handle_song_select(uint8_t song_num) = 0;
        virtual void handle_bus_select(uint8_t bus) = 0;

        virtual void handle_tune_request() = 0;
        virtual void handle_timing_tick() = 0;
        virtual void handle_start_song() = 0;
        virtual void handle_continue_song() = 0;
        virtual void handle_stop_song() = 0;
        virtual void handle_active_sensing() = 0;
        virtual void handle_system_reset() = 0;
};

class Midi {
    public:
        static void init(uint8_t rx_pin, uint8_t tx_pin, MidiCallbackHandler* handler);
        static void rx_loop();
    private:
        static uint8_t rx_pin;
        static uint8_t tx_pin;
        static TaskHandle_t midi_task_handle;
        static MidiCallbackHandler * handler;
        static void dispatch_message(uint8_t status, uint8_t data1, uint8_t data2);
        static size_t get_expected_bytes_count(uint8_t status);
        static void ignore_sysex();
        static bool is_status_byte(uint8_t byte);
        static bool is_sysex_start(uint8_t byte);
        static bool is_sysex_end(uint8_t byte);
};

#endif