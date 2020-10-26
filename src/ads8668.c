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
	tx[1] = (addr << 1) | rw;
	tx[0] = range;

	HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive_IT(hads->hspi, (uint8_t*)tx, rx, 2);
	while(hads->hspi->State != HAL_SPI_STATE_READY);
	HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_SET);
}

void ADS8668_Reset(ADS8668_HandleTypeDef* hads) {
	// Chip de-select
	HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(hads->RST_PORT, hads->RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(hads->RST_PORT, hads->RST_PIN, GPIO_PIN_SET);
	HAL_Delay(1);
}

void ADS8668_AutoScanReset(ADS8668_HandleTypeDef* hads) {
	bzero(tx, sizeof(uint8_t)*4);
	tx[1] = 0xA0; // Auto Reset

	// Chip select
	HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_RESET);

	// Write command
	HAL_StatusTypeDef status = HAL_BUSY;
	while(status != HAL_OK) {
		status = HAL_SPI_Transmit_IT(hads->hspi, (uint8_t*)tx, 2);
	}
	while(HAL_SPI_GetState(hads->hspi) != HAL_SPI_STATE_READY);

	// Chip de-select
	HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_SET);

	// Depricated data
	while(status != HAL_OK) {
		status = HAL_SPI_Receive_IT(hads->hspi, (uint8_t*)rx, 2);
	}
	while(HAL_SPI_GetState(hads->hspi) != HAL_SPI_STATE_READY);
}

void ADS8668_ReadAutoScan(ADS8668_HandleTypeDef* hads, uint16_t* channels) {
	ADS8668_AutoScanReset(hads);

	bzero(tx, sizeof(uint8_t)*4);
	for (uint8_t ch_index = 0; ch_index < 8; ch_index++) {
		HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_RESET);

		HAL_StatusTypeDef status = HAL_BUSY;
		while(status != HAL_OK) {
			status = HAL_SPI_TransmitReceive_IT(hads->hspi, tx, rx, 4);
		}
		while(HAL_SPI_GetState(hads->hspi) != HAL_SPI_STATE_READY);

		HAL_GPIO_WritePin(hads->CS_PORT, hads->CS_PIN, GPIO_PIN_SET);

		bzero(tx, sizeof(uint8_t)*4); // Continue Auto-Scan

		channels[ch_index] = (rx[3]<<8 | rx[2]) >> 4;
		//channels[ch_index] &= 0xFFF;
	}
}
