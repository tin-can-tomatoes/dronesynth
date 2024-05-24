idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES pins synth esp_system freertos driver
                    )
