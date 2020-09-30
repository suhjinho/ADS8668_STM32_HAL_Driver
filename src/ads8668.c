/*
 * ads8668.c
 *
 *  Created on: Aug 3, 2020
 *      Author: Falk
 */

#include "../inc/ads8668.h"


static uint8_t tx[4] = {0};
static uint8_t rx[4] = {0};

void ADS8668_SetRange(ADS8668_HandleTypeDef* hads, uint8_t channel, uint8_t range) {
	uint16_t addr  = 0x05 + channel;
	uint16_t rw    = 0x01;

	tx[0] = (((addr << 1) + rw) << 8) + range;

	HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit_IT(hads->hspi, (uint8_t*)tx, 4);
	while(hads->hspi->State != HAL_SPI_STATE_READY);
	HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_SET);
}

void ADS8668_Reset(ADS8668_HandleTypeDef* hads) {
	HAL_GPIO_WritePin(hads->RST_PORT, hads->RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(hads->RST_PORT, hads->RST_PIN, GPIO_PIN_SET);
}

void ADS8668_AutoReset(ADS8668_HandleTypeDef* hads) {
	HAL_GPIO_WritePin(hads->RST_PORT, hads->RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(hads->RST_PORT, hads->RST_PIN, GPIO_PIN_SET);
}

void ADS8668_ReadAutoScan(ADS8668_HandleTypeDef* hads, uint16_t* channels) {
	bzero(tx, sizeof(uint8_t)*4);
	tx[0] = 0xA0; // Auto Reset
	for (uint8_t ch_index = 0; ch_index < 8; ch_index++) {
		HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_RESET);

		HAL_StatusTypeDef status = HAL_BUSY;
		while(status != HAL_OK) {
			status = HAL_SPI_Transmit_IT(hads->hspi, (uint8_t*)tx, 2);
		}
		while(HAL_SPI_GetState(hads->hspi) != HAL_SPI_STATE_READY);

		HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_SET);

		while(status != HAL_OK) {
			status = HAL_SPI_Receive_IT(hads->hspi, (uint8_t*)rx, 2);
		}
		while(HAL_SPI_GetState(hads->hspi) != HAL_SPI_STATE_READY);

		bzero(tx, sizeof(uint8_t)*4); // Continue Auto-Scan

		channels[ch_index] = rx[0] & rx[1]<<8;
	}
}
