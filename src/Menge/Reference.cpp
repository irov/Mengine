#	include "Reference.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Reference::Reference()
		: m_refcounter(0)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void Reference::incrementReference()
	{
		if( ++m_refcounter == 1 )
		{
			_incrementZero();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Reference::decrementReference()
	{
		if( m_refcounter == 0 )
		{			
			return 0;
		}

		if( --m_refcounter == 0 )
		{
			_decrementZero();
		}

		return m_refcounter;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Reference::countReference()
	{
		return m_refcounter;
	}
	//////////////////////////////////////////////////////////////////////////
	void Reference::_incrementZero()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Reference::_decrementZero()
	{
		//Empty
	}
}