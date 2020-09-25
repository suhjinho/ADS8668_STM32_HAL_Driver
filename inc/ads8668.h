/*
 * ads8668.h
 *
 *  Created on: Aug 3, 2020
 *      Author: Falk
 */

#include <stm32f3xx_hal.h>
#include <string.h>

#ifndef INC_ADS8668_H_
#define INC_ADS8668_H_

#define ADS8668_RANGE_10V24_BI	0b0000
#define ADS8668_RANGE_5V12_BI	0b0001
#define ADS8668_RANGE_2V56_BI	0b0010
#define ADS8668_RANGE_1V28_BI	0b0011
#define ADS8668_RANGE_0V64_BI	0b1011
#define ADS8668_RANGE_10V24		0b0101
#define ADS8668_RANGE_5V12		0b0110
#define ADS8668_RANGE_2V56		0b0111
#define ADS8668_RANGE_1V28		0b1111

struct ads8668_handler {
	SPI_HandleTypeDef* hspi;
	GPIO_TypeDef* CS_PORT;
	uint16_t CS_PIN;
	GPIO_TypeDef* RST_PORT;
	uint16_t RST_PIN;
};

typedef struct ads8668_handler ADS8668_HandleTypeDef;

void ADS8668_SetRange(ADS8668_HandleTypeDef* hads, uint8_t channel, uint8_t range);
void ADS8668_Reset(ADS8668_HandleTypeDef* hads);
void ADS8668_ReadAutoScan(ADS8668_HandleTypeDef* hads, uint16_t* channels);


#endif /* INC_ADS8668_H_ */
