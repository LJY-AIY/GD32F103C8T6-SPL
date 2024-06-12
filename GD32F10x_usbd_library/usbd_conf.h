/*!
    \file    usbd_conf.h
    \brief   usb device driver basic configuration

    \version 2024-01-05, V2.3.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

/****************************************************************************************
 * 保留设备类框架的总配置文件 为了在一个项目中调试多种USB设备类，除了 BTABLE_OFFSET 和 上拉引脚 
 * 为全局共用，其他都为独立的配置。同一时间只能使用一个设备类
****************************************************************************************/
#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#include "gd32f10x.h"

/****************************************************************************************
 * 总配置开关
****************************************************************************************/
#define USE_USBD_CDCACM                   0U
#define USE_USBD_HIDKEY                   0U
#define USE_USBD_WINUSB                   1U
#define USBD_INT_DEBUG                    0U


/****************************************************************************************
 * CDC_ACM 虚拟串口设备独立配置
****************************************************************************************/
#if (USE_USBD_CDCACM)

#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   1U

#define CDC_COM_INTERFACE                  0U

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
//#define USB_DEVICE_LOW_PWR_MODE_SUPPORT

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* endpoint count used by the CDC ACM device */
#define CDC_CMD_EP                         EP_IN(2U)
#define CDC_IN_EP                          EP_IN(1U)
#define CDC_OUT_EP                         EP_OUT(3U)

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                        (0x40U)
#define EP0_TX_ADDR                        (0x80U)

/* CDC data Tx buffer address offset */
#define BULK_TX_ADDR                       (0x140U)

/* CDC data Rx buffer address offset */
#define BULK_RX_ADDR                       (0x100U)

/* CDC command Tx buffer address offset */
#define INT_TX_ADDR                        (0xC0U)

#define CDC_ACM_CMD_PACKET_SIZE            8U
#define CDC_ACM_DATA_PACKET_SIZE           64U

/* endpoint count used by the CDC ACM device */
#define EP_COUNT                           (4U)

#define USB_STRING_COUNT                   4U

#endif


/****************************************************************************************
 * HID KEYBOARD 输入键盘设备
****************************************************************************************/
#if (USE_USBD_HIDKEY)
/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
#define USBD_LOWPWR_MODE_ENABLE

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* link power mode support */
/* #define LPM_ENABLED */

#define USBD_CFG_MAX_NUM                   1
#define USBD_ITF_MAX_NUM                   1

#define USBD_HID_INTERFACE                 0

/* endpoint count used by the HID device */
#define EP_COUNT                           (2)

#define USB_STRING_COUNT                   4

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_TX_ADDR                        0x20
#define EP0_RX_ADDR                        0x40

#define INT_TX_ADDR                        0x50
#define INT_RX_ADDR                        0x60

#define HID_IN_EP                          EP_IN(1)

#define HID_IN_PACKET                      8

#endif

/****************************************************************************************
 * WIN USB 转用于WINDOWS下的专用设备 进行数据传输
****************************************************************************************/
#if (USE_USBD_WINUSB)

#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   1U

#define WINUSB_IN_EP                       EP_IN(2U)
#define WINUSB_OUT_EP                      EP_OUT(1U)

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                        (0x40U)
#define EP0_TX_ADDR                        (0x80U)

/*data Tx buffer address offset */
#define BULK_TX_ADDR                       (0x140U)

/*data Rx buffer address offset */
#define BULK_RX_ADDR                       (0x100U)

#define CDC_WINUSB_PACKET_SIZE             64U

/* endpoint count used by the WIN USB device */
#define EP_COUNT                           (3U)

#define USB_STRING_COUNT                    3U

#endif

/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                      (0x0000U)

#define USB_PULLUP                         GPIOA
#define USB_PULLUP_PIN                     GPIO_PIN_15
#define RCU_AHBPeriph_GPIO_PULLUP          RCU_GPIOA

#endif/* __USBD_CONF_H */
