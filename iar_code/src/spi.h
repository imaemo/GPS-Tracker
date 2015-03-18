#ifndef	_SPI_H_
#define _SPI_H_

#include "type.h"

void spi_init();
void spi_send(uchar dat);
uchar spi_receive();



#endif