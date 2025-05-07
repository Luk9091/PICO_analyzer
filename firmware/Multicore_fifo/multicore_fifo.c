#include "multicore_fifo.h"


static bool tx_isEmpty = false; //Core 1 tx flag

bool fifo_trySetValidBuffer(uint8_t buffer_number, uint8_t valid_bufferSize)
{
    if(!multicore_fifo_rvalid()) //check if core_1 pushed any data
        return false;

    uint32_t fifo_readData = multicore_fifo_pop_blocking_inline();
    if(!(fifo_readData & (1<<0))) //check if Core 0 can push new data 
    { 
        tx_isEmpty = true;
        return false;
    }

    uint32_t fifo_writeData = 0;
    fifo_writeData |= ((buffer_number << 24) | (valid_bufferSize << 8));
    multicore_fifo_push_blocking_inline(fifo_writeData);
    return true;
}


bool fifo_trySetReadStatus(void)
{
    if(!multicore_fifo_rvalid())
        return false;

    if(tx_isEmpty)
    {
        multicore_fifo_push_blocking_inline(mc_readyCore1);
        tx_isEmpty = false;
        return true;
    }

    return false;
}

bool fifo_tryReadBufferData(uint8_t *buffer_number, uint8_t *buffer_size)
{
    if(!multicore_fifo_rvalid())
        return false;

    uint32_t read_buffer = multicore_fifo_pop_blocking_inline();

    *buffer_number = (read_buffer >> 24);
    *buffer_size = (read_buffer & 0x000000ff);

    return true;
}