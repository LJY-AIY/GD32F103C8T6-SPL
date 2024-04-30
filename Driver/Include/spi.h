#ifndef _SPI_H
#define _SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_CSS_H   gpio_bit_set(SPI_CS_GPIO, SPI_CS_PIN)
#define SPI_CSS_L   gpio_bit_reset(SPI_CS_GPIO, SPI_CS_PIN)

void SPI_Init(void);

uint8_t SPI_transfer_Byte(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
