idf_component_register(SRCS "can_protocol.c"
                            INCLUDE_DIRS "include"
                            REQUIRES 
                                driver
                                isotp_library
                            PRIV_REQUIRES 
                                config_parameter)