#include "global.h"
#include "ps.h"
#include "flc.h"
#include "crc.h"

#define PS_SIZE	8192


static const uint32_t * s_flash = (uint32_t*)0x1FE000;

bool ps_read( void * pv, uint16_t magic, uint16_t size )
{
    uint32_t i;
    uint32_t crc;
    uint32_t sig = (magic << 16) | size;
    for( i = PS_SIZE / 4 - 1; i > 0; i-- )
    {
        if( s_flash[i] == sig )
        {
            crc = s_flash[i + 1];
            CRC32_Init( false );
            CRC32_Reseed( 0xFFFFFFFF );
            CRC32_AddDataArray( (uint32_t*)&s_flash[i + 2], size / 4 );
            crc = CRC32_GetCRC();
            if( crc == s_flash[i + 1] )
            {
                memcpy( pv, &s_flash[i + 2], size );
                return true;
            }
        }
    }
    return false;
}

void ps_write( const void * pv, uint16_t magic, uint16_t size )
{
    uint32_t i, j, bc;
    CRC32_Init( false );
    uint32_t sig = (magic << 16) | size;
    CRC32_Reseed( 0xFFFFFFFF );
    CRC32_AddDataArray( (uint32_t*)pv, size / 4 );
    uint32_t crc = CRC32_GetCRC();
    uint16_t ts = size + sizeof(uint32_t);

    for( i = 0; i < PS_SIZE / 4; i++ )
    {
        bc = 0;
        for( j = 0; j < ts / 4; j++ )
        {
            if( s_flash[i + j] == ~0 )
                bc++;
        }
        if( bc == j )
        {
            FLC_Write( (uint32_t)&s_flash[i], &sig, sizeof(sig), MXC_V_FLC_FLSH_UNLOCK_KEY );
            FLC_Write( (uint32_t)&s_flash[i + 1], &crc, sizeof(crc), MXC_V_FLC_FLSH_UNLOCK_KEY );
            FLC_Write( (uint32_t)&s_flash[i + 2], pv, size, MXC_V_FLC_FLSH_UNLOCK_KEY );
            return;
        }
    }
    FLC_PageErase( (uint32_t)&s_flash[0], MXC_V_FLC_ERASE_CODE_PAGE_ERASE, MXC_V_FLC_FLSH_UNLOCK_KEY );
    ps_write( pv, magic, size );
    return;
}


