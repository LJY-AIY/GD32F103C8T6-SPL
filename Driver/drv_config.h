#ifndef _DRV_CONFIG_H
#define _DRV_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32f10x.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/********************************************************
Driver config 
*********************************************************/

/***************************SPI config***********************************/
#define SPI_DEVICE      SPI0
#define SPI_CLOCK       RCU_SPI0
#define SPI_CS_GPIO     GPIOA
#define SPI_CS_PIN      GPIO_PIN_4
#define SPI_CLK_GPIO    GPIOA
#define SPI_CLK_PIN     GPIO_PIN_5
#define SPI_MISO_GPIO   GPIOA     
#define SPI_MISO_PIN    GPIO_PIN_6
#define SPI_MOSI_GPIO   GPIOA
#define SPI_MOSI_PIN    GPIO_PIN_7


#ifdef __cplusplus
}
#endif


#endif
