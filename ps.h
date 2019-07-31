#ifndef __PS_H_INCL__
#define __PS_H_INCL__


bool ps_read( void * pv, uint16_t magic, uint16_t size );

void ps_write( const void * pv, uint16_t magic, uint16_t size );

#endif
