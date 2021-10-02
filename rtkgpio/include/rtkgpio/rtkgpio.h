/*
 * rtkgpio.h
 *
 *  Created on: Jan 7, 2021
 *      Author: de23a4
 */

#ifndef RTKGPIO_H_
#define RTKGPIO_H_

#include "rtkgpio_dll.h"

#ifdef __cplusplus
extern "C"
{
#endif

enum rtkgpio_mode {
    RTKGPIO_MODE_INPUT, RTKGPIO_MODE_OUTPUT
};

enum rtkgpio_pud {
    RTKGPIO_PUD_DOWN, RTKGPIO_PUD_UP, RTKGPIO_PUD_OFF
};

enum rtkgpio_value {
    RTKGPIO_VALUE_HIGH, RTKGPIO_VALUE_LOW
};

/**
 * Forward declaration of the opaque handle.
 */
struct rtkgpio;

/**
 * @brief Creates a new RTk.GPIO object.
 *
 * An successful created instance must be freed by tk_gpio_free(struct rtkgpio *rtkgpio) after it is not used anymore, to free the used resources.
 *
 * @param port_name the port name that should be used. If NULL is provided the first found port of a connected RTk.GPIO board is used.
 * @param rtkgpio pointer to the location a pointer to an instance object should be stored
 * @return return 0 on success, on error a negative value is returned
 */
RTKGPIO_API int RTKGPIO_CALL rtkgpio_create(const char *port_name, struct rtkgpio **rtkgpio);

/**
 * @brief Free a RTk.GPIO instance.
 *
 * After this method has been called the instance must not be used anymore.
 *
 * @param rtkgpio the instance that should be freed.
 */
RTKGPIO_API void RTKGPIO_CALL rtkgpio_free(struct rtkgpio *rtkgpio);

/**
 * @brief Set / unset board mode.
 *
 * @param rtkgpio the instance
 * @param boardmode flag that indicates if board mode should be used or not (0: false, others: true)
 */
RTKGPIO_API void RTKGPIO_CALL rtkgpio_set_boardmode(struct rtkgpio *rtkgpio, int boardmode);

/**
 * @brief Setup an GPIO pin.
 *
 * @param rtkgpio the instance
 * @param channel the PIN (the number to PIN mapping respects if board mode is used or not)
 * @param mode the mode
 * @param pud the pull-up or -down configuration
 * @param value the initial value
 * @return return 0 on success, on error a negative value is returned
 */
RTKGPIO_API int RTKGPIO_CALL rtkgpio_setup(struct rtkgpio *rtkgpio, int channel,
                                           enum rtkgpio_mode mode, enum rtkgpio_pud pud,
                                           enum rtkgpio_value value);

/**
 * @brief Gets the value of an input pin.
 *
 * @param rtkgpio the instance
 * @param channel the PIN (the number to PIN mapping respects if board mode is used or not)
 * @param value the location the current value should be stored to
 * @return return 0 on success, on error a negative value is returned
 */
RTKGPIO_API int RTKGPIO_CALL rtkgpio_input(struct rtkgpio *rtkgpio, int channel,
                                           enum rtkgpio_value *value);

/**
 * @brief Sets an output pin.
 *
 * @param rtkgpio the instance
 * @param channel the PIN (the number to PIN mapping respects if board mode is used or not)
 * @param value the value to set
 * @return return 0 on success, on error a negative value is returned
 */
RTKGPIO_API int RTKGPIO_CALL rtkgpio_output(struct rtkgpio *rtkgpio, int channel,
                                            enum rtkgpio_value value);

#ifdef __cplusplus
} // __cplusplus defined.
#endif

#endif /* RTKGPIO_H_ */
