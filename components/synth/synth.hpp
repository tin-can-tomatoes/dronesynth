#ifndef TOMATO_SYNTH_H
#define TOMATO_SYNTH_H

#include <cstdint>

#include <lfo_analog.hpp>
#include <lfo_pwm.hpp>
#include <midi.hpp>
#include <params.hpp>


class TomatoSynth : public MidiCallbackHandler, public ParamCallback{
    public:
        void init();
        void tick();


        // Midi handlers:
        void handle_note_on(MidiChannel midi_channel, uint8_t note, uint8_t velocity);
        void handle_note_off(MidiChannel midi_channel, uint8_t note, uint8_t velocity);
        void handle_key_pressue(MidiChannel midi_channel, uint8_t key, uint8_t pressure);
        void handle_controller_change(MidiChannel midi_channel, uint8_t controller, uint8_t value);
        void handle_program_change(MidiChannel midi_channel, uint8_t preset);
        void handle_channel_pressure(MidiChannel midi_channel, uint8_t pressure);
        void handle_pitch_bend(MidiChannel midi_channel, uint16_t bend);

        void handle_song_position(uint16_t position);
        void handle_song_select(uint8_t song_num);
        void handle_bus_select(uint8_t bus);

        void handle_tune_request();
        void handle_timing_tick();
        void handle_start_song();
        void handle_continue_song();
        void handle_stop_song();
        void handle_active_sensing();
        void handle_system_reset();

        // Param handler
        void param_changed(SynthParams param, int old_int, int new_int, double old_double, double new_double);
    private:
        AnalogLfo lfo1;
        AnalogLfo lfo2;
        PWMLfo lfo3;
        PWMLfo lfo4;

        MidiChannel channel;
        int bpm;
        unsigned long last_bpm_tick;
        

};

#endif