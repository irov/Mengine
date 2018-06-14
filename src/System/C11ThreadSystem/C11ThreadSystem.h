#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "C11ThreadIdentity.h"
#include "C11ThreadMutex.h"

#include "Factory/Factory.h"

#include "Core/ServiceBase.h"

namespace Mengine
{
	class C11ThreadSystem
		: public ServiceBase<ThreadSystemInterface>
	{
	public:
		C11ThreadSystem();
		~C11ThreadSystem() override;

	public:
		bool _initializeService() override;
		void _finalizeService() override;

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
        FactoryPtr m_factoryC11ThreadIdentity;
        FactoryPtr m_factoryC11ThreadMutex;
	};
}
