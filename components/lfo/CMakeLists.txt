idf_component_register(SRCS "lfo.cpp" "lfo_pwm.cpp" "lfo_analog.cpp"
               INCLUDE_DIRS "."
               REQUIRES parameters driver pins ledc_channels )
