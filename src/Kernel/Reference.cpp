#include "Reference.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Reference::Reference()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Reference::~Reference()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Reference::incrementReference()
    {
        if( m_refcounter.incref() == false )
        {
            return true;
        }

        if( this->_incrementZero() == false )
        {
            m_refcounter.reset();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Reference::decrementReference()
    {
        if( m_refcounter.decref() == false )
        {
            return;
        }

        this->_decrementZero();
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
