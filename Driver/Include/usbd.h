#ifndef _USBD_H
#define _USBD_H

#ifdef __cplusplus
extern "C" {
#endif

void USBD_Init(void);

void USBD_Kernel(void);

#if (USE_USBD_CDCACM)

#endif

#if (USE_USBD_HIDKEY)


#endif

#ifdef __cplusplus
}
#endif

#endif
