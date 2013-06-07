#	include "MarmaladeThreadIdentity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadIdentity::MarmaladeThreadIdentity( s3eThread * _thread )
		: m_thread(_thread)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	s3eThread * MarmaladeThreadIdentity::getThread() const
	{
		return m_thread;
	}
}