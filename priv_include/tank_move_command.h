#ifndef TANK_MOVE_COMMAND_H
#define TANK_MOVE_COMMAND_H

/**
 * @brief A type that tank drive uses as a movement command.
 */
typedef struct
{
    float T; //! Scalar value of translationar movement. (from 1.0 to -1.0)
             // This value needs to be in range from 1.0f to -1.0f, which
             // corresponds to max possible possitive and negative translation
             // speed. (1.0 = go forrward at max speed, 0.0 = stay in place,
             // -1.0 = backwards at max speed)
             //
    float R; //! Scalar value of rotational movement. (from 1.0 to -1.0)
             // This value needs to be in range from 1.0f to -1.0f, which
             // corresponds to max possible possitive and negative rotation
             // speed. (1.0 = rotate clockwise at max speed, 0.0 = do not rotate,
             // -1.0 = rotate counterclockwise at max speed)
} tank_drive_move_command_t;

typedef tank_drive_move_command_t (*move_command_getter)(void);

#endif // TANK_MOVE_COMMAND_H
