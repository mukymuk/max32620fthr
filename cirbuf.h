#ifndef __CIRBUF_H__
#define __CIRBUF_H__

struct _cirbuf_t;

typedef void (*cirbuf_write_update_t)(struct _cirbuf_t *);
typedef void (*cirbuf_read_update_t)(struct _cirbuf_t *);
typedef void (*cirbuf_lock_t)(struct _cirbuf_t *);

typedef struct
{
    uint8_t *   data;
    int32_t     size;
}
sized_buffer_t;

typedef struct _cirbuf_t
{
    int32_t                 free;
    int32_t                 write_ndx;
    int32_t                 read_ndx;
    cirbuf_write_update_t   write_update;
    cirbuf_read_update_t    read_update;
    cirbuf_lock_t           lock;
    uint8_t *               buffer;
    int32_t                 size;
}
cirbuf_t;

void cirbuf_init( cirbuf_t * p_cirbuf, void *pv_buffer, uint32_t size, cirbuf_write_update_t p_write_update, cirbuf_read_update_t p_read_update, cirbuf_lock_t p_lock );

uint32_t cirbuf_read( cirbuf_t * p_cirbuf, const void *pv, uint32_t size );
uint32_t cirbuf_read_aquire( cirbuf_t * p_curbuf, const void **ppv, uint32_t max_size );
void cirbuf_read_release( cirbuf_t * p_cirbuf, uint32_t size );

uint32_t cirbuf_write( cirbuf_t * p_cirbuf, const void *pv, uint32_t size );
uint32_t cirbuf_write_aquire( cirbuf_t * p_cirbuf, void **ppv, uint32_t max_size );
void cirbuf_write_release( cirbuf_t * p_cirbuf, uint32_t size );

#endif
