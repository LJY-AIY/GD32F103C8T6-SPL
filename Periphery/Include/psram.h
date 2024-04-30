#ifndef _PSRAM_H
#define _PSRAM_H

#ifdef __cplusplus
extern "C" {
#endif

void PSRAM_Init(void);

void PSRAM_ReadID(void);

void PSRAM_Read_Data(uint32_t addr,uint8_t *Rxbuff,uint32_t len);

void PSRAM_ReadEx_Data(uint32_t addr,uint8_t *Rxbuff,uint32_t len);

void PSRAM_Write_Date(uint32_t addr,uint8_t *Wxbuff,uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
