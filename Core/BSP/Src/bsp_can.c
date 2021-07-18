/**
 * bsp_can.c
 *
 * Created on: Mar 2 2020
 *     Author: Raghav Bhardwaj
 */

#include "bsp_can.h"

/** Note
 * Currently only the CAN FIFO0 has been implemented. An implementation of FIFO1
 * simultaneously wouldn't be too hard to do.
 */

/**
 * HAL internal callback function that calls abstracted ISR for ease of use.
 * Define can_ISR() elsewhere in code to define behaviour of CAN receive ISR.
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	can_ISR(hcan);
}

HAL_StatusTypeDef can_get_msg(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *rx_msg_header, uint8_t *rx_buffer)
{
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, rx_msg_header, rx_buffer);
	return HAL_OK;
}


/**
 * CAN1 and CAN2 are handled slightly differently.
 */
void can_start(CAN_HandleTypeDef *hcan) {
    CAN_FilterTypeDef can_filter_st = {0};
    can_filter_st.FilterActivation = ENABLE;
	can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
	can_filter_st.FilterIdHigh = 0x0000;
	can_filter_st.FilterIdLow = 0x0000;
	can_filter_st.FilterMaskIdHigh = 0x0000;
	can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    // can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO1; // Uncomment line if using RX1 queue as well.

	if (hcan->Instance == CAN1) {
	    can_filter_st.FilterBank = 0;
	} else if (hcan->Instance == CAN2) {
		can_filter_st.SlaveStartFilterBank = 14;
		can_filter_st.FilterBank = 14;
	}

    HAL_CAN_ConfigFilter(hcan, &can_filter_st);
    HAL_CAN_Start(hcan);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    // HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING); // Uncomment line if using RX1 queue as well.
}
