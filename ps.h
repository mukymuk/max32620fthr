#ifndef __PS_H_INCL__
#define __PS_H_INCL__

#define PS_FLASH_BANK_SIZE	8192

typedef struct
{
    uint32_t    address;    // starting address of flash area (32-bit aligned)
    uint32_t    size;       // size of flash area in 32-bit words
    uint32_t    version;
}
ps_t;

uint32_t ps_locate( const ps_t * ps, void ** p_data );
bool ps_write( const ps_t * ps, const void * pv, uint32_t size );


#endif
