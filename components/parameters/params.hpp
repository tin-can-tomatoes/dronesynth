#ifndef TOMATO_PARAMS_H
#define TOMATO_PARAMS_H


/// @brief Wave functions for the LFOs
enum LFOWaveFunc{
     /// @brief Use a sine waveform for the LFO
     WAVE_FUNC_SINE=0,
     /// @brief Use a sawtooth or ramp waveform for the LFO
     WAVE_FUNC_SAWTOOTH,
     /// @brief Use a square or pwm waveform for the LFO
     WAVE_FUNC_SQUARE
};

/// @brief How the oscillators map input values to pitches
enum OscillatorPitchMode {
     /// @brief Play a musical note within MUSICAL_NOTE_MIN and MUSICAL_NOTE_MAX
     OSCILLATOR_MODE_NOTE,
     /// @brief Play the frequency that = (input_value / 2) + OSCILLATOR_MIN_HZ
     OSCILLATOR_MODE_HALFHZ,
     /// @brief Play the frequency that = input_value + OSCILLATOR_MIN_HZ
     OSCILLATOR_MODE_1HZ,
     /// @brief Play the frequency that = (input_value * 2) + OSCILLATOR_MIN_HZ
     OSCILLATOR_MODE_2HZ,
     /// @brief Play the frequency that = (input_value * 3) + OSCILLATOR_MIN_HZ
     OSCILLATOR_MODE_3HZ

};

/// @brief Enumeration of parameters used by the synth
enum SynthParams {
PARAM_DISABLED = -1,
PARAM_ZERO,
PARAM_SYNTH_MIDI_CHANNEL /* Which midi channel the synth should listen to */,
PARAM_SYNTH_BPM /* BPM Reported by MIDI */,
PARAM_OSC1_PITCH_RAW /* Note or pitch for OSC1 */,
PARAM_OSC1_PWM_DUTY, /* OSC1 PWM Duty */
PARAM_OSC1_PITCH_MODE,
PARAM_OSC2_PITCH_RAW,
PARAM_OSC2_PWM_DUTY,
PARAM_OSC2_PITCH_MODE,
PARAM_OSC3_PITCH_RAW,
PARAM_OSC3_PWM_DUTY,
PARAM_OSC3_PITCH_MODE,
PARAM_OSC4_PITCH_RAW,
PARAM_OSC4_PWM_DUTY,
PARAM_OSC4_PITCH_MODE,
PARAM_LFO1_PERIOD_CS, // lfo period in centiseconds
PARAM_LFO1_PHASE,
PARAM_LFO1_WAVE_FUNC,
PARAM_LFO1_WAVE_MIDPOINT_DUTY,
PARAM_LFO1_FLOOR,
PARAM_LFO1_CIEL,
PARAM_LFO2_PERIOD_CS,
PARAM_LFO2_PHASE,
PARAM_LFO2_WAVE_FUNC,
PARAM_LFO2_WAVE_MIDPOINT_DUTY,
PARAM_LFO2_FLOOR,
PARAM_LFO2_CIEL,
PARAM_MAX
};

/// @brief Map a value from an input range to an output range. Same function exists in arduino.
/// @param x Value
/// @param in_min Minimum input value
/// @param in_max Maximum input value
/// @param out_min Minimum output value
/// @param out_max Maximum output value
/// @return 
long map(long x, long in_min, long in_max, long out_min, long out_max);

/// @brief Interface for a class that can handle a parameter changing
class ParamCallback{
     public:
          virtual void param_changed(SynthParams param, int old_int, int new_int, double old_double, double new_double) = 0;
};

/// @brief Param class holds param values.
/// The static methods of the class allow accessing and changing parameters without holding the instance of Param for that parameter
class Param {
    public:

        /// @brief Get the integer value of a given param
        /// @param param The parameter (from SynthParams enum) to get
        /// @return The integer value, which should be between the param's min and max
        static int get(SynthParams param);

        /// @brief Get the double-precision floating point value of a given param
        /// @param param The parameter (from SynthParams enum) to get
        /// @return The floating point value, between 0.0 and 1.0
        static double getd(SynthParams param);

        /// @brief Set the integer value of a given param
        /// @param param The parameter (from SynthParams enum) to set
        /// @param value The integer value. This value will be clamped to the param's min and max values before being applied
        static void set(SynthParams param, int value);

