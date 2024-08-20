#include "CompilableReference.h"

#include "Kernel/ThreadGuardScope.h"

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

        if( m_compileReferenceCount.incref() != 0 )
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

        if( m_compileReferenceCount.decref() != 0 )
        {
            return;
        }

        Compilable::release();
    }
    //////////////////////////////////////////////////////////////////////////
}
