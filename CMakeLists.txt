set(srcs
        "src/main.c"
        )

set(includes 
        "."
        )

# set(priv_includes
#         "priv_includes/"
#         )

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                    #    PRIV_INCLUDE_DIRS ${priv_includes}
                       PRIV_REQUIRES motor_control
                    )