        /// @brief Set the floating point value of a given param
        /// @param param The parameter (from SynthParams enum) to set
        /// @param value The double value, between 0 and 1. Values less than zero will be upped to 0, values greather than 1 will be capped at 1.
        static void set(SynthParams param, double value);

        /// @brief Map a midi value (0 to 127) to the param's actual range, then set the param to the mapped value
        /// @param param The parameter (from SynthParams enum) to set
        /// @param val The midi value to set (between 0 and 127)
        static void set_midi(SynthParams param, signed char val);

        /// @brief The array of Param instances
        static Param params[PARAM_MAX];

        /// @brief Utility function that dumps all param values to the serial monitor
        static void dump();

        /// @brief Checks the input pins for any changes and applies them
        /// @attention This function is an infinite loop. Don't call this - it runs in a RTOS task
        /// @see Param::start_watching_inputs()
        static void poll_inputs();

        /// @brief Starts the RTOS task that monitors the input pins
        /// @attention This function should only be called once.
        static void start_watching_inputs();

        /// @brief Gets the integer value of this Param
        /// @return The integer value, which should be between the param's min and max
        int    get_int();

        /// @brief Gets the double-precision floating point value of this param
        /// @return The floating point value, between 0.0 and 1.0
        double get_double();

        /// @brief Set the integer value of this param
        /// @param value The integer value. This value will be clamped to the param's min and max values before being applied
        void set_int(int value);

        /// @brief Set the floating point value of this param
        /// @param value The double value, between 0 and 1. Values less than zero will be upped to 0, values greather than 1 will be capped at 1.
        void set_double(double value);

        /// @brief Map a midi value (0 to 127) to this param's actual range, then set this param to the mapped value
        /// @param value The midi value to set (between 0 and 127)
        void set_midi(signed char value);

        /// @brief Class to callback when the parameter is updated.
        ParamCallback * callback;

        /// @brief Set the callback for when the parameter is updated.
        /// @param cb Class to callback when the parameter is updated.
        void set_callback(ParamCallback* cb);
        
    private:
     
        /// @brief Construct a param without any associated input pin or MIDI channel control number. 
        /// @attention Unless a input pin or midi channel control number is assigned later in code, 
        /// or the param's value is manually updated in code - the param will not change from default val
        /// @param param Which param
        /// @param default_val Default/starting value
        /// @param value_min Minimum integer value, maps to 0.0 in the param's floating point value
        /// @param value_max Maximum integer value, maps to 1.0 in the param's floating point value
        Param(SynthParams param,
             int default_val,
             int value_min,
             int value_max);

        /// @brief Construct a param with a MIDI channel control number.
        /// @param param Which param
        /// @param default_val Default starting value
        /// @param value_min Minimum integer value, maps to 0.0 in the param's floating point value
        /// @param value_max Maximum integer value, maps to 1.0 in the param's floating point value
        /// @param midi_cc The midi channel controller parameter that should control the value of this param
        Param(SynthParams param,int default_val,
             int value_min,
             int value_max,
             signed char midi_cc);

        
        /// @brief Construct a param with a MIDI channel control number.
        /// @attention The value from the input pin will overrule whatever MIDI sends. This behavior will
        /// be fixed eventually, but for now, MIDI CCs and Input Pins for the same param do not play together well.
        /// @param param Which param
        /// @param default_val Default starting value
        /// @param value_min Minimum integer value, maps to 0.0 in the param's floating point value
        /// @param value_max Maximum integer value, maps to 1.0 in the param's floating point value
        /// @param midi_cc The midi channel controller parameter that should control the value of this param.
        ///                Set to -1 to disable
        /// @param input_pin Which ADC pin to monitor as the physical control knob for this parameter.
        ///                Set to 0 to disable
        Param(SynthParams param,int default_val,
             int value_min,
             int value_max,
             signed char midi_cc,
             unsigned char input_pin);


        signed char midi_cc;
        unsigned char input_pin;

        SynthParams param;

        // Integer value
        volatile int value_int;
        // double value (0 - 1)
        volatile double value_double;
        // Minimum integer value, maps to double value 0
        int value_min; 
        // Maximum integer value, maps to double value 1
        int value_max;
        // Last input value
        volatile unsigned int last_input_value;
        // Default value
        int default_value;

};
#endif