idf_component_register(SRCS "app_main.c"
                    INCLUDE_DIRS ".")

target_add_binary_data(${COMPONENT_TARGET} "client.crt" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "client.key" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "ca.crt" TEXT)