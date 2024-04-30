#include "drv_config.h"
#include "led.h"
#include "usart.h"

USARTINFO_T USART0_INFO = {0};

void USART0_Init(uint32_t bps)
{
	// 使能串口1时钟
	rcu_periph_clock_enable(RCU_USART0);
	rcu_periph_clock_enable(RCU_GPIOA);

	// 配置串口1的 GPIO 引脚
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);		 // TX Pin
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10); // RX Pin

	// 配置串口1的参数
	usart_deinit(USART0);
	usart_baudrate_set(USART0, bps);
	usart_word_length_set(USART0, USART_WL_8BIT); // 数据位
	usart_stop_bit_set(USART0, USART_STB_1BIT);	  // 停止位
	usart_parity_config(USART0, USART_PM_NONE);	  // 无奇偶校验
	usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	usart_interrupt_enable(USART0, USART_INT_RBNE);
	usart_interrupt_enable(USART0, USART_INT_IDLE);
	usart_interrupt_enable(USART0, USART_INT_TC);

	usart_enable(USART0);

	nvic_irq_enable(USART0_IRQn, 2, 2);
}

void USART0_IRQHandler(void)
{
	if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) == SET)
	{
		USART0_INFO.Rxbuff[USART0_INFO.Pos++] = usart_data_receive(USART0); // 软件可以通过对该位写0或读USART_DATA寄存器来将该位清0
		LED3_ON;
	}
	if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE) == SET)
	{
		usart_flag_get(USART0, USART_FLAG_IDLEF); // 软件先读USART_STAT，再读USART_DATA可清除该位。
		usart_data_receive(USART0);
		USART0_INFO.Rxbuff[USART0_INFO.Pos] = '\0';
		USART0_INFO.Pos = 0;
		USART0_INFO.Idleflag = 1;
		printf("USART0_RX:%s\n", USART0_INFO.Rxbuff);
	}
	if (usart_interrupt_flag_get(USART0, USART_INT_FLAG_TC) == SET)
	{
		usart_interrupt_flag_clear(USART0,USART_INT_FLAG_TC);
		USART0_INFO.Tcfalg = 1;
	}
}

// 重定向 printf 函数到串口1
int fputc(int c, FILE *stream)
{
	usart_data_transmit(USART0, (uint8_t)c);
	LED2_ON;
	while (usart_flag_get(USART0, USART_FLAG_TBE) == RESET);
	return c;
}
