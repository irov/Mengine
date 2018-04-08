#include "Kernel/Reference.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Reference::Reference()
		: m_refcounter(0)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	bool Reference::incrementReference()
	{
#ifndef NDEBUG
        this->_debugIncrementReference();
#endif

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
#ifndef NDEBUG
        this->_debugDecrementReference();
#endif

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
    void Reference::_debugIncrementReference()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Reference::_debugDecrementReference()
    {
        //Empty
    }
}
