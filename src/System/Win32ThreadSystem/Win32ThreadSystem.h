#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "WindowsLayer/WindowsIncluder.h"

#	include "Win32ThreadIdentity.h"
#	include "Win32ThreadMutex.h"

#   include "Factory/FactoryStore.h"

#   include "stdex/pool.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	class Win32ThreadSystem
		: public ServiceBase<ThreadSystemInterface>
	{
	public:
		Win32ThreadSystem();
		~Win32ThreadSystem();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool avaliable() const override;

	public:
		ThreadIdentityPtr createThread( int _priority, const char * _doc ) override;

	public:
        ThreadMutexInterfacePtr createMutex( const char * _doc ) override;

	public:
		void sleep( unsigned int _ms ) override;

	public:
		ptrdiff_t getCurrentThreadId() const override;

	protected:
		void onThreadIdentityRemove_( Win32ThreadIdentity * _identity );
	
	protected:
        typedef FactoryPoolStore<Win32ThreadIdentity, 16> TPoolWin32ThreadIdentity;
        TPoolWin32ThreadIdentity m_poolWin32ThreadIdentity;

        typedef FactoryPoolStore<Win32ThreadMutex, 16> TPoolWin32ThreadMutex;
        TPoolWin32ThreadMutex m_poolWin32ThreadMutex;				
	};
}
