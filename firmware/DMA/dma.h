#ifndef __DMA_CONFIG_H__
#define __DMA_CONFIG_H__

#include <pico/stdio.h>
#include <stdio.h>

#include <hardware/dma.h>

#define DATA_SIZE       (16384)


#define DMA_DATA_0      (0)
#define DMA_DATA_1      (1)
#define DMA_TIME_0      (2)
#define DMA_TIME_1      (3)


extern uint16_t *sampleData;
extern uint16_t *timeStamp ;

int DMA_PIOconfig(const void *writeAddress, const void *readAddress, uint dreq, uint dma_1, uint dma_2, bool priority, bool doubleSize);
void DMA_setEnable(uint dmaChannel, bool enable);
void DMA_chain(uint dma_1, uint dma_2, bool chain);

bool DMA_clear();

uint dma_getCurrentIndex(uint dmaChannel);


#endif // __DMA_CONFIG_H__