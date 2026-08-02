#ifndef DEADMAN_H
#define DEADMAN_H

#include <stdbool.h>

/**
 * @brief Returns the state of the deadman.
 *
 * @return Returns true if deadman held, false if released.
 * @note Deadman usses a delay for releasing and holding,
 *       defined in tank drive settings.
 */
bool get_deadman_state(void);

#endif // DEADMAN_H
