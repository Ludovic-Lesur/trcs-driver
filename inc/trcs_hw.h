/*
 * trcs_hw.h
 *
 *  Created on: 17 oct. 2024
 *      Author: Ludo
 */

#ifndef __TRCS_HW_H__
#define __TRCS_HW_H__

#include "trcs.h"
#include "types.h"

/*** TRCS structures ***/

/*!******************************************************************
 * \enum TRCS_range_t
 * \brief TRCS current measurement ranges.
 *******************************************************************/
typedef enum {
    TRCS_RANGE_LOW = 0,
    TRCS_RANGE_MIDDLE,
    TRCS_RANGE_HIGH,
    TRCS_RANGE_LAST
} TRCS_range_t;

/*!******************************************************************
 * \fn NEOM8X_HW_rx_irq_cb_t
 * \brief Byte reception interrupt callback.
 *******************************************************************/
typedef void (*TRCS_HW_timer_irq_cb_t)(void);

/*** TRCS HW functions ***/

/*!******************************************************************
 * \fn TRCS_status_t TRCS_init(void)
 * \brief Init TRCS hardware interface.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_HW_init(void);

/*!******************************************************************
 * \fn TRCS_status_t TRCS_de_init(void)
 * \brief Release TRCS hardware interface.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_HW_de_init(void);

/*!******************************************************************
 * \fn TRCS_status_t TRCS_HW_timer_start(uint32_t period_ms, TRCS_HW_timer_irq_cb_t irq_callback)
 * \brief Start timer.
 * \param[in]   period_ms: Timer period in ms.
 * \param[in]   irq_callback: Function to call on timer completion.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_HW_timer_start(uint32_t period_ms, TRCS_HW_timer_irq_cb_t irq_callback);

/*!******************************************************************
 * \fn TRCS_status_t TRCS_HW_timer_stop(void)
 * \brief Stop timer.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_HW_timer_stop(void);

/*!******************************************************************
 * \fn TRCS_status_t TRCS_HW_set_range_state(TRCS_range_t range, uint8_t state)
 * \brief Set TRCS range state.
 * \param[in]   range: Current range to control.
 * \param[in]   state: Disable (0) or enable (otherwise) the range.
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_HW_set_range_state(TRCS_range_t range, uint8_t state);

/*!******************************************************************
 * \fn TRCS_status_t TRCS_HW_adc_get_iout(int32_t* iout_mv)
 * \brief Read TRCS output voltage.
 * \param[in]   none
 * \param[out]  iout_mv: Pointer to the TRCS board output voltage.
 * \retval      Function execution status.
 *******************************************************************/
TRCS_status_t TRCS_HW_adc_get_iout(int32_t* iout_mv);

#endif /* __TRCS_HW_H__ */
