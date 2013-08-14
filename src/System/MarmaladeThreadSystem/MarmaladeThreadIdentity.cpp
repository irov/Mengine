#	include "MarmaladeThreadIdentity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeThreadIdentity::MarmaladeThreadIdentity()
		: m_thread(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeThreadIdentity::initialize( s3eThread * _thread )
    {
        m_thread = _thread;
    }
	//////////////////////////////////////////////////////////////////////////
	s3eThread * MarmaladeThreadIdentity::getThread() const
	{
		return m_thread;
	}
}