/*!
    \file    standard_winusb_core.c
    \brief   WinUSB class driver

    \version 2024-04-11, V2.3.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2024, LJY.

*/

#include "usbd_transc.h"
#include "standard_winusb_core.h"


#if (USE_USBD_WINUSB)

#define USBD_VID                     0x28E9U
#define USBD_PID                     0x0383U

usb_desc_dev winusb_dev_desc =
{
    .header = 
     {
         .bLength          = USB_DEV_DESC_LEN, 
         .bDescriptorType  = USB_DESCTYPE_DEV,
     },
    .bcdUSB                = 0x0200U,
    .bDeviceClass          = 0x00U,
    .bDeviceSubClass       = 0x00U,
    .bDeviceProtocol       = 0x00U,
    .bMaxPacketSize0       = USBD_EP0_MAX_SIZE,
    .idVendor              = USBD_VID,
    .idProduct             = USBD_PID,
    .bcdDevice             = 0x0200U,
    .iManufacturer         = STR_IDX_MFC,
    .iProduct              = STR_IDX_PRODUCT,
    .iSerialNumber         = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM,
};


usb_winusb_desc_config_set winusb_config_desc = 
{
    .config = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_config), 
             .bDescriptorType = USB_DESCTYPE_CONFIG,
         },
        .wTotalLength         = USB_WINUSB_CONFIG_DESC_SIZE,
        .bNumInterfaces       = 0x01U,
        .bConfigurationValue  = 0x01U,
        .iConfiguration       = 0x00U,
        .bmAttributes         = 0x80U,
        .bMaxPower            = 0x32U
    },

    .winusb_itf = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_itf), 
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber     = 0x00U,
        .bAlternateSetting    = 0x00U,
        .bNumEndpoints        = 0x02U,
        .bInterfaceClass      = USB_CLASS_WINUSB,
        .bInterfaceSubClass   = 0x00,
        .bInterfaceProtocol   = 0x00,
        .iInterface           = 0x00U
    },

    .winusb_out_endpoint = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = WINUSB_OUT_EP,
        .bmAttributes         = USB_EP_ATTR_BULK,
        .wMaxPacketSize       = CDC_WINUSB_PACKET_SIZE,
        .bInterval            = 0x00U
    },

    .winusb_in_endpoint = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = WINUSB_IN_EP,
        .bmAttributes         = USB_EP_ATTR_BULK,
        .wMaxPacketSize       = CDC_WINUSB_PACKET_SIZE,
        .bInterval            = 0x00U
    }
};
 
/* USB language ID Descriptor */
static usb_desc_LANGID usbd_language_id_desc = 
{
    .header = 
     {
         .bLength         = sizeof(usb_desc_LANGID), 
         .bDescriptorType = USB_DESCTYPE_STR
     },
    .wLANGID              = ENG_LANGID
};

/* USB manufacture string */
static usb_desc_str manufacturer_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(7U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'T', 'J', 'C', '-', 'L', 'J', 'Y'}
};

/* USB product string */
static usb_desc_str product_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(17U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'W', 'I', 'N', 'U', 'S', 'B', ' ', 'D', 'E', 'V', 'I', 'C', 'E', '-', 'L', 'J', 'Y'}
};

/* USB serial string */
static usb_desc_str serial_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(12U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     }
};

