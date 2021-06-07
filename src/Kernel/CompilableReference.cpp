#include "CompilableReference.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadGuardScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CompilableReference::CompilableReference()
        : m_compileReferenceCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CompilableReference::~CompilableReference()
    {
        MENGINE_ASSERTION_FATAL( m_compileReferenceCount == 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool CompilableReference::compile()
    {
        MENGINE_THREAD_GUARD_SCOPE( CompilableReference, this, "CompilableReference::compile" );

        if( ++m_compileReferenceCount == 1 )
        {
            if( Compilable::compile() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CompilableReference::release()
    {
        MENGINE_THREAD_GUARD_SCOPE( CompilableReference, this, "CompilableReference::release" );

        MENGINE_ASSERTION_FATAL( m_compileReferenceCount != 0, "release compilable refcount == 0" );

        if( --m_compileReferenceCount == 0 )
        {
            Compilable::release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
