#include "Driver.h"
#include "drv_config.h"
#include "usbd_conf.h"
#include "cdc_acm_core.h"
#include "standard_hid_core.h"
#include "standard_winusb_core.h"
#include "usbd.h"

/******************************************************************
 * 必须保证USB设备控制器的时钟为48M，否则无法正常工作
******************************************************************/
extern void usbd_isr (void);

static uint8_t GetSysemtRest(void);
static void SetSerialString(char *desc);

static usb_dev usbd_cdc;    //USBD相关设备信息结构体,将保存整个设备和数据结构的信息
static usb_desc *desc;      //设备、配置、电源以及其他描述符集合体
static usb_class *class;    //设备类相关规范

#if (USE_USBD_HIDKEY)
    static void Key_config(void);
    static void hid_key_data_send(usb_dev *udev);
    hid_fop_handler hid_fop;
#endif

void UBSD_Init(void)
{
    uint32_t system_clock;
    uint8_t power_reset_flag;

    rcu_periph_clock_enable(RCU_AHBPeriph_GPIO_PULLUP);//使能检测引脚IO时钟

    // USBD时钟配置
    system_clock= rcu_clock_freq_get(CK_SYS);//获取系统主频 根据主频设置USBD分频系数
    if (system_clock == 48000000U) {
        rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV1);
    } else if (system_clock == 72000000U) {
        rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV1_5);
    } else if (system_clock == 96000000U) {
        rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV2);
    } else {
        /*  reserved  */
    }

    // USBD设备选择
#if (USE_USBD_CDCACM)
    desc = &cdc_desc;
    class = &cdc_class;
#endif
#if (USE_USBD_HIDKEY)
    desc = &hid_desc;
    class = &hid_class;
    hid_fop.hid_itf_config = Key_config;
    hid_fop.hid_itf_data_process = hid_key_data_send;
    hid_itfop_register(&usbd_cdc,&hid_fop);
#endif
#if (USE_USBD_WINUSB)
    desc = &winusb_desc;
    class = &winusb_class;
#endif

    rcu_periph_clock_enable(RCU_USBD);//使能USBD外设时钟

    //gpio_init(USB_PULLUP, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, USB_PULLUP_PIN);//初始化IO检测引脚为推挽输出
		
	//SetSerialString("LJY CDC-ACM USB2.0");
    
    usbd_init(&usbd_cdc, desc, class);//初始化USBD硬件控制器 并开启对应的中断

    /*共4位可控优先级 设置1位配置抢占优先级 3位配置响应优先级*/
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    /*使能中断源 并设置其抢占和响应优先级*/
    nvic_irq_enable((uint8_t)USBD_LP_CAN0_RX0_IRQn, 1U, 0U);

    //usbd_connect(&usbd_cdc);//上拉检测IO,等待主机枚举

    power_reset_flag = GetSysemtRest();

    while (USBD_CONFIGURED != usbd_cdc.cur_status && power_reset_flag) {
        /* 等待枚举完成 */
    }

#if (USE_USBD_CDCACM)
    uint8_t usbd_info[] = "This print message comes from USBD CDC_ACD Deivce\n";
    usb_cdc_handler *cdc = (usb_cdc_handler *)usbd_cdc.class_data[CDC_COM_INTERFACE];
    memcpy(cdc->data,usbd_info,sizeof(usbd_info) - 1);
    cdc->receive_length = sizeof(usbd_info) - 1;
    cdc_acm_data_send(&usbd_cdc);
#endif

}

/******************************************************************
* USB低优先级中断（通道20）：可被所有USB事件触发
******************************************************************/
void USBD_LP_CAN0_RX0_IRQHandler(void)
{
    usbd_isr();
}

/******************************************************************
* 这个函数在每次复位后都得到运行,复位标志寄存器将会被清除
* 标志位可以进行有效的判断从而不受影响
* 返回1表示一定发生了电源复位，而不是因为下载程序和外部复位
******************************************************************/
static uint8_t GetSysemtRest(void)
{
    uint32_t rstsck = 0;
    uint32_t power_flag = 0;
    
    rstsck = RCU_RSTSCK;
    power_flag = (rstsck & (1 << 27));
    rcu_all_reset_flag_clear();

    if(power_flag)
    {
        printf("USBD[info]:Waiting for the enumeration to complete\n");
        return 1;
    }
    else
    {
        printf("USBD[info]:No wait enum\n");
        return 0;
    }
}

