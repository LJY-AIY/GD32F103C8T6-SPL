# GD32F103C8T6-SPL

由于硬件资源的限制选用了GD32,STM32通用可以参考。
基于标准库的USB固件开发，实现了CDC_ACM(虚拟串口),HID(键盘),WINUSB(通用串行总线设备)。
硬件外设USBD，仅作为USB设备工作。
三种设备互相独立，一次只能以一种设备工作。

# CDC_ACM
GD32F103C8T6-SPL\GD32F10x_usbd_library\class\device\cdc\Source\cdc_acm_core.c
![image](https://github.com/LJY-AIY/GD32F103C8T6-SPL/assets/161273666/efbb4c65-53bb-4aa3-aab0-3e1d11093bac)
![image](https://github.com/LJY-AIY/GD32F103C8T6-SPL/assets/161273666/8d16d479-7315-4502-8403-c56922de3340)

# HID
GD32F103C8T6-SPL\GD32F10x_usbd_library\class\device\hid\Source\standard_hid_core.c
![image](https://github.com/LJY-AIY/GD32F103C8T6-SPL/assets/161273666/e16ae8cd-2f1b-4ac9-b0f9-1f20fea387b1)
![image](https://github.com/LJY-AIY/GD32F103C8T6-SPL/assets/161273666/986769f3-0ea4-41ed-a8b4-1f0f91ca962a)

# WINUSB
GD32F103C8T6-SPL\GD32F10x_usbd_library\class\device\winusb\Source\standard_winusb_core.c
![image](https://github.com/LJY-AIY/GD32F103C8T6-SPL/assets/161273666/a78b3312-f5c0-43e6-a240-66cab334d4fe)
![image](https://github.com/LJY-AIY/GD32F103C8T6-SPL/assets/161273666/9bb7b0ec-9409-4378-b38e-1f26690517cd)




