#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "WIN32/WindowsIncluder.h"

#include "Win32ThreadIdentity.h"
#include "Win32ThreadMutex.h"

#include "Factory/Factory.h"

#include "Core/ServiceBase.h"

#include "stdex/pool.h"

#include "stdex/stl_vector.h"

namespace Mengine
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
		ThreadIdentityInterfacePtr createThread( int _priority, const char * _file, uint32_t _line ) override;

	public:
        ThreadMutexInterfacePtr createMutex( const char * _file, uint32_t _line ) override;

	public:
		void sleep( uint32_t _ms ) override;

	public:
		ptrdiff_t getCurrentThreadId() const override;
        	
	protected:
        FactoryPtr m_factoryWin32ThreadIdentity;
        FactoryPtr m_factoryWin32ThreadMutex;
	};
}
