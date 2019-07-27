#ifndef __PMU_H__
#define __PMU_H__

#include ".\csl\periphdriver\include\pmu.h"

struct _pmu_channel_t;
typedef void (*pmu_isr_t)( void *pv );

typedef struct _pmu_channel_t
{
    mxc_pmu_regs_t  *   regs;
    pmu_isr_t           isr;
    void            *   context;
}
pmu_channel_t;

void pmu_init( void );

pmu_channel_t * pmu_alloc( pmu_isr_t p_isr, void * pv_context );
void pmu_free( pmu_channel_t * p_pmu_channel );
void pmu_execute( pmu_channel_t * p_pmu_channel, const void * p_pmu_descriptor, uint16_t loop0, uint16_t loop1, uint8_t burst );

static inline void pmu_lock( pmu_channel_t * p_pmu_channel )
{
    p_pmu_channel->regs->cfg &= ~MXC_F_PMU_CFG_INT_EN; 
}

static inline void pmu_unlock( pmu_channel_t * p_pmu_channel )
{
    p_pmu_channel->regs->cfg |= MXC_F_PMU_CFG_INT_EN; 
}

#endif
