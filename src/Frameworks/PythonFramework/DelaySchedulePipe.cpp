#include "DelaySchedulePipe.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DelaySchedulePipe::DelaySchedulePipe()
        : m_delay( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DelaySchedulePipe::~DelaySchedulePipe()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DelaySchedulePipe::initialize( float _delay )
    {
        m_delay = _delay;
    }
    //////////////////////////////////////////////////////////////////////////    
    float DelaySchedulePipe::onSchedulerPipe( uint32_t _id, uint32_t _index )
    {
        MENGINE_UNUSED( _id );
        MENGINE_UNUSED( _index );

        return m_delay;
    }
}