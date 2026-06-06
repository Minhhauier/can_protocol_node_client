file(GLOB SRCS "at_command.c" "*.c")

idf_component_register(
        SRCS ${SRCS}
        INCLUDE_DIRS include
        REQUIRES
            driver
            drivers
            freertos
            config  
            encrypt
            config_parameter
            control_relay
            )
