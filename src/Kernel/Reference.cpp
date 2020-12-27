#include "Reference.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadGuardScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Reference::Reference()
        : m_refcounter( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Reference::~Reference()
    {
        MENGINE_ASSERTION_FATAL( m_refcounter == 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Reference::incrementReference()
    {
        MENGINE_THREAD_GUARD_SCOPE( Reference, this, "Reference::incrementReference" );

        if( ++m_refcounter == 1 )
        {
            if( this->_incrementZero() == false )
            {
                m_refcounter = 0;

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Reference::decrementReference()
    {
        MENGINE_THREAD_GUARD_SCOPE( Reference, this, "Reference::decrementReference" );

        MENGINE_ASSERTION_FATAL( m_refcounter != 0, "decrement reference refcount == 0" );

        if( --m_refcounter == 0 )
        {
            this->_decrementZero();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Reference::_incrementZero()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Reference::_decrementZero()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