/**********************************************************************
小端序UTF-8字符串转换为UTF-16字符串
参数：
    utf8_string: 输入的UTF-8字符串
    utf16_buffer: 用于存储转换后的UTF-16字符串的缓冲区
    buffer_size: 缓冲区大小（以字节为单位）
返回值：
    返回成功转换的字符数，不包括结尾的null字符
    如果转换失败或缓冲区太小，则返回-1
***********************************************************************/
int utf8_to_utf16_le(const char* utf8_string, uint16_t* utf16_buffer, size_t buffer_size) 
{
    if (utf8_string == NULL || utf16_buffer == NULL || buffer_size == 0)
        return -1;

    size_t utf16_length = 0;
    uint16_t* utf16_ptr = utf16_buffer;

    while (*utf8_string != '\0') {
        uint32_t codepoint = 0;

        // 读取UTF-8字符的第一个字节
        unsigned char ch = *utf8_string;

        // 根据UTF-8字符的第一个字节确定字符长度
        if ((ch & 0x80) == 0) {
            // 单字节字符
            codepoint = ch;
            utf8_string++;
        } else if ((ch & 0xE0) == 0xC0) {
            // 两字节字符
            if ((utf8_string[1] & 0xC0) != 0x80)
                return -1; // 非法UTF-8序列
            codepoint = ((ch & 0x1F) << 6) | (utf8_string[1] & 0x3F);
            utf8_string += 2;
        } else if ((ch & 0xF0) == 0xE0) {
            // 三字节字符
            if ((utf8_string[1] & 0xC0) != 0x80 || (utf8_string[2] & 0xC0) != 0x80)
                return -1; // 非法UTF-8序列
            codepoint = ((ch & 0x0F) << 12) | ((utf8_string[1] & 0x3F) << 6) | (utf8_string[2] & 0x3F);
            utf8_string += 3;
        } else {
            // 非法UTF-8序列
            return -1;
        }

        // 如果缓冲区不够大，提前结束转换
        if (utf16_length + 1 > buffer_size / sizeof(uint16_t))
            return -1;

        // 写入UTF-16字符到缓冲区
        if (codepoint <= 0xFFFF) {
            *utf16_ptr++ = (uint16_t)codepoint;
            utf16_length++;
        } else {
            // 需要使用代理对
            if (utf16_length + 2 > buffer_size / sizeof(uint16_t))
                return -1; // 缓冲区不够大，提前结束转换
            codepoint -= 0x10000;
            *utf16_ptr++ = (uint16_t)((codepoint >> 10) + 0xD800);
            *utf16_ptr++ = (uint16_t)((codepoint & 0x3FF) + 0xDC00);
            utf16_length += 2;
        }
    }

    // 添加结尾的null字符
    *utf16_ptr = 0;

    return utf16_length;
}


/**********************************************************************
* CDCACM function
***********************************************************************/
#if (USE_USBD_CDCACM)
/**********************************************************************
* 修改字符串标识符中的串行字符串标识符 默认是没有信息 PC显示为：USB串行设备
***********************************************************************/
static void SetSerialString(char *desc)
{
    uint8_t len;
    usb_desc_str *str;

    len = strlen(desc);
    if(len >= 64)
    {
        return;
    }
    else
    {
        str = (usb_desc_str *)cdc_desc.strings[2];
        memset(str->unicode_string,0,sizeof(str->unicode_string));
        utf8_to_utf16_le(desc,str->unicode_string,sizeof(str->unicode_string));
        str->header.bLength = USB_STRING_LEN(len);
    }
}

#endif

/**********************************************************************
* HIDKEY function
***********************************************************************/
#if (USE_USBD_HIDKEY)

enum keyboard
{
    CHAR_A = 1,
    CHAR_B,
    CHAR_C,
    CHAR_D,
    CHAR_E,
    CHAR_F
};

struct gpio_config
{
    uint32_t gpio_periph;
    uint32_t pin;
};

static struct gpio_config gpio_info[] = {
    [CHAR_A - 1] = {GPIOC, GPIO_PIN_13},
    [CHAR_B - 1] = {GPIOA, GPIO_PIN_1},
    [CHAR_C - 1] = {GPIOA, GPIO_PIN_2},
    [CHAR_D - 1] = {GPIOA, GPIO_PIN_3},
    [CHAR_E - 1] = {GPIOA, GPIO_PIN_4},
    [CHAR_F - 1] = {GPIOA, GPIO_PIN_5}
};

