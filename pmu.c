#include "global.h"
#include "mxc_device.h"
#include ".\pmu.h"

static pmu_channel_t s_p_pmu_channel[MXC_CFG_PMU_CHANNELS];

void pmu_init( void )
{
    NVIC_EnableIRQ(PMU_IRQn);
}

pmu_channel_t * pmu_alloc( pmu_isr_t p_isr, void *pv_context )
{
    uint32_t i;
    pmu_channel_t * p;
    for( i = 0; i < MXC_CFG_PMU_CHANNELS; i++ )
    {
        p = (pmu_channel_t*)&s_p_pmu_channel[i];
        if( !p->isr )
        {
            p->isr = p_isr;
            p->regs = MXC_PMU_GET_PMU(i);
            p->context = pv_context;
            return p;
        }
    }
    return NULL;
}

void pmu_free( pmu_channel_t * p_pmu_channel )
{
    p_pmu_channel->isr = NULL;
}

void pmu_execute( pmu_channel_t * p_pmu_channel, const void * p_pmu_descriptor, uint16_t loop0, uint16_t loop1, uint8_t burst )

{
    mxc_pmu_regs_t * p_regs = p_pmu_channel->regs;
    p_regs->dscadr = (uint32_t)p_pmu_descriptor;
    p_regs->loop = (loop1 << 16) | loop0;
    p_regs->cfg = MXC_F_PMU_CFG_ENABLE | MXC_F_PMU_CFG_INT_EN | (burst << MXC_F_PMU_CFG_BURST_SIZE_POS);

}

void PMU_IRQHandler( void )
{
    uint32_t i;
    for(i=0;i<MXC_CFG_PMU_CHANNELS;i++)
    {
        pmu_channel_t * p = (pmu_channel_t*)&s_p_pmu_channel[i];

        if( p->isr && (p->regs->cfg & MXC_F_PMU_CFG_INTERRUPT) )
        {
            p->isr( s_p_pmu_channel[i].context );
            p->regs->cfg |= MXC_F_PMU_CFG_INTERRUPT;
        }
    }
}

