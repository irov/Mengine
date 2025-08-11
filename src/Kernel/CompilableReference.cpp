#include "CompilableReference.h"

#include "Kernel/ThreadGuardScope.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CompilableReference::CompilableReference()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CompilableReference::~CompilableReference()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CompilableReference::compile()
    {
        MENGINE_THREAD_GUARD_SCOPE( CompilableReference, this );

        uint32_t referenceCount = m_compileReferenceCount.increfReferenceCount();

        if( referenceCount != 0 )
        {
            return true;
        }

        if( Compilable::compile() == false )
        {
            return false;
        }
    
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CompilableReference::release()
    {
        MENGINE_THREAD_GUARD_SCOPE( CompilableReference, this );

        MENGINE_ASSERTION_FATAL( m_compileReferenceCount.getReferenceCount() != 0, "invalid release compilable zero reference" );

        uint32_t referenceCount = m_compileReferenceCount.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        Compilable::release();
    }
    //////////////////////////////////////////////////////////////////////////
}
