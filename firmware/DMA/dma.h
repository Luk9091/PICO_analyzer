#ifndef __DMA_CONFIG_H__
#define __DMA_CONFIG_H__

#include <pico/stdio.h>
#include <stdio.h>

#include <hardware/dma.h>
#define DATA_SIZE       (256)


#define DMA_DATA_0      (0)
#define DMA_DATA_1      (1)
#define DMA_TIME        (2)
// #define DMA_TIME_0      (2)
// #define DMA_TIME_1      (3)



int DMA_PIOconfig(volatile void *writeAddress, const volatile void *readAddress, uint dreq, uint dma_1, uint dma_2);
int DMA_TIMERconfig(volatile void *writeAddress, const volatile void *readAddress, uint dreq, uint dma, bool enable);
void DMA_setEnable(uint dmaChannel, bool enable);
void DMA_chain(uint dma_1, uint dma_2, bool chain);

bool DMA_clear();

uint dma_getCurrentIndex(uint dmaChannel);

#endif // __DMA_CONFIG_H__