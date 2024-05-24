idf_component_register(SRCS "oscillator.cpp"
               INCLUDE_DIRS "."
               REQUIRES parameters
             PRIV_REQUIRES ledc_channels musical_notes pins)
