#ifndef _KEY_H
#define _KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_IN_STATUS   gpio_input_bit_get(GPIOA,GPIO_PIN_0)

void Key_Init(void);

void Key_set_status(uint8_t status);

uint8_t Key_get_status(void);

#ifdef __cplusplus
}
#endif

#endif

