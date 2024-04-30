#include "Periphery.h"
#include "psram.h"

/*ESP-PSRAM64H: SPI/QPI 接⼝包含⼀个⽤于启动⾃初始化的⽚上电压传感器。当 Vcc 达到或⾼于最⼩ Vcc 的稳定
电平时，芯⽚需要 150 μs 并且⽤户需要进⾏复位操作（⻅第 8 章）才能完成⾃初始化过程。从上电开始到 150 μs 周期结束，
CLK 应保持低电平，CE# 应保持⾼电平（以便跟踪在 200 mV 以内的 Vcc），SI/SO/SIO[3:0] 应保持低电平。
150 μs 后，芯⽚可以正常运⾏*/


/*****************************************************************************
 * 64Mbit 8Mbyte 寻址空间为8M字节 地址从4开始 页面大小为1K
 * 模式有两种：线性突发模式和32字节wrap模式 默认是线性突发模式
 * 线性突发模式：线性突发模式允许设备跨越⻚⾯边界。
 * ⻚⾯边界跨越对存储器控制器是不可⻅的，并且最⾼时钟限制在 84 MHz。
 * 32字节wrap：不允许跨越⻚⾯边界。
 * 
 * 目前SPI0是速度最快的 上限到60M左右 不用担心超出芯片硬件上限
 * 读写测试均通过 PSRAM的读写只有单线和四线 读的话速度分普通和高频 
*****************************************************************************/

void PSRAM_Init(void)
{

    SPI_CSS_H;
    delay_1ms(10);
    PSRAM_ReadID();

}

//MFID:0x0D KGDID:0x5D
void PSRAM_ReadID(void)
{
    uint8_t MF_ID,KGD_ID;

    SPI_CSS_L;

    SPI_transfer_Byte(0x9F);
    SPI_transfer_Byte(0x00);
    SPI_transfer_Byte(0x00);
    SPI_transfer_Byte(0x00);
    MF_ID = SPI_transfer_Byte(0x00);
    KGD_ID = SPI_transfer_Byte(0x00);
    printf("MF_ID:%02x,KGD_ID:%02x\n",MF_ID,KGD_ID);
    if(MF_ID == 0x0D && KGD_ID == 0x5D)
    {
        printf("PSRAM[info]:proper functioning\n");
    }
    else
    {
        printf("PSRAM[error]:device abnormal\n");
    }

    SPI_CSS_H;
}

//PSRAM 普通读 最高33M
void PSRAM_Read_Data(uint32_t addr,uint8_t *Rxbuff,uint32_t len)
{
    uint32_t i;

    SPI_CSS_L;

    SPI_transfer_Byte(0x03);
    SPI_transfer_Byte(addr >> 16);
    SPI_transfer_Byte(addr >> 8);
    SPI_transfer_Byte(addr & 0xFF);
    for(i = 0;i < len;i++)
    {
        Rxbuff[i] = SPI_transfer_Byte(0x00);
    }

    SPI_CSS_H;
}

//PSRAM 快速读 最⾼频率：ESP-PSRAM64 为 144 MHz，ESP-PSRAM64H 为 133 MHz
void PSRAM_ReadEx_Data(uint32_t addr,uint8_t *Rxbuff,uint32_t len)
{
    uint32_t i;

    SPI_CSS_L;

    SPI_transfer_Byte(0x0B);
    SPI_transfer_Byte(addr >> 16);
    SPI_transfer_Byte(addr >> 8);
    SPI_transfer_Byte(addr & 0xFF);
    SPI_transfer_Byte(0x00);//wait 8 clock
    for(i = 0;i < len;i++)
    {
        Rxbuff[i] = SPI_transfer_Byte(0x00);
    }

    SPI_CSS_H;
}

//PSRAM 写操作 最⾼频率：ESP-PSRAM64 为 144 MHz，ESP-PSRAM64H 为 133 MHz
void PSRAM_Write_Date(uint32_t addr,uint8_t *Wxbuff,uint32_t len)
{
    uint32_t i;

    SPI_CSS_L;

    SPI_transfer_Byte(0x02);
    SPI_transfer_Byte(addr >> 16);
    SPI_transfer_Byte(addr >> 8);
    SPI_transfer_Byte(addr & 0xFF);
    for(i = 0;i < len;i++)
    {
        SPI_transfer_Byte(Wxbuff[i]);
    }

    SPI_CSS_H;
}
