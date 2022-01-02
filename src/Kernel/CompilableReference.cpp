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
        MENGINE_THREAD_GUARD_SCOPE( CompilableReference, this, "CompilableReference::compile" );

        if( m_compileReferenceCount.incref() == false )
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
        MENGINE_THREAD_GUARD_SCOPE( CompilableReference, this, "CompilableReference::release" );

        if( m_compileReferenceCount.decref() == false )
        {
            return;
        }

        Compilable::release();
    }
    //////////////////////////////////////////////////////////////////////////
}
