#pragma once

#include "stm32f4xx_hal.h"
#include "Queue.h"

HAL_StatusTypeDef CAN_init(CAN_HandleTypeDef *handle);
HAL_StatusTypeDef CAN_write(CanTxMsgTypeDef *txMsg);

void CAN_handleEvent(Event_p event);
