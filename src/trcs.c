/*
 * trcs.c
 *
 *  Created on: 17 oct. 2024
 *      Author: Ludo
 */

#include "trcs.h"

#ifndef TRCS_DRIVER_DISABLE_FLAGS_FILE
#include "trcs_driver_flags.h"
#endif
#include "error.h"
#include "math.h"
#include "trcs_hw.h"
#include "types.h"

/*** TRCS local macros ***/

#define TRCS_PROCESS_PERIOD_MS              100

#define TRCS_ADC_SAMPLE_BUFFER_SIZE         10
#define TRCS_ADC_RANGE_MARGIN_MV            100

#define TRCS_RANGE_UP_THRESHOLD_MV          (TRCS_DRIVER_ADC_RANGE_MV - TRCS_ADC_RANGE_MARGIN_MV)
#define TRCS_RANGE_DOWN_THRESHOLD_MV        ((TRCS_RANGE_UP_THRESHOLD_MV / 100) >> 1)

#define TRCS_RANGE_RECOVERY_DELAY_MS        100
#define TRCS_RANGE_STABILIZATION_DELAY_MS   100

#define TRCS_LT6105_VOLTAGE_GAIN            59

/*** TRCS local structures ***/

/*******************************************************************/
typedef struct {
    uint8_t switch_request_pending;
    uint32_t switch_timer_ms;
} TRCS_range_context_t;

/*******************************************************************/
typedef struct {
    TRCS_process_cb_t process_callback;
    volatile uint8_t timer_irq_flag;
    TRCS_range_t current_range;
    TRCS_range_t previous_range;
    TRCS_range_context_t range_ctx[TRCS_RANGE_LAST];
    TRCS_range_state_t range_state;
    uint8_t switch_pending;
    int32_t iout_mv_buf[TRCS_ADC_SAMPLE_BUFFER_SIZE];
    uint8_t iout_mv_buf_idx;
    int32_t iout_mv;
} TRCS_context_t;

/*** TRCS local global variables ***/

static const uint16_t TRCS_SHUNT_RESISTOR_MOHM[TRCS_RANGE_LAST] = { 50000, 500, 5 };
static TRCS_context_t trcs_ctx;

/*** TRCS local functions ***/

/*******************************************************************/
static void _TRCS_timer_irq_callback(void) {
    // Set local flag.
    trcs_ctx.timer_irq_flag = 1;
    // Ask for processing.
    if (trcs_ctx.process_callback != NULL) {
        trcs_ctx.process_callback();
    }
}

/*******************************************************************/
static TRCS_status_t _TRCS_update_adc_data(void) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    MATH_status_t math_status = MATH_SUCCESS;
    // Get raw data in mV.
    status = TRCS_HW_adc_get_iout((int32_t*) &trcs_ctx.iout_mv_buf[trcs_ctx.iout_mv_buf_idx]);
    if (status != TRCS_SUCCESS) goto errors;
    // Update average.
    math_status = MATH_average((int32_t*) trcs_ctx.iout_mv_buf, TRCS_ADC_SAMPLE_BUFFER_SIZE, &trcs_ctx.iout_mv);
    MATH_exit_error(TRCS_ERROR_BASE_MATH);
    // Manage index.
    trcs_ctx.iout_mv_buf_idx++;
    if (trcs_ctx.iout_mv_buf_idx >= TRCS_ADC_SAMPLE_BUFFER_SIZE) {
        trcs_ctx.iout_mv_buf_idx = 0;
    }
errors:
    return status;
}

/*******************************************************************/
static void _TRCS_reset(TRCS_range_state_t range_state) {
    // Local variables.
    uint8_t idx = 0;
    // Reset state.
    trcs_ctx.current_range = TRCS_RANGE_HIGH;
    trcs_ctx.previous_range = TRCS_RANGE_HIGH;
    trcs_ctx.range_state = range_state;
    trcs_ctx.switch_pending = 0;
    // Reset ranges context.
    for (idx = 0; idx < TRCS_RANGE_LAST; idx++) {
        trcs_ctx.range_ctx[idx].switch_request_pending = 0;
        trcs_ctx.range_ctx[idx].switch_timer_ms = 0;
    }
    // Reset buffers.
    for (idx = 0; idx < TRCS_ADC_SAMPLE_BUFFER_SIZE; idx++) {
        trcs_ctx.iout_mv_buf[idx] = 0;
    }
    trcs_ctx.iout_mv_buf_idx = 0;
    trcs_ctx.iout_mv = 0;
}

/*******************************************************************/
static TRCS_status_t _TRCS_set_off_state(uint8_t high_range_state) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    // Use high range by default.
    status = TRCS_HW_set_range_state(TRCS_RANGE_HIGH, high_range_state);
    if (status != TRCS_SUCCESS) goto errors;
    status = TRCS_HW_set_range_state(TRCS_RANGE_MIDDLE, 0);
    if (status != TRCS_SUCCESS) goto errors;
    status = TRCS_HW_set_range_state(TRCS_RANGE_LOW, 0);
    if (status != TRCS_SUCCESS) goto errors;
    // Update state.
    _TRCS_reset((high_range_state == 0) ? TRCS_RANGE_STATE_NONE : TRCS_RANGE_STATE_HIGH);
errors:
    return status;
}

/*** TRCS functions ***/

