#	include "Win32ThreadIdentity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadIdentity::Win32ThreadIdentity()
        : m_handle(NULL)
        , m_thread(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::initialize( HANDLE _handle, ThreadListener * _thread )
    {
        m_handle = _handle;
        m_thread = _thread;
    }
	//////////////////////////////////////////////////////////////////////////
	HANDLE Win32ThreadIdentity::getHandle() const
	{
		return m_handle;
	}
    //////////////////////////////////////////////////////////////////////////
    ThreadListener * Win32ThreadIdentity::getListener() const
    {
        return m_thread;
    }
}