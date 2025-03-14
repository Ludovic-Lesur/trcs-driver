/*
 * trcs.h
 *
 *  Created on: 17 oct. 2024
 *      Author: Ludo
 */

#ifndef __TRCS_H__
#define __TRCS_H__

#ifndef TRCS_DRIVER_DISABLE_FLAGS_FILE
#include "trcs_driver_flags.h"
#endif
#include "error.h"
#include "maths.h"
#include "types.h"

/*** TRCS structures ***/

/*!******************************************************************
 * \enum TRCS_status_t
 * \brief TRCS driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    TRCS_SUCCESS = 0,
    TRCS_ERROR_NULL_PARAMETER,
    TRCS_ERROR_OVERFLOW,
    TRCS_ERROR_RANGE,
    // Low level drivers errors.
    TRCS_ERROR_BASE_GPIO = ERROR_BASE_STEP,
    TRCS_ERROR_BASE_TIMER = (TRCS_ERROR_BASE_GPIO + TRCS_DRIVER_GPIO_ERROR_BASE_LAST),
    TRCS_ERROR_BASE_ADC = (TRCS_ERROR_BASE_TIMER + TRCS_DRIVER_TIMER_ERROR_BASE_LAST),
    TRCS_ERROR_BASE_MATH = (TRCS_ERROR_BASE_ADC + TRCS_DRIVER_ADC_ERROR_BASE_LAST),
    // Last base value.
    TRCS_ERROR_BASE_LAST = (TRCS_ERROR_BASE_MATH + MATH_ERROR_BASE_LAST)
} TRCS_status_t;

#ifndef TRCS_DRIVER_DISABLE

/*!******************************************************************
 * \enum TRCS_range_state_t
 * \brief TRCS current measurement ranges state.
 *******************************************************************/
typedef enum {
    TRCS_RANGE_STATE_NONE = 0,
    TRCS_RANGE_STATE_LOW,
    TRCS_RANGE_STATE_MIDDLE,
    TRCS_RANGE_STATE_HIGH,
    TRCS_RANGE_STATE_LAST
} TRCS_range_state_t;

/*!******************************************************************
 * \fn TRCS_process_cb_t
 * \brief TRCS driver process callback.
 *******************************************************************/
typedef void (*TRCS_process_cb_t)(void);

/*** TRCS functions ***/

/*!******************************************************************
 * \fn TRCS_status_t TRCS_init(void)
 * \brief Init TRCS driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_init(void);

/*!******************************************************************
 * \fn TRCS_status_t TRCS_de_init(void)
 * \brief Release TRCS driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_de_init(void);

/*!******************************************************************
 * \fn TRCS_status_t TRCS_start(void)
 * \brief Start TRCS board control.
 * \param[in]   process_callback: Function which will be called when the TRCS driver has to be processed.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_start(TRCS_process_cb_t process_callback);

/*!******************************************************************
 * \fn TRCS_status_t TRCS_stop(void)
 * \brief Stop TRCS board control.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_stop(void);

/*!******************************************************************
 * \fn TRCS_status_t TRCS_process(void)
 * \brief Process TRCS driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_process(void);

/*!******************************************************************
 * \fn TRCS_range_state_t TRCS_get_range_state(void)
 * \brief Get TRCS current range state.
 * \param[in]   none
 * \param[out]  none
 * \retval      TRCS range state.
 *******************************************************************/
TRCS_range_state_t TRCS_get_range_state(void);

/*!******************************************************************
 * \fn int32_t TRCS_get_iout(void)
 * \brief Get TRCS measured output current.
 * \param[in]   none
 * \param[out]  none
 * \retval      Measured output current in uA.
 *******************************************************************/
int32_t TRCS_get_iout(void);

/*******************************************************************/
#define TRCS_exit_error(base) { ERROR_check_exit(trcs_status, TRCS_SUCCESS, base) }

/*******************************************************************/
#define TRCS_stack_error(base) { ERROR_check_stack(trcs_status, TRCS_SUCCESS, base) }

/*******************************************************************/
#define TRCS_stack_exit_error(base, code) { ERROR_check_stack_exit(trcs_status, TRCS_SUCCESS, base, code) }

#endif /* TRCS_DRIVER_DISABLE */

#endif /* __TRCS_H__ */
