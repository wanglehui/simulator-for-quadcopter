#ifndef _SPI_H
#define _SPI_H

#include <stm32f10x.h>

#define SPI_MASTER                  SPI1
#define SPI_MASTER_CLK              RCC_APB2Periph_SPI1
#define SPI_MASTER_GPIO             GPIOB
#define SPI_MASTER_GPIO_CLK         RCC_APB2Periph_GPIOB
#define SPI_MASTER_PIN_SCK          GPIO_Pin_3
#define SPI_MASTER_PIN_MISO         GPIO_Pin_4
#define SPI_MASTER_PIN_MOSI         GPIO_Pin_5
#define SPI_MASTER_PIN_CS           GPIO_Pin_15

#define SPI_ReadByte(SPIx)  SPI_WriteByte(SPIx, 0)

u8 SPI_WriteByte(SPI_TypeDef *SPIx, u8 data);

void spi_init(void);
/*

int spi_write(SPI_TypeDef *SPIx, const char *buf, int len);
int spi_read(SPI_TypeDef *SPIx, char *buf, int len);

int spi_master_xfer(SPI_TypeDef *SPIx, const char *tx, int tx_len, char *rx, int rx_len);
{
    if (tx != NULL)
}
*/
#endif /* _SPI_H */
