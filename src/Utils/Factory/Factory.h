#	pragma once

#   include "Factory/FactorablePtr.h"

#	include "Core/ThreadGuard.h"

#   include <stddef.h>

namespace Menge
{
	class Factorable;

	class FactoryListenerInterface
	{
	public:
		virtual void onFactoryDestroyObject( Factorable * _object ) = 0;
	};

	class Factory
		: public FactorablePtr
	{
	public:
		Factory();
		virtual ~Factory();

	public:
		void setListener( FactoryListenerInterface * _listener );

	public:
		Factorable * createObject();
		void destroyObject( Factorable * _object );

	public:
		size_t countObject() const;

	protected:
		virtual Factorable * _createObject() = 0;
		virtual void _destroyObject( Factorable * _object ) = 0;

	protected:
		FactoryListenerInterface * m_listener;

		size_t m_count;

		THREAD_GUARD_INIT
	};

	typedef stdex::intrusive_ptr<Factory> FactoryPtr;
}