/* USB string descriptor set */
static uint8_t* usbd_winusb_strings[] = 
{
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc winusb_desc =
{
#ifdef LPM_ENABLED
    .bos_desc    = (uint8_t *)&USBD_BOSDesc,
#endif /* LPM_ENABLED */
    .dev_desc    = (uint8_t *)&winusb_dev_desc,
    .config_desc = (uint8_t *)&winusb_config_desc,
    .strings     = usbd_winusb_strings
};

/* local function prototypes ('static') */
static uint8_t winusb_init         (usb_dev *udev, uint8_t config_index);
static uint8_t winusb_deinit       (usb_dev *udev, uint8_t config_index);
static uint8_t winusb_req_handler  (usb_dev *udev, usb_req *req);
static void winusb_data_in         (usb_dev *udev, uint8_t ep_num);
static uint8_t winusb_ctlx_out     (usb_dev *udev);
static void winusb_data_out        (usb_dev *udev, uint8_t ep_num);

/*
    req_cmd      请求的命令的值 
    req_altset   接口的值  在请求设置接口时会调用
    uint8_t (*req_process)  (usb_dev *udev, usb_req *req);  在非标准设备请求和设备类请求中被调用

    uint8_t (*ctlx_in)      (usb_dev *udev);  仅在控制端点0发送IN事务时调用
*/
usb_class winusb_class = 
{
    .req_cmd        = 0,
    .req_altset     = 0,
    .req_process    = winusb_req_handler,
    .init           = winusb_init,
    .deinit         = winusb_deinit,
    .data_in        = winusb_data_in,
    .ctlx_out       = winusb_ctlx_out,
    .data_out       = winusb_data_out
};





/*Winusb OS 字符串描述符*/
const usb_winusb_osdesc_str winusb_os_string = 
{ 
    .header = 
    {
        .bLength = 0x12,
        .bDescriptorType = USB_DESCTYPE_STR,
    },

    .unicode_string = {'M', 'S', 'F', 'T', '1', '0', '0'},

    .vendor = USB_WINUSB_VENDOR_CODE,
    .padding = 0x00
}; 

/*Winusb 兼容ID特征描述符*/
const uint8_t bosfeature_desc[USB_LEN_OS_FEATURE_DESC] = 
{
   0x28, 0, 0, 0, // length
   0, 1,          // bcd version 1.0
   4, 0,          // windex: extended compat ID descritor
   1,             // no of function
   0, 0, 0, 0, 0, 0, 0, // reserve 7 bytes
   // function
   0,             // interface no
   0,             // reserved
   'W', 'I', 'N', 'U', 'S', 'B', 0, 0, //  first ID
   0,   0,   0,   0,   0,   0, 0, 0,  // second ID
   0,   0,   0,   0,   0,   0 // reserved 6 bytes    
};

/*Winusb 扩展属性描述符*/
const uint8_t bosproperty_desc[USB_LEN_OS_PROPERTY_DESC] = 
{
  0x8E, 0, 0, 0,  // length 246 byte
  0x00, 0x01,   // BCD version 1.0
  0x05, 0x00,   // Extended Property Descriptor Index(5)
  0x01, 0x00,   // number of section (1)
  //; property section        
  0x84, 0x00, 0x00, 0x00,// size of property section
  0x1, 0, 0, 0,   //; property data type (1)
  0x28, 0,        //; property name length (42)
  'D', 0,
  'e', 0,
  'v', 0,
  'i', 0,
  'c', 0,
  'e', 0,
  'I', 0,
  'n', 0,
  't', 0,
  'e', 0,
  'r', 0,
  'f', 0,
  'a', 0,
  'c', 0,
  'e', 0,
  'G', 0,
  'U', 0,
  'I', 0,
  'D', 0,
  0, 0,
  /* {13eb360b-bc1e-46cb-ac8b-ef3da47b4062} */  /*LJY:{12DE6357-6828-F819-6B8F-A334024A741E}*/
  0x4E, 0, 0, 0,  // ; property data length
  '{', 0,
  '1', 0,
  '3', 0,
  'E', 0,
  'B', 0,
  '3', 0,
  '6', 0,
  '0', 0,
  'B', 0,
  '-', 0,
  'B', 0,
  'C', 0,
  '1', 0,
  'E', 0,
  '-', 0,
  '4', 0,
  '6', 0,
  'C', 0,
  'B', 0,
  '-', 0,
  'A', 0,
  'C', 0,
  '8', 0,
  'B', 0,
  '-', 0,
  'E', 0,
  'F', 0,
  '3', 0,
  'D', 0,
  'A', 0,
  '4', 0,
  '7', 0,
  'B', 0,
  '4', 0,
  '0', 0,
  '6', 0,
  '2', 0,
  '}', 0,
    0, 0,
};


/**********************************************************************
* 设备类的初始化 初始化端点 关联输入输出事务函数 注册数据缓冲区
* 在枚举过程中 标准设备请求 设置配置中被调用
***********************************************************************/
static uint8_t winusb_init (usb_dev *udev, uint8_t config_index)
{
    static usb_winusb_handler winusb_handler;

    /* initialize the data endpoints */
    usbd_ep_init(udev, EP_BUF_SNG, BULK_TX_ADDR, &(winusb_config_desc.winusb_in_endpoint));
    usbd_ep_init(udev, EP_BUF_SNG, BULK_RX_ADDR, &(winusb_config_desc.winusb_out_endpoint));

    udev->ep_transc[EP_ID(WINUSB_IN_EP)][TRANSC_IN] = winusb_class.data_in;
    udev->ep_transc[WINUSB_OUT_EP][TRANSC_OUT] = winusb_class.data_out;


    udev->class_data[0] = (void *)&winusb_handler;
    winusb_handler.packet_receive = 0U;
    winusb_handler.packet_sent = 1U;
    winusb_handler.pre_packet_send = 1U; 
    winusb_handler.receive_length = 0U;

    return USBD_OK;
}

/**********************************************************************
* 解除设备初始化 注销端点
* 在枚举过程中 标准设备请求 设置配置中被调用
***********************************************************************/
static uint8_t winusb_deinit(usb_dev *udev, uint8_t config_index)
{
    /* deinitialize the data endpoints */
    usbd_ep_deinit(udev, WINUSB_IN_EP);
    usbd_ep_deinit(udev, WINUSB_OUT_EP);

    return USBD_OK;
}

/**********************************************************************
* 设备类请求处理函数 在非标准设备请求的设备类请求中调用
***********************************************************************/
static uint8_t winusb_req_handler(usb_dev *udev, usb_req *req)
{
    uint8_t status = REQ_NOTSUPP;

    switch (req->bRequest) 
    {
        case USB_GET_DESCRIPTOR:
            if(req->wValue == 0x03EE)//获取OS字符串描述符
            {
                usb_transc_config(&udev->transc_in[0U], (uint8_t *)&winusb_os_string, USB_MIN(0x12U,req->wLength), 0U);
                status = REQ_SUPP;
            }
        break;

        case USB_WINUSB_VENDOR_CODE:
            if(req->wIndex == 0x0004) //获取兼容ID描述符
            {
                usb_transc_config(&udev->transc_in[0U], (uint8_t *)&bosfeature_desc, USB_MIN(USB_LEN_OS_FEATURE_DESC,req->wLength), 0U);
                status = REQ_SUPP;
            }
            else if(req->wIndex == 0x0005) //获取扩展ID属性描述符
            {
                usb_transc_config(&udev->transc_in[0U], (uint8_t *)&bosproperty_desc, USB_MIN(USB_LEN_OS_PROPERTY_DESC,req->wLength), 0U);
                status = REQ_SUPP;
            }
        break;

        default:
        break;
    }

    return status;
}

/**********************************************************************
* 在收到IN令牌后开始传输数据阶段 在数据发送完才会调用，如果最后一次包数据为完整
* 的包，那么这个函数将补充0数据包表示结束，结束之后会再次调用这个函数以else结束
* 如果最后一个包为短包就直接else结束
***********************************************************************/
static void winusb_data_in(usb_dev *udev, uint8_t ep_num)
{
    usb_transc *transc = &udev->transc_in[ep_num];
    usb_winusb_handler *winusb = (usb_winusb_handler *)udev->class_data[0];

    if (transc->xfer_count == transc->max_len) {
        usbd_ep_send(udev, EP_ID(ep_num), NULL, 0U);//发送0数据包 
    } else {
        winusb->packet_sent = 1U;
        winusb->pre_packet_send = 1U;
    }
}

/**********************************************************************
* 仅在控制端点0收到OUT事务时调用 表示有来自主机的数据
***********************************************************************/
static uint8_t winusb_ctlx_out(usb_dev *udev)
{
    usb_winusb_handler *winusb = (usb_winusb_handler *)udev->class_data[0];

    winusb->packet_receive = 1U;
    winusb->pre_packet_send = 1U;

    return USBD_OK;
}

/**********************************************************************
* 在收到OUT令牌后的数据创数阶段 在所有数据被接收完后调用，如果数据流的长度大于
* 一个包的长度，需要uint8_t (*ctlx_out) 这个函数提前通知收起数据来实现分包
* 如果不分包的话，需要保证接收事务数据缓存区的递增地址的安全和数据处理的顺序
***********************************************************************/
static void winusb_data_out(usb_dev *udev, uint8_t ep_num)
{
    usb_winusb_handler *winusb = (usb_winusb_handler *)udev->class_data[0];

    winusb->packet_receive = 1U;//表示已经接收完了到来自主机的信息

    winusb->receive_length = udev->transc_out[ep_num].xfer_count;//获取信息长度
}


/**********************************************************************
* 
***********************************************************************/
void winusb_data_receive(usb_dev *udev)
{
    usb_winusb_handler *winusb = (usb_winusb_handler *)udev->class_data[0];
    //uint32_t data_len = winusb->receive_length;

    winusb->packet_receive = 0U;
    winusb->pre_packet_send = 0U;

    usbd_ep_recev(udev, WINUSB_OUT_EP, (uint8_t*)(winusb->data), CDC_WINUSB_PACKET_SIZE);
}


void winusb_data_send (usb_dev *udev)
{
    usb_winusb_handler *winusb = (usb_winusb_handler *)udev->class_data[0];
    uint32_t data_len = winusb->receive_length;

    if ((0U != data_len) && (1U == winusb->packet_sent)) {
        winusb->packet_sent = 0U;
        usbd_ep_send(udev, WINUSB_IN_EP, (uint8_t*)(winusb->data), (uint16_t)data_len);
        winusb->receive_length = 0U;
    }
}


uint8_t winusb_check_ready(usb_dev *udev)
{
    if (udev->class_data[0] != NULL) {
        usb_winusb_handler *winusb = (usb_winusb_handler *)udev->class_data[0];

        if ((1U == winusb->packet_receive) && (1U == winusb->pre_packet_send)) {
            return 0U;
        }
    }

    return 5U;
}


#endif
