#include "global.h"
#include "cirbuf.h"

// This module provides generic a circular buffer support

void cirbuf_init( cirbuf_t * p_cirbuf, void *pv_buffer, uint32_t size,
                  cirbuf_write_update_t p_write_update, cirbuf_read_
                  update_t p_read_update, cirbuf_lock_t p_lock )
{
    p_cirbuf->read_ndx = 0;
    p_cirbuf->write_ndx = 0;
    p_cirbuf->free = size;
    p_cirbuf->buffer = (uint8_t*)pv_buffer;
    p_cirbuf->write_update = p_write_update;
    p_cirbuf->read_update = p_read_update;
    p_cirbuf->lock = p_lock;
}


uint32_t cirbuf_write( cirbuf_t * p_cirbuf, const void *pv, uint32_t size )
{
    // copy up to 'size' bytes from 'pv' into the circular buffer
    // return number of bytes actually copied

    uint32_t ndx = 0;
    const uint8_t * p = (const uint8_t *)pv;
    if( p_cirbuf->lock )
        p_cirbuf->lock(p_cirbuf);
    while( size && p_cirbuf->free )
    {
        p_cirbuf->buffer[ p_cirbuf->write_ndx++ ] = p[ ndx++ ];
        if( p_cirbuf->write_ndx >= p_cirbuf->size )
            p_cirbuf->write_ndx = 0;
        p_cirbuf->free--;
        size--;
    }
    if( p_cirbuf->write_update )
        p_cirbuf->write_update(p_cirbuf);
    return ndx;
}

uint32_t cirbuf_write_aquire( cirbuf_t * p_cirbuf, void **ppv, uint32_t max_size )
{
    // reserve writable block of up to 'size' contiguous bytes from the circular buffer
    // '*ppv' receives a pointer to the block
    // return number of bytes actually reserved

    uint32_t size = max_size;
    if( p_cirbuf->lock )
        p_cirbuf->lock(p_cirbuf);
    uint32_t contig_size = p_cirbuf->size - p_cirbuf->write_ndx;
    if( size > p_cirbuf->free )
        size = p_cirbuf->free;
    if( size > contig_size )
        size = contig_size;
    *ppv = (void*)&p_cirbuf->buffer[ p_cirbuf->write_ndx ];
    return size;

}

void cirbuf_write_release( cirbuf_t * p_cirbuf, uint32_t size )
{
    // release 'size' bytes in circular buffer previously aquired via cirbuf_write_aquire()

    p_cirbuf->write_ndx += size;
    p_cirbuf->free -= size;
    if( p_cirbuf->write_ndx >= p_cirbuf->size )
        p_cirbuf->write_ndx = 0;
    if( p_cirbuf->write_update )
        p_cirbuf->write_update(p_cirbuf);
}

uint32_t cirbuf_read( cirbuf_t * p_cirbuf, const void *pv, uint32_t size )
{
    // copy up to 'size' bytes from the circular buffer into 'pv'
    // return number of bytes actually copied

    uint32_t ndx = 0;
    uint8_t * p = (uint8_t *)pv;
    if( p_cirbuf->lock )
        p_cirbuf->lock(p_cirbuf);
    while( size && (p_cirbuf->free < p_cirbuf->size) )
    {
        p[ndx++] = p_cirbuf->buffer[ p_cirbuf->read_ndx++ ];
        if( p_cirbuf->read_ndx >= p_cirbuf->size)
            p_cirbuf->read_ndx = 0;
        p_cirbuf->free++;
        size--;
    }
    if( p_cirbuf->read_update )
        p_cirbuf->read_update(p_cirbuf);
    return size;
}

uint32_t cirbuf_read_aquire( cirbuf_t * p_cirbuf, const void ** ppv, uint32_t max_size )
{
    // reserve readable block up to 'size' contiguous bytes  from the circular buffer
    // '*ppv' receives a pointer to the block
    // return number of bytes actually reserved

    uint32_t size = max_size;
    uint32_t used = p_cirbuf->size - p_cirbuf->free;
    uint32_t contig_size = p_cirbuf->size - p_cirbuf->read_ndx;
    if( size > used )
        size = used;
    if( size > contig_size )
        size = contig_size;
    *ppv = (void*)&p_cirbuf->buffer[ p_cirbuf->read_ndx ];
    return size;
}

void cirbuf_read_release( cirbuf_t * p_cirbuf, uint32_t size )
{
    // release 'size' bytes in circular buffer previously aquired via cirbuf_read_aquire()
    if( p_cirbuf->lock )
        p_cirbuf->lock(p_cirbuf);
    p_cirbuf->read_ndx += size;
    if( p_cirbuf->read_ndx >= p_cirbuf->size )
        p_cirbuf->read_ndx = 0;
    p_cirbuf->free += size;
    if( p_cirbuf->read_update )
        p_cirbuf->read_update(p_cirbuf);
}
