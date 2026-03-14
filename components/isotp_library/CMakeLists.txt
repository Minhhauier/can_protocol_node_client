idf_component_register(SRCS "isotp_handler.c" "isotp.c"
                            INCLUDE_DIRS "include"
                            REQUIRES 
                                driver
                                esp_timer)