idf_component_register(SRCS "lfo.cpp" "lfo_analog.cpp"
               INCLUDE_DIRS "."
               REQUIRES parameters driver pins
             PRIV_REQUIRES ledc_channels )
