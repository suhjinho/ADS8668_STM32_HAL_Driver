/*
 * ads8668.c
 *
 *  Created on: Aug 3, 2020
 *      Author: Falk
 */

#include "../inc/ads8668.h"

void ADS8668_SetRange(ADS8668_HandleTypeDef hads, uint8_t channel, uint8_t range) {
	uint16_t tx[4] = {0};
	uint16_t addr  = 0x05 + channel;
	uint16_t rw    = 0x01;

	tx[0] = (((addr << 1) + rw) << 8) + range;

	HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(hads->hspi, (uint8_t*)tx, 4);
	while(HAL_SPI_GetState(hads->hspi) == HAL_SPI_STATE_BUSY);
	HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_SET);
}

void ADS8668_Reset(ADS8668_HandleTypeDef hads) {
	HAL_GPIO_WritePin(hads->RST_PORT, hads->RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(hads->RST_PORT, hads->RST_PIN, GPIO_PIN_SET);
}

void ADS8668_ReadAutoScan(ADS8668_HandleTypeDef hads, uint16_t* channels) {
	uint16_t tx[4] = {0};
	uint16_t rx[4] = {0};

	for (uint8_t ch_index = 0; ch_index < 8; ch_index++)
	{
		HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_RESET);
		HAL_SPI_TransmitReceive_IT(hads->hspi, (uint8_t*)tx, (uint8_t*)rx, 4);
		while(HAL_SPI_GetState(hads->hspi) == HAL_SPI_STATE_BUSY);
		HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_SET);

		channels[ch_index] = rx[3] & rx[2]<<8;
	}
}