/*
 * trcs_hw.c
 *
 *  Created on: 17 oct. 2024
 *      Author: Ludo
 */

#include "trcs_hw.h"

#ifndef TRCS_DRIVER_DISABLE_FLAGS_FILE
#include "trcs_driver_flags.h"
#endif
#include "trcs.h"
#include "types.h"

#ifndef TRCS_DRIVER_DISABLE

/*** TRCS HW functions ***/

/*******************************************************************/
TRCS_status_t __attribute__((weak)) TRCS_HW_init(void) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    /* To be implemented */
    return status;
}

/*******************************************************************/
TRCS_status_t __attribute__((weak)) TRCS_HW_de_init(void) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    /* To be implemented */
    return status;
}

/*******************************************************************/
TRCS_status_t __attribute__((weak)) TRCS_HW_timer_start(uint32_t period_ms, TRCS_HW_timer_irq_cb_t irq_callback) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    /* To be implemented */
    UNUSED(period_ms);
    UNUSED(irq_callback);
    return status;
}

/*******************************************************************/
TRCS_status_t __attribute__((weak)) TRCS_HW_timer_stop(void) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    /* To be implemented */
    return status;
}

/*******************************************************************/
TRCS_status_t __attribute__((weak)) TRCS_HW_set_output_current_range_state(TRCS_output_current_range_t output_current_range, uint8_t state) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    /* To be implemented */
    UNUSED(output_current_range);
    UNUSED(state);
    return status;
}

/*******************************************************************/
TRCS_status_t __attribute__((weak)) TRCS_HW_adc_get_output_current(int32_t* output_current_mv) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    /* To be implemented */
    UNUSED(output_current_mv);
    return status;
}

#endif /* TRCS_DRIVER_DISABLE */
