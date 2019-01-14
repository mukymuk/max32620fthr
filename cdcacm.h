void cdcacm_init( void );
uint32_t cdcacm_write( const void *p_data, uint32_t length);
void cdcacm_write_all( const void *p_data, uint32_t length);
uint32_t cdcacm_read( void *p_data, uint32_t length );
void cdcacm_read_all( void *p_data, uint32_t length );
uint32_t cdcacm_read_term( void *p_data, uint32_t length, uint8_t *p_term, uint32_t term_count );

