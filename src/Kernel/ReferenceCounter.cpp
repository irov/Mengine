#include "ReferenceCounter.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ReferenceCounter::ReferenceCounter()
        : m_counter( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ReferenceCounter::ReferenceCounter( uint32_t _counter )
        : m_counter( _counter )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ReferenceCounter::~ReferenceCounter()
    {
        MENGINE_ASSERTION_FATAL( m_counter == 0, "reference count is not zero '%u'"
            , m_counter.load()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void ReferenceCounter::setReferenceCount( uint32_t _counter )
    {
        m_counter = _counter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ReferenceCounter::incref()
    {
        if( ++m_counter == 1 )
        {
            MENGINE_ASSERTION_FATAL( m_counter == 1, "reference count is not one '%u'"
                , m_counter.load()
            );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ReferenceCounter::decref()
    {
        MENGINE_ASSERTION_FATAL( m_counter > 0, "reference count is already zero" );

        if( --m_counter != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ReferenceCounter::reset()
    {
        m_counter = 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
