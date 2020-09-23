#include "CompilableReference.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CompilableReference::CompilableReference()
        : m_compileReference( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CompilableReference::~CompilableReference()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CompilableReference::compile()
    {
        if( ++m_compileReference == 1 )
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
        if( --m_compileReference == 0 )
        {
            Compilable::release();
        }
    }    
}
