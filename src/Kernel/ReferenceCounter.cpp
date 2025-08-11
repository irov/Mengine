#include "ReferenceCounter.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ReferenceCounter::ReferenceCounter()
        : m_referenceCounter( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ReferenceCounter::ReferenceCounter( uint32_t _counter )
        : m_referenceCounter( _counter )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ReferenceCounter::~ReferenceCounter()
    {
        MENGINE_ASSERTION_FATAL( m_referenceCounter == 0, "reference count is not zero '%u'"
            , m_referenceCounter.load()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ReferenceCounter::increfReferenceCount()
    {
        uint32_t referenceCount = m_referenceCounter++;

        return referenceCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ReferenceCounter::decrefReferenceCount()
    {
        MENGINE_ASSERTION_FATAL( m_referenceCounter != 0, "decrefReferenceCount called on zero reference count" );

        uint32_t referenceCount = --m_referenceCounter;

        return referenceCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void ReferenceCounter::resetReferenceCount()
    {
        m_referenceCounter.store( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
}
