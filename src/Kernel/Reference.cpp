#	include "Kernel/Reference.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Reference::Reference()
		: m_refcounter(0)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Reference::incrementReference()
	{
		if( ++m_refcounter == 1 )
		{
			if( this->_incrementZero() == false )
			{
				m_refcounter = 0;
			}
		}

		return m_refcounter;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Reference::decrementReference()
	{
		if( m_refcounter == 0 )
		{			
			return 0;
		}

		if( --m_refcounter == 0 )
		{
			this->_decrementZero();
		}

		return m_refcounter;
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
}
