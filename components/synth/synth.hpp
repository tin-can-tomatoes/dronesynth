#ifndef TOMATO_SYNTH_H
#define TOMATO_SYNTH_H

#include <cstdint>

#include <adsr.hpp>
#include <lfo_analog.hpp>
#include <midi.hpp>
#include <musical_notes.hpp>
#include <oscillator.hpp>
#include <params.hpp>


class TomatoSynth : public MidiCallbackHandler, public ParamCallback{
    public:
        void init();
        void tick();
        void note_on(uint8_t note, uint8_t velocity);
        void note_off(uint8_t note);
        void note_force_off();


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
        Oscillator osc1;
        Oscillator osc2;
        Oscillator osc3;
        Oscillator osc4;
        AnalogLfo lfo1;
        AnalogLfo lfo2;

        MidiChannel channel;
        int bpm;
        unsigned long last_bpm_tick;
        
        size_t note_stack[MUSICAL_NOTES_TOTAL];
        uint8_t velocity_stack[MUSICAL_NOTES_TOTAL];
        signed char note_stack_pointer;

};

#endif