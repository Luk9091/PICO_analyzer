#ifndef _RING_BUFFER_
#define _RING_BUFFER_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct{
    uint16_t *data;      
    uint32_t counter;   
    uint32_t head;      
    uint32_t tail;      
    uint32_t buffer_size;
}ring_buffer;

/// @brief buffer initialize
/// @param ring_Buffer_t  buffer pointer to data
/// @param buffer_size buffer size
void ring_bufferInit(ring_buffer* ring_Buffer_t, uint32_t buffer_size);


/// @brief ring buffer push data
/// @param ring_Buffer_t 
/// @param data  
void ring_bufferPush(ring_buffer* ring_Buffer_t, uint16_t data);

/// @brief ring buffer pop data
/// @param ring_Buffer_t 
/// @param data
void ring_bufferPop(ring_buffer* ring_Buffer_t, uint16_t *data);

/// @brief ring buffer get capacity
/// @param ring_Buffer_t 
/// @return ring_Buffer_t data pointer
uint16_t ring_bufferGetCapacity(ring_buffer* ring_Buffer_t);

/// @brief clear ring buffer
/// @param ring_Buffer_t 
void ring_bufferClear(ring_buffer* ring_Buffer_t);

/// @brief print data contained in ring buffer
/// @param ring_Buffer_t 
void ring_bufferPrint(ring_buffer* ring_Buffer_t);

/// @brief free ring buffer allocated memory
/// @param - -- 
void ring_bufferFree(ring_buffer* ring_buffer_t);

#endif 