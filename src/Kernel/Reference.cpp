#include "Reference.h"

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

        if( m_refcounter == 0 )
        {
            return false;
        }

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
