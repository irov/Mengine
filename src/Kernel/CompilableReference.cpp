#include "CompilableReference.h"

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
        if( m_compileReferenceCount.decref() == false )
        {
            return;
        }

        Compilable::release();
    }
    //////////////////////////////////////////////////////////////////////////
}
