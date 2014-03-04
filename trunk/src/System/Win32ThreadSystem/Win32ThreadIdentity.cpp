#	include "Win32ThreadIdentity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadIdentity::Win32ThreadIdentity()
        : m_handle(NULL)
        , m_task(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadIdentity::initialize( HANDLE _handle, const ThreadTaskInterfacePtr & _thread )
    {
        m_handle = _handle;
        m_task = _thread;
    }
	//////////////////////////////////////////////////////////////////////////
	HANDLE Win32ThreadIdentity::getHandle() const
	{
		return m_handle;
	}
    //////////////////////////////////////////////////////////////////////////
    const ThreadTaskInterfacePtr & Win32ThreadIdentity::getTask() const
    {
        return m_task;
    }
}