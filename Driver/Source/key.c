#include "drv_config.h"
#include "key.h"

static uint8_t key_status = 0;
// PA0 外接下拉电阻 按下为高
void Key_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
}

void Key_set_status(uint8_t status)
{
    key_status = status;
}

uint8_t Key_get_status(void)
{
    return key_status;
}



