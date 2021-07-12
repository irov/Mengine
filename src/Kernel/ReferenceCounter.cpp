#include "ReferenceCounter.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadGuardScope.h"

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
        MENGINE_ASSERTION_FATAL( m_counter == 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ReferenceCounter::incref()
    {
        MENGINE_THREAD_GUARD_SCOPE( CompilableReference, this, "ReferenceCounter::incref" );

        bool result = this->increfWithMutex();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ReferenceCounter::decref()
    {
        MENGINE_THREAD_GUARD_SCOPE( CompilableReference, this, "ReferenceCounter::decref" );

        bool result = this->decrefWithMutex();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ReferenceCounter::increfWithMutex()
    {
        if( ++m_counter == 1 )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ReferenceCounter::decrefWithMutex()
    {
        MENGINE_ASSERTION_FATAL( m_counter != 0, "refcount == 0" );

        if( --m_counter != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ReferenceCounter::reset()
    {
        MENGINE_THREAD_GUARD_SCOPE( CompilableReference, this, "ReferenceCounter::decref" );

        m_counter = 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
