#ifndef _LED_H
#define _LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_config.h"

#define LED2_ON     gpio_bit_set(GPIOB, GPIO_PIN_13)
#define LED3_ON     gpio_bit_set(GPIOB, GPIO_PIN_14)
#define LED4_ON     gpio_bit_set(GPIOB, GPIO_PIN_15)
#define LED5_ON     gpio_bit_set(GPIOA, GPIO_PIN_8)

#define LED2_OFF    gpio_bit_reset(GPIOB, GPIO_PIN_13)
#define LED3_OFF    gpio_bit_reset(GPIOB, GPIO_PIN_14)
#define LED4_OFF    gpio_bit_reset(GPIOB, GPIO_PIN_15)
#define LED5_OFF    gpio_bit_reset(GPIOA, GPIO_PIN_8)

void LED_Init(void);

#ifdef __cplusplus
}
#endif

#endif
