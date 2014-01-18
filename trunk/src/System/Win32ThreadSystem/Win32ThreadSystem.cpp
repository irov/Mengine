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
			const Win32ThreadIdentityPtr & identity = *it;

            HANDLE handle = identity->getHandle();
            CloseHandle( handle );
		}

		m_threadIdentities.clear();
	}
    //////////////////////////////////////////////////////////////////////////
    static DWORD WINAPI s_tread_job( LPVOID _userData )
    {
        ThreadTaskInterface * threadListener = static_cast<ThreadTaskInterface*>(_userData);

        threadListener->main();

        return 0;
    }
	//////////////////////////////////////////////////////////////////////////
	ThreadIdentityPtr Win32ThreadSystem::createThread( const ThreadTaskInterfacePtr & _thread, int _priority )
	{
		ThreadTaskInterface * thread_ptr = _thread.get();
        HANDLE handle = CreateThread( NULL, 0, &s_tread_job, (LPVOID)thread_ptr, 0, NULL);

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
			{
				SetThreadPriority( handle, THREAD_PRIORITY_LOWEST );
			}break;
		case -1:
			{
				SetThreadPriority( handle, THREAD_PRIORITY_BELOW_NORMAL );
			}break;
		case 0:
			{
				SetThreadPriority( handle, THREAD_PRIORITY_NORMAL );
			}break;
		case 1:
			{
				SetThreadPriority( handle, THREAD_PRIORITY_ABOVE_NORMAL );
			}break;
		case 2:
			{
				SetThreadPriority( handle, THREAD_PRIORITY_HIGHEST );
			}break;
		case 3:
			{
				SetThreadPriority( handle, THREAD_PRIORITY_TIME_CRITICAL );
			}break;
        }        
           
        Win32ThreadIdentityPtr identity = m_poolWin32ThreadIdentity.createObjectT();
        identity->initialize( handle, _thread );

        m_threadIdentities.push_back( identity );
		
		return identity;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32ThreadSystem::joinThread( const ThreadIdentityPtr & _thread )
	{
		Win32ThreadIdentityPtr identity = stdex::intrusive_static_cast<Win32ThreadIdentityPtr>(_thread);

		HANDLE treadHandle = identity->getHandle();

		WaitForSingleObject( treadHandle, INFINITE );

        CloseHandle( treadHandle );

        m_threadIdentities.erase( 
            std::remove( m_threadIdentities.begin(), m_threadIdentities.end(), identity )
            , m_threadIdentities.end() 
            );

        const ThreadTaskInterfacePtr & listener = identity->getListener();
        listener->join();
    
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr Win32ThreadSystem::createMutex()
    {
        Win32ThreadMutex * mutex = m_poolWin32ThreadMutex.createObjectT();
        mutex->initialize( m_serviceProvider );

        return mutex;
    }
	//////////////////////////////////////////////////////////////////////////
	void Win32ThreadSystem::sleep( unsigned int _ms )
	{
		::Sleep( _ms );
	}
	//////////////////////////////////////////////////////////////////////////
}
