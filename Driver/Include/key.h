#ifndef _KEY_H
#define _KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_DEBOUNCE_TICK   (20U)        //消抖参数
#define KEY_IN_STATUS       gpio_input_bit_get(GPIOA,GPIO_PIN_0)

typedef enum
{
    KEY_NULL,
    KEY_WORLUP,
}Keypad_value_t;

void Key_Init(void);

void Keypad_state_machine(void);

Keypad_value_t Key_Scan(void);

#ifdef __cplusplus
}
#endif

#endif

