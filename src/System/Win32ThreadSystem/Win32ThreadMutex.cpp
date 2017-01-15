#   include "Win32ThreadMutex.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadMutex::Win32ThreadMutex()
        : m_doc(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
	void Win32ThreadMutex::initialize( const char * _doc )
    {
        InitializeCriticalSection( &m_cs );

		m_doc = _doc;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::lock()
    {
        EnterCriticalSection( &m_cs );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::unlock()
    {
        LeaveCriticalSection( &m_cs );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadMutex::try_lock()
	{
		if( TryEnterCriticalSection( &m_cs ) == FALSE )
		{
			return false;
		}

		return true; 
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadMutex::_destroy()
    {
        DeleteCriticalSection( &m_cs );
    }
}