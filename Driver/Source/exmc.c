#include "drv_config.h"
#include "exmc.h"

/******************************************************************
 * 总共1G的地址区域 分为4个Bank  4 * 256M;
 * Bank0 支持NOR FLASH / PSRAM Bank0 addr:0x6000 0000
 * Bank0 又分4个Region   4 * 64M
 * 每个Region都有独立的片选信号 意味分区的选择可以使用多个设备
 * PORT：
******************************************************************/


void EXMC_Init(void)
{
    rcu_periph_clock_enable(RCU_EXMC);

    exmc_norsram_parameter_struct psram_init_struct;//总配置
    exmc_norsram_timing_parameter_struct nor_timing_init_struct;//扩展模式配置

    nor_timing_init_struct.asyn_access_mode = EXMC_ACCESS_MODE_A;           // 异步访问模式
    nor_timing_init_struct.syn_data_latency = EXMC_DATALAT_2_CLK;           // 配置数据延迟
    nor_timing_init_struct.syn_clk_division = EXMC_SYN_CLOCK_RATIO_DISABLE; // 时钟分配比 不分频
    nor_timing_init_struct.bus_latency = 1;                                 // 总线延迟时间
    nor_timing_init_struct.asyn_data_setuptime = 7;                         // 数据设置时间 异步访问有效
    nor_timing_init_struct.asyn_address_holdtime = 2;                       // 地址保持时间 异步访问有效
    nor_timing_init_struct.asyn_address_setuptime = 5;                      // 数据设置时间 异步访问有效

    psram_init_struct.norsram_region = EXMC_BANK0_NORSRAM_REGION0; // 选择Bank0的区域0 64M
    psram_init_struct.write_mode = EXMC_SYN_WRITE;                 // 同步写模式
    psram_init_struct.extended_mode = DISABLE;                     // 启用扩展模式
    psram_init_struct.asyn_wait = DISABLE;                         // 启用异步等待
    psram_init_struct.nwait_signal = DISABLE;                      // 在同步中断模式下启用或禁用NWAIT信号
    psram_init_struct.memory_write = ENABLE;                       // 启用写操作
    psram_init_struct.nwait_config = EXMC_NWAIT_CONFIG_BEFORE;     // NWAIT信号配置
    psram_init_struct.wrap_burst_mode = DISABLE;                   // 自动换行模式
    psram_init_struct.nwait_polarity = EXMC_NWAIT_POLARITY_LOW;    // 指定内存中NWAIT信号的极性
    psram_init_struct.burst_mode = DISABLE;                        // 连拍模式
    psram_init_struct.databus_width = EXMC_NOR_DATABUS_WIDTH_8B;   // 外部存储器数据总线宽度
    psram_init_struct.memory_type = EXMC_MEMORY_TYPE_PSRAM;        // 外部储存器的类型
    psram_init_struct.address_data_mux = ENABLE;                   // 地址总线和数据总线是否复用
    psram_init_struct.read_write_timing = &nor_timing_init_struct; // 不使用扩展模式时读和写的定时参数，使用扩展模式时读的定时参数
    psram_init_struct.write_timing = &nor_timing_init_struct;      // 使用扩展模式时读的定时参数

    exmc_norsram_init(&psram_init_struct);

    /* Enable EXMC bank */
    exmc_norsram_enable(EXMC_BANK0_NORSRAM_REGION0);


}