/**********************************************************************
* 物理键盘硬件初始化
***********************************************************************/
static void Key_config(void)
{
    uint8_t i;

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);

    for(i = 0;i < (sizeof(gpio_info) / sizeof(gpio_info[0]));i++)
    {
        gpio_init(gpio_info[i].gpio_periph, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, gpio_info[i].pin);//初始化IO检测引脚为推挽输出
    }
}

/**********************************************************************
* 物理键盘状态检测函数
***********************************************************************/
static uint8_t key_status(void)
{
    uint8_t i;

    for(i = 0;i < (sizeof(gpio_info) / sizeof(gpio_info[0]));i++)
    {
        if(gpio_input_bit_get(gpio_info[i].gpio_periph,gpio_info[i].pin) == RESET)
        {
            return i + 1;
        }
    }

    return 0;
}

/**********************************************************************
* HID数据请求报告函数
***********************************************************************/
static void hid_key_data_send(usb_dev *udev)
{
    standard_hid_handler *hid = (standard_hid_handler *)udev->class_data[USBD_HID_INTERFACE];

    if (hid->prev_transfer_complete) {
        switch (key_status()) {
        case CHAR_A:
            hid->data[2] = 0x04U;
            break;
        case CHAR_B:
            hid->data[2] = 0x05U;
            break;
        case CHAR_C:
            hid->data[2] = 0x06U;
            break;
        case CHAR_D:
            hid->data[2] = 0x07U;
            break;
        case CHAR_E:
            hid->data[2] = 0x08U;
            break;
        case CHAR_F:
            hid->data[2] = 0x09U;
            break;
        default:
            break;
        }

        if (0U != hid->data[2]) {
            hid_report_send(udev, hid->data, HID_IN_PACKET);
        }
    }
}

#endif


/**********************************************************************
* USB  轮询核心函数 可作用多种类型设备
***********************************************************************/
void USBD_Kernel(void)
{
#if (USE_USBD_CDCACM)
    if (cdc_acm_check_ready(&usbd_cdc) == 0) // 当有CDC的数据接收到
    {
        cdc_acm_data_receive(&usbd_cdc); // 接收CDC数据，一次最多不超过64字节
    }
    else
    {
        cdc_acm_data_send(&usbd_cdc);
    }
#endif

#if (USE_USBD_HIDKEY)
    hid_fop.hid_itf_data_process(&usbd_cdc);
#endif

#if (USE_USBD_WINUSB)
    static usb_winusb_handler *winusb;
    static uint8_t flag = 0;
    static uint32_t pack_coumt = 0;
    static uint8_t usbd_info[] = {0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8};
    winusb = (usb_winusb_handler *)usbd_cdc.class_data[0];
    if (winusb_check_ready(&usbd_cdc) == 0) // 当有CDC的数据接收到
    {
        winusb_data_receive(&usbd_cdc); // 接收CDC数据，一次最多不超过64字节
        flag = 1;
    }
    else
    {
        if(winusb->receive_length != 0)
        {
            for(uint8_t i = 0;i < winusb->receive_length;i++)
            {
                winusb->data[i] = ~winusb->data[i];
                printf("%02x ",winusb->data[i]);
            }
        }
        winusb_data_send(&usbd_cdc);
    }
#if 0
    if(flag)
    {
        if(winusb->receive_length != 0)
        {
            printf("pack:%d\n",pack_coumt);
            for(uint32_t i = 0;i < winusb->receive_length;i++)
            {
                if(i % 32 == 0 && i != 0)
                {
                    printf("\n");
                }
                printf("%02x ",*(winusb->data + i));
            }
            printf("\n");
            winusb->pre_packet_send = 1;
            winusb->receive_length = 0;
            pack_coumt += 1;
            flag = 0;
        } 
    }

    if(Key_get_status() && flag == 0)
    {
        Key_set_status(0);
        memcpy(winusb->data,usbd_info,sizeof(usbd_info));
        winusb->receive_length = sizeof(usbd_info);
        winusb_data_send(&usbd_cdc);
        memset(winusb->data,0,64);
    }
#endif

#endif
}




