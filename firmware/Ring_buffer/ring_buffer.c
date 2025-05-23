#include "ring_buffer.h"

void ring_bufferInit(ring_buffer* ring_buffer_t, uint32_t buffer_size)
{
    ring_buffer_t->data = (uint16_t*)malloc(buffer_size * sizeof(uint16_t));
    
    ring_buffer_t->buffer_size = buffer_size;
    ring_buffer_t->head     = 0;
    ring_buffer_t->tail     = 0;
    ring_buffer_t->counter  = 0;
}

void ring_bufferPush(ring_buffer *ring_buffer_t, uint16_t data)
{
    ring_buffer_t->data[ring_buffer_t->head] = data;
    
    ring_buffer_t->head = (ring_buffer_t->head + 1) % ring_buffer_t->buffer_size;
    ring_buffer_t->counter++;

    if(ring_buffer_t->counter > ring_buffer_t->buffer_size)
        ring_buffer_t->counter = 0; // data overflow
}

void ring_bufferPop(ring_buffer* ring_buffer_t, uint16_t *data)
{
    *data = ring_buffer_t->data[ring_buffer_t->tail];

    ring_buffer_t->tail = (ring_buffer_t->tail + 1) % ring_buffer_t->buffer_size;
    ring_buffer_t->counter--;

    if(ring_buffer_t->counter <= 0)
        ring_buffer_t->counter = 0;
}

uint16_t ring_bufferGetCapacity(ring_buffer* ring_buffer_t)
{
    return ring_buffer_t->counter;
}

void ring_bufferClear(ring_buffer* ring_buffer_t)
{
    ring_buffer_t->counter = 0;
    ring_buffer_t->head = 0;
    ring_buffer_t->tail = 0;
}

void ring_bufferPrint(ring_buffer* ring_buffer_t)
{
    for(uint16_t i = ring_buffer_t->tail; i < ring_buffer_t->head; i++)
    {
       printf("%d: %d\n", i, ring_buffer_t->data[i]);
       printf("---------------=end of data=----------------\n");
    }
    
}

void ring_bufferFree(ring_buffer* ring_buffer_t)
{
    free(ring_buffer_t->data);
    ring_buffer_t->data = NULL;
}