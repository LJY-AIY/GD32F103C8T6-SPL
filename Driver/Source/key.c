#include "drv_config.h"
#include "key.h"
/*物理按键控制参数 由状态机管理 显示的存在触发状态*/
static uint8_t work_key_status = 0;


/*物理按键初始化*/
// PA0 外接下拉电阻 按下为高
void Key_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
}

//状态机处理函数
static uint8_t work_key_tick = 0;
void Keypad_state_machine(void)
{
    if(KEY_IN_STATUS)
    {
       work_key_tick++;
    }
    else
    {
        work_key_tick = 0;
        work_key_status = 0;
    }


    if(work_key_tick >= KEY_DEBOUNCE_TICK)
    {
        work_key_status = 1;
        work_key_tick = 0;
    }
}

//应用扫描函数
Keypad_value_t Key_Scan(void)
{
    static uint8_t work_key_flag = 1;
    if(work_key_status && work_key_flag)
    {
        work_key_flag = 0;
        return KEY_WORLUP;
    }
    else
    {
        if(!work_key_status && !work_key_flag)
        {
            work_key_flag = 1;
        }
    }

    return KEY_NULL;
}



