idf_component_register(SRCS "synth.cpp"
               INCLUDE_DIRS "."
                REQUIRES adsr driver ledc_channels lfo midi musical_notes oscillator pins parameters)
