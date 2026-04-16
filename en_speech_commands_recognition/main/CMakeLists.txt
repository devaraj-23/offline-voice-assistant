set(srcs
    main.c
    speech_commands_action.c
)
set(requires
    esp-sr
    led_strip
    hardware_driver
    i2cdev
    ssd1306
)
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       REQUIRES ${requires}
                       PRIV_REQUIRES esp_common)


component_compile_options(-w)