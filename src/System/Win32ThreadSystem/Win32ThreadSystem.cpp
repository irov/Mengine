#	include "Win32ThreadSystem.h"

#	include "Logger/Logger.h"

#   include <algorithm>

SERVICE_FACTORY( ThreadSystem, Menge::ThreadSystemInterface, Menge::Win32ThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadSystem::Win32ThreadSystem()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32ThreadSystem::~Win32ThreadSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Win32ThreadSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadSystem::initialize()		
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::finalize()
	{
		for(TVectorPosixThreadIdentity::iterator
			it = m_threadIdentities.begin(),
			it_end = m_threadIdentities.end();
		it != it_end;
		++it )
		{
			Win32ThreadIdentity * identity = *it;

            HANDLE handle = identity->getHandle();
            CloseHandle( handle );

			m_poolWin32ThreadIdentity.destroyT( identity );
		}

		m_threadIdentities.clear();
	}
    //////////////////////////////////////////////////////////////////////////
    static DWORD WINAPI s_tread_job( LPVOID _userData )
    {
        ThreadListener * threadListener = static_cast<ThreadListener*>(_userData);

        threadListener->main();

        return 0;
    }
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentity * Win32ThreadSystem::createThread( ThreadListener * _thread, int _priority )
	{
        HANDLE handle = CreateThread( NULL, 0, &s_tread_job, (LPVOID)_thread, 0, NULL);

        if( handle == NULL )
        {
            DWORD error_code = GetLastError();

            LOGGER_ERROR(m_serviceProvider)("Win32ThreadSystem::createThread: invalid create thread error code - %d"
                , error_code
                );

            return nullptr;
        }

        switch( _priority )
        {   
        case -2:
            SetThreadPriority( handle, THREAD_PRIORITY_LOWEST );
        case -1:
            SetThreadPriority( handle, THREAD_PRIORITY_BELOW_NORMAL );
        case 0:
            SetThreadPriority( handle, THREAD_PRIORITY_NORMAL );
        case 1:
            SetThreadPriority( handle, THREAD_PRIORITY_ABOVE_NORMAL );
        case 2:
            SetThreadPriority( handle, THREAD_PRIORITY_HIGHEST );
        case 3:
            SetThreadPriority( handle, THREAD_PRIORITY_TIME_CRITICAL );
        }        
           
        Win32ThreadIdentity * identity = m_poolWin32ThreadIdentity.createT();
        identity->initialize( handle, _thread );

        m_threadIdentities.push_back( identity );
		
		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadSystem::joinThread( ThreadIdentity * _thread )
	{
		Win32ThreadIdentity * identity = static_cast<Win32ThreadIdentity*>(_thread);

		HANDLE treadHandle = identity->getHandle();

		WaitForSingleObject( treadHandle, INFINITE );

        CloseHandle( treadHandle );

        m_threadIdentities.erase( 
            std::remove( m_threadIdentities.begin(), m_threadIdentities.end(), identity )
            , m_threadIdentities.end() 
            );

        ThreadListener * listener = identity->getListener();
        listener->join();
    
        m_poolWin32ThreadIdentity.destroyT( identity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterface * Win32ThreadSystem::createMutex()
    {
        Win32ThreadMutex * mutex = m_poolWin32ThreadMutex.createObjectT();
        mutex->initialize( m_serviceProvider );

        return mutex;
    }
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::sleep( unsigned int _ms )
	{
		Sleep( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
}
