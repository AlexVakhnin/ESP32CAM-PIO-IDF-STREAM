set(srcs "main.c"
         "reconnect_wifi.c")
idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
