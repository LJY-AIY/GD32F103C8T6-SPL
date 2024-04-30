/*!
    \file    standard_winusb_core.h
    \brief   WinUSB class driver

    \version 2024-04-11, V2.3.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2024, LJY.

*/
#ifndef _STANDRARD_WINUSB_CORE_H
#define _STANDRARD_WINUSB_CORE_H

#include "usbd_enum.h"

#if (USE_USBD_WINUSB)

/* communications device class code */
#define USB_CLASS_WINUSB                0xFFU

/*描述符号长度定义*/
#define USB_WINUSB_CONFIG_DESC_SIZE     0x20U
#define USB_LEN_OS_FEATURE_DESC         0x28U
#define USB_LEN_OS_PROPERTY_DESC        0x8EU

/*特定类的厂商代码*/
#define USB_WINUSB_VENDOR_CODE          0xA0U

/*WIN USB类代码*/
#define GET_MS_DESCRIPTOR               0xC0U

#pragma pack(1)

/* CDC ACM line coding structure */
typedef struct {
    uint32_t dwDTERate;                   /*!< data terminal rate */
    uint8_t  bCharFormat;                 /*!< format bits */
    uint8_t  bParityType;                 /*!< parity */
    uint8_t  bDataBits;                   /*!< data bits */
} acm_line;

#define NO_CMD                            0xFFU

// 配置描述符集结构
typedef struct
{
    usb_desc_config config;
    usb_desc_itf winusb_itf;
    usb_desc_ep winusb_out_endpoint;
    usb_desc_ep winusb_in_endpoint;
} usb_winusb_desc_config_set;

// Winusb OS 字符串描述符结构
typedef struct
{
    usb_desc_header header;
    uint16_t unicode_string[7];
    uint8_t vendor;
    uint8_t padding;
} usb_winusb_osdesc_str;

// Winusb 兼容ID特征描述符结构
typedef struct
{
    uint32_t length;            // length(LE 4)
    uint16_t version;           // bcd version(LE 2)
    uint16_t windex;            // extended compat ID descritor L:index  H:ID
    uint8_t numsect;
    uint8_t reserved1[7];       // default:0x00
    uint8_t iftnum;
    uint8_t reserved2;          // default:0x01
    uint8_t compatibleid[8];
    uint8_t sub_compatibleid[8];
    uint8_t reserved3[6];       // default:0x00
} usb_winusb_bosfeature_desc;

typedef struct {
    uint8_t pre_packet_send;
    uint8_t packet_sent;
    uint8_t packet_receive;

    uint8_t data[CDC_WINUSB_PACKET_SIZE];

    uint32_t receive_length;

    acm_line line_coding;
} usb_winusb_handler;

extern usb_desc winusb_desc;
extern usb_class winusb_class;

void winusb_data_receive(usb_dev *udev);

void winusb_data_send(usb_dev *udev);

uint8_t winusb_check_ready(usb_dev *udev);

#endif

#endif
