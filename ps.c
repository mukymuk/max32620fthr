#include "global.h"
#include "ps.h"
#include "flc.h"
#include "crc.h"

#define PS_FLASH_BANK SIZE  8192
#define CRC_SEED    0xFFFFFFFF

#define HDR_SIZE    4   // size of the header written to persistant storage in 32-bit words

// persistant data format:
// 
// 0 - version
// 1 - size in 32-bit words
// 2 - crc32 of data[]
// 3 - data[]

uint32_t ps_locate( const ps_t * ps, void ** p_data )
{
    uint32_t * flash = (uint32_t*)ps->address;
    int32_t i;
    uint32_t crc;
    uint32_t bank_size = ps->size;
    uint32_t size;

    for( i = bank_size-HDR_SIZE-1; i > 0; i-- )
    {
        if( flash[i] == ps->version )
        {
            size = flash[i+1];  // size in 32-bit words
            if( size <= (ps->size - i) )
            {
                crc = flash[i + 2];
                CRC32_Init( false );
                CRC32_Reseed( CRC_SEED );
                CRC32_AddDataArray( (uint32_t*)&flash[i+3], size );
                if( crc == CRC32_GetCRC() )
                {
                    *p_data = &flash[i+3];
                    return size<<2; // size in bytes
                }
            }
        }
    }
    return 0;
}

bool ps_write( const ps_t * ps, const void * pv, uint32_t size )
{
    uint32_t i, j, bc;
    while( size & 0x03 );
    uint32_t * flash = (uint32_t*)ps->address;
    CRC32_Init( false );
    CRC32_Reseed( CRC_SEED );
    size >>= 2; // size in 32-bit words
    CRC32_AddDataArray( (uint32_t*)pv, size );
    uint32_t crc = CRC32_GetCRC();
    uint16_t ts = size + sizeof(uint32_t);

    for( i = 0; i < ps->size - HDR_SIZE - size ; i++ )
    {
        bc = 0;
        for( j = 0; j < HDR_SIZE + size; j++ )
        {
            if( flash[i + j] == ~0 )
                bc++;
        }
        if( bc == j )
        {
            FLC_Write( (uint32_t)&flash[i], &ps->version, sizeof(ps->version), MXC_V_FLC_FLSH_UNLOCK_KEY );
            FLC_Write( (uint32_t)&flash[i+1], &size, sizeof(size), MXC_V_FLC_FLSH_UNLOCK_KEY );
            FLC_Write( (uint32_t)&flash[i+2], &crc, sizeof(crc), MXC_V_FLC_FLSH_UNLOCK_KEY );
            FLC_Write( (uint32_t)&flash[i+3], pv, size<<2, MXC_V_FLC_FLSH_UNLOCK_KEY );
            return true;
        }
    }
    uint32_t flash_size = ps->size;
    while( flash_size )
    {
        FLC_PageErase( (uint32_t)&flash[0], MXC_V_FLC_ERASE_CODE_PAGE_ERASE, MXC_V_FLC_FLSH_UNLOCK_KEY );
        flash_size -= PS_FLASH_BANK_SIZE;
    }
    return ps_write( ps, pv, size>>2 );
}


