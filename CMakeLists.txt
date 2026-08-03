set(srcs
        "src/tank_drive.c"
        "src/kinematic_task.c"
        "src/kinematic_model.c"
        "src/deadman.c"
        )

set(include
        "."
        "priv_include"
        )

set(priv_include
        # "priv_include/"
        )

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include}
                       PRIV_INCLUDE_DIRS ${priv_includes}
                       REQUIRES tank_drive_move_command
                       PRIV_REQUIRES motor_control
                    )
