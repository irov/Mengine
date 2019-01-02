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
    float DelaySchedulePipe::onSchedulePipe( uint32_t _id, uint32_t _index )
    {
        (void)_id;
        (void)_index;

        return m_delay;
    }
}