/*******************************************************************/
TRCS_status_t TRCS_init(void) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    // Init context.
    trcs_ctx.process_callback = NULL;
    trcs_ctx.timer_irq_flag = 0;
    _TRCS_reset(TRCS_RANGE_STATE_NONE);
    // Init hardware interface.
    status = TRCS_HW_init();
    if (status != TRCS_SUCCESS) goto errors;
    // Set board in high range by default to ensure power continuity.
    status = _TRCS_set_off_state(1);
    if (status != TRCS_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TRCS_status_t TRCS_de_init(void) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    // Disable all ranges.
    status = _TRCS_set_off_state(0);
    if (status != TRCS_SUCCESS) goto errors;
    // Release hardware interface.
    status = TRCS_HW_de_init();
    if (status != TRCS_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TRCS_status_t TRCS_start(TRCS_process_cb_t process_callback) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    // Check parameter.
    if (process_callback == NULL) {
        status = TRCS_ERROR_NULL_PARAMETER;
        goto errors;
    }
    // Register callback.
    trcs_ctx.process_callback = process_callback;
    // Start timer.
    status = TRCS_HW_timer_start(TRCS_PROCESS_PERIOD_MS, &_TRCS_timer_irq_callback);
    if (status != TRCS_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TRCS_status_t TRCS_stop(void) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    // Set board in high range by default to ensure power continuity.
    status = _TRCS_set_off_state(1);
    if (status != TRCS_SUCCESS) goto errors;
    // Stop timer.
    status = TRCS_HW_timer_stop();
    if (status != TRCS_SUCCESS) goto errors;
errors:
    return status;
}

/*******************************************************************/
TRCS_status_t TRCS_process(void) {
    // Local variables.
    TRCS_status_t status = TRCS_SUCCESS;
    uint8_t idx = 0;
    // Check flag.
    if (trcs_ctx.timer_irq_flag == 0) goto errors;
    // Clear flag.
    trcs_ctx.timer_irq_flag = 0;
    // Increment recovery timer.
    for (idx = 0; idx < TRCS_RANGE_LAST; idx++) {
        trcs_ctx.range_ctx[idx].switch_timer_ms += (TRCS_PROCESS_PERIOD_MS * trcs_ctx.range_ctx[idx].switch_request_pending);
    }
    // Update ADC result and output current.
    status = _TRCS_update_adc_data();
    if (status != TRCS_SUCCESS) goto errors;
    // Compute range.
    if (trcs_ctx.iout_mv > TRCS_RANGE_UP_THRESHOLD_MV) {
        trcs_ctx.current_range += (!trcs_ctx.switch_pending);
    }
    if ((trcs_ctx.iout_mv < TRCS_RANGE_DOWN_THRESHOLD_MV) && (trcs_ctx.current_range > 0)) {
        trcs_ctx.current_range -= (!trcs_ctx.switch_pending);
    }
    // Check if GPIO control is required.
    if (trcs_ctx.current_range > TRCS_RANGE_HIGH) {
        // Disable all range (protection mode).
        _TRCS_set_off_state(0);
        status = TRCS_ERROR_OVERFLOW;
        goto errors;
    }
    else {
        if (trcs_ctx.current_range != trcs_ctx.previous_range) {
            // Enable new range.
            status = TRCS_HW_set_range_state(trcs_ctx.current_range, 1);
            if (status != TRCS_SUCCESS) goto errors;
            // Update state.
            trcs_ctx.range_state = (TRCS_RANGE_STATE_LOW + trcs_ctx.current_range);
            // Start off timer.
            trcs_ctx.range_ctx[trcs_ctx.previous_range].switch_timer_ms = 0;
            trcs_ctx.range_ctx[trcs_ctx.previous_range].switch_request_pending = 1;
            trcs_ctx.switch_pending++;
        }
        for (idx = 0; idx < TRCS_RANGE_LAST; idx++) {
            // Check recovery timer.
            if (trcs_ctx.range_ctx[idx].switch_timer_ms >= TRCS_RANGE_RECOVERY_DELAY_MS) {
                // Disable range.
                status = TRCS_HW_set_range_state(idx, 0);
                if (status != TRCS_SUCCESS) goto errors;
            }
            if (trcs_ctx.range_ctx[idx].switch_timer_ms >= (TRCS_RANGE_RECOVERY_DELAY_MS + TRCS_RANGE_STABILIZATION_DELAY_MS)) {
                // Stop timer.
                trcs_ctx.range_ctx[idx].switch_timer_ms = 0;
                trcs_ctx.range_ctx[idx].switch_request_pending = 0;
                trcs_ctx.switch_pending--;
            }
        }
    }
errors:
    // Update previous index.
    trcs_ctx.previous_range = trcs_ctx.current_range;
    return status;
}

/*******************************************************************/
TRCS_range_state_t TRCS_get_range_state(void) {
    return (trcs_ctx.range_state);
}

/*******************************************************************/
int32_t TRCS_get_iout(void) {
    // Local variables.
    int32_t iout_ua = 0;
    int64_t num = 0;
    int64_t den = 0;
    // Voltage to current conversion.
    num = (((int64_t) trcs_ctx.iout_mv) * ((int64_t) MATH_POWER_10[6]));
    den = (((int64_t) TRCS_LT6105_VOLTAGE_GAIN) * ((int64_t) TRCS_SHUNT_RESISTOR_MOHM[trcs_ctx.current_range]));
    // Compute output current.
    iout_ua = (int32_t) ((num) / (den));
    return iout_ua;
}
