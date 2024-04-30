#include "drv_config.h"
#include "spi.h"

void SPI_Init(void)
{
    // 使能 SPI 时钟和 GPIO 时钟
    rcu_periph_clock_enable(SPI_CLOCK);
    rcu_periph_clock_enable(RCU_GPIOA);

    // 配置 SPI CS 引脚
    gpio_init(SPI_CS_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_CS_PIN);

    // 配置 SPI CLK 引脚
    gpio_init(SPI_CLK_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, SPI_CLK_PIN);

    // 配置 SPI MISO 引脚
    gpio_init(SPI_MISO_GPIO, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI_MISO_PIN);

    // 配置 SPI MOSI 引脚
    gpio_init(SPI_MOSI_GPIO, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, SPI_MOSI_PIN);

    // 配置 SPI 参数
    spi_parameter_struct spi_init_struct;
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;         // 全双工模式
    spi_init_struct.device_mode = SPI_MASTER;                      // 主机模式
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;               // 数据帧大小为8位
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE; // 时钟极性和相位  MODE:0
    spi_init_struct.nss = SPI_NSS_SOFT;                            // 软件管理 NSS
    spi_init_struct.endian = SPI_ENDIAN_MSB;                       // 数据传输的端序
    spi_init_struct.prescale = SPI_PSC_2;                          // 时钟分频 Max:108M
    spi_init(SPI_DEVICE, &spi_init_struct);

    // 使能 SPI
    spi_enable(SPI_DEVICE);
}

uint8_t SPI_transfer_Byte(uint8_t data)
{
    while (RESET == spi_i2s_flag_get(SPI_DEVICE, SPI_FLAG_TBE));
    spi_i2s_data_transmit(SPI_DEVICE,data);
    while (RESET == spi_i2s_flag_get(SPI_DEVICE, SPI_FLAG_RBNE));
    return spi_i2s_data_receive(SPI_DEVICE);
}
