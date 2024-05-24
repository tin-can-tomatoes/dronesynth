#include <esp_attr.h>
#include <esp_cpu.h>
#include <esp_log.h>
#include <esp_system.h>


#include <ledc_channels.hpp>
#include <params.hpp>
#include <pins.hpp>

#include "synth.hpp"

#define TIMER_DIVIDER (16)
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)

uint32_t ticks = 0;
// uint32_t adsr_ticks = 0;

static uint32_t get_timestamp(){
    static uint32_t divisor = esp_rom_get_cpu_ticks_per_us() * 1000;
    return esp_cpu_get_cycle_count() / divisor;
}

void TomatoSynth::init(){
    ESP_LOGI("INFO","Tomatosynth!");

    /// Init output LFOS, ADSR, Oscillator
    // adsr.init(OUTPUT_ADSR);
    lfo1.init(OUTPUT_LFO1, 1);
    lfo2.init(OUTPUT_LFO2, 2);
    osc1.init(OSCILLATOR_1, OUTPUT_OSC1);
    osc2.init(OSCILLATOR_2, OUTPUT_OSC2);
    osc3.init(OSCILLATOR_3, OUTPUT_OSC3);
    osc4.init(OSCILLATOR_4, OUTPUT_OSC4);
    
    Param::params[PARAM_SYNTH_MIDI_CHANNEL].set_callback(this);
    
    Midi::init(21,22,this);
    Param::start_watching_inputs();

}


void IRAM_ATTR TomatoSynth::tick(){
    ++ticks;
    // ++ adsr_ticks;
    // adsr.tick(adsr_ticks);
    lfo1.tick(ticks);
    lfo2.tick(ticks);
}


//
//     NOTE:
//     Everything below here relates to MIDI input;
//     this was written for my musical monophonic synth project
//     and isn't particularly relevant for the noise/drone synth.
//     I just left it here below in case anyone might
//     want to see how I've handled the MIDI side of things...
//     but since this version of the code has drifted somewhat
//     from the mono synth project, a lot of things are broken;
//     (those would be everything that's commented out)
//     and since the mono synth wasn't even finished when I 
//     forked the code for this project, some of it wasn't even
//     fully implemented in the first place
//                                  - tomatoes
//

void IRAM_ATTR TomatoSynth::note_on(uint8_t note, uint8_t velocity){
    // WARNING: note_on for any given note should not be called again for that note
    // until after the note_off has been called for that note; the array will fill up.
    // todo: fix that

    size_t next_slot = note_stack_pointer + 1;
    if(next_slot < MUSICAL_NOTES_TOTAL)
    {
        note_stack[next_slot] = note;
        velocity_stack[next_slot] = velocity;
        note_stack_pointer = next_slot;
        /*
        osc1.play_note(note);
        osc2.play_note(note);
        osc3.play_note(note);
        osc4.play_note(note);
        */
        //if(retrigger_adsr_adding_note || note_stack_pointer == 0){
        //
        //        adsr_ticks = 0;
        //    adsr.trigger_on(velocity);
        //}
    }
}

void IRAM_ATTR TomatoSynth::note_off(uint8_t note){
    if(note_stack_pointer < 0) return; // Already off. Hopefully, shouldn't happen
    if(note_stack[note_stack_pointer] == note){ 
        // Note is at top of stack, so need to stop this note and go to the note below in the stack (if applicable)
        -- note_stack_pointer;

        if(note_stack_pointer < 0){
            // If the stack is now empty, trigger adsr off
            // adsr.trigger_off(); 
        } else {
            // osc1.play_note(note_stack[note_stack_pointer]);
            // osc2.play_note(note_stack[note_stack_pointer]);
            // osc3.play_note(note_stack[note_stack_pointer]);
            // osc4.play_note(note_stack[note_stack_pointer]);
            // if(retrigger_adsr_removing_note){
            //     adsr_ticks = 0;
            //     adsr.trigger_on(velocity_stack[note_stack_pointer]);
            // }
            }
    }
    else
    {   // Note is not at top of stack, need to shift stack
        // Find position of note in stack
        size_t note_pos;
        for(note_pos = 0; note_pos < note_stack_pointer; ++note_pos){
            if(note_stack[note_pos] == note)
            {
                break;
            }
        }
        if(note_pos == note_stack_pointer) return; // Not in stack. Nothing to do


        // Shift note stack down from position of note on
        for(size_t i = note_pos; i < note_stack_pointer; ++i){
            note_stack[i] = note_stack[i+1];
            velocity_stack[i] = velocity_stack[i+1];
        }
        -- note_stack_pointer;
        // Don't need to change the tone
    }
}

void IRAM_ATTR TomatoSynth::note_force_off(){
    // Just clear the note stack
    note_stack_pointer = -1;
    // adsr.trigger_off();
}
void TomatoSynth::handle_note_on(MidiChannel midi_channel, uint8_t note, uint8_t velocity)
{
    if(midi_channel!=channel) return;
    //note_on(note, velocity);
}

void TomatoSynth::handle_note_off(MidiChannel midi_channel, uint8_t note, uint8_t velocity)
{
    if(midi_channel!=channel) return;
    //note_off(note);
}

void TomatoSynth::handle_key_pressue(MidiChannel midi_channel, uint8_t key, uint8_t pressure)
{
    // ignore this command
}

void TomatoSynth::handle_controller_change(MidiChannel midi_channel, uint8_t controller, uint8_t value)
{
    if(midi_channel!=channel) return;
    // todo: params
}

void TomatoSynth::handle_program_change(MidiChannel midi_channel, uint8_t preset)
{
    // ignore this command
}

void TomatoSynth::handle_channel_pressure(MidiChannel midi_channel, uint8_t pressure)
{
    // ignore this command
}

void TomatoSynth::handle_pitch_bend(MidiChannel midi_channel, uint16_t bend)
{
    // ignore this command
}

void TomatoSynth::handle_song_position(uint16_t position)
{
    // ignore this command
}

void TomatoSynth::handle_song_select(uint8_t song_num)
{
    // ignore this command
}

void TomatoSynth::handle_bus_select(uint8_t bus)
{
    // ignore this command
}

void TomatoSynth::handle_tune_request()
{
    // ignore this command
}

void TomatoSynth::handle_timing_tick()
{
    uint32_t timestamp = get_timestamp();
    int calculated_bpm = 0;
    // todo: calculate bpm from timing ticks (1 tick = 1ms)

    if(calculated_bpm != Param::get(PARAM_SYNTH_BPM)){
        Param::set(PARAM_SYNTH_BPM, calculated_bpm);
    }
    // finally:
    last_bpm_tick = timestamp;

}

void TomatoSynth::handle_start_song()
{
    // ignore this command
}

void TomatoSynth::handle_continue_song()
{
    // ignore this command
}

void TomatoSynth::handle_stop_song()
{
    // ignore this command
}

void TomatoSynth::handle_active_sensing()
{
    // ignore this command
}

void TomatoSynth::handle_system_reset()
{
    esp_restart();
}

void TomatoSynth::param_changed(SynthParams param, int old_int, int new_int, double old_double, double new_double)
{
    switch (param)
    {
    case PARAM_SYNTH_MIDI_CHANNEL:
        channel = (MidiChannel) new_int;
        break;
        default: return;
    }
}
