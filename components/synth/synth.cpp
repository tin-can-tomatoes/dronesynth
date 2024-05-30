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


    lfo1.init(OUTPUT_LFO1, 1);
    lfo2.init(OUTPUT_LFO2, 2);
    lfo3.init(OUTPUT_LFO3, 3);
    lfo4.init(OUTPUT_LFO4, 4);
    
    Param::params[PARAM_SYNTH_MIDI_CHANNEL].set_callback(this);
    
    Midi::init(21,22,this);

}


void IRAM_ATTR TomatoSynth::tick(){
    ++ticks;
    // ++ adsr_ticks;
    // adsr.tick(adsr_ticks);
    lfo1.tick(ticks);
    lfo2.tick(ticks);
    lfo3.tick(ticks);
    lfo4.tick(ticks);
}

void TomatoSynth::handle_note_on(MidiChannel midi_channel, uint8_t note, uint8_t velocity)
{
    // Ignore this command
}

void TomatoSynth::handle_note_off(MidiChannel midi_channel, uint8_t note, uint8_t velocity)
{
    // Ignore this command
}

void TomatoSynth::handle_key_pressue(MidiChannel midi_channel, uint8_t key, uint8_t pressure)
{
    // Ignore this command
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
