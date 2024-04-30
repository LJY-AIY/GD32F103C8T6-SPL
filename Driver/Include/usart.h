#ifndef _USART_H
#define _USART_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t Rxbuff[512];
    uint8_t Idleflag;
    uint8_t Tcfalg;
    uint16_t Pos;
} USARTINFO_T;

extern USARTINFO_T USART0_INFO;

void USART0_Init(uint32_t bps);

#ifdef __cplusplus
}
#endif

#endif
