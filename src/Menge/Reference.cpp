#	include "Reference.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Reference::Reference()
		: m_refcounter(0)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int Reference::incrementReference()
	{
		if( ++m_refcounter == 1 )
		{
			if( _incrementZero() == false )
			{
				m_refcounter = 0;
			}
		}
		return m_refcounter;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int Reference::decrementReference()
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
	unsigned int Reference::countReference()
	{
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