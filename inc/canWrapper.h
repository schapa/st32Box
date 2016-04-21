#pragma once

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef CAN_init(CAN_HandleTypeDef *handle);
HAL_StatusTypeDef CAN_write(CanTxMsgTypeDef *txMsg);
