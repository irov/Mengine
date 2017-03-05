#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "Factory/FactorablePtr.h"

#   include "Core/Pointer.h"

#	include "stdex/thread_guard.h"

#   include <stddef.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	class Factorable;
    //////////////////////////////////////////////////////////////////////////
    typedef PointerT<Factorable> PointerFactorable;
    //////////////////////////////////////////////////////////////////////////
	class FactoryDestroyListenerInterface
		: public FactorablePtr
	{
	public:
		virtual void onFactoryDestroyObject( Factorable * _object ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<FactoryDestroyListenerInterface> FactoryDestroyListenerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class Factory
		: public FactorablePtr
	{
	public:
		Factory();
		virtual ~Factory();

	public:
		void setDestroyListener( const FactoryDestroyListenerInterfacePtr & _listener );
		void setMutex( const ThreadMutexInterfacePtr & _mutex );
		
	public:
        PointerFactorable createObject();
		void destroyObject( Factorable * _object );

	public:
		uint32_t countObject() const;

	protected:
		virtual Factorable * _createObject() = 0;
		virtual void _destroyObject( Factorable * _object ) = 0;

	protected:
		FactoryDestroyListenerInterfacePtr m_destroyListener;
		ThreadMutexInterfacePtr m_mutex;

		uint32_t m_count;

		STDEX_THREAD_GUARD_INIT;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Factory> FactoryPtr;
    //////////////////////////////////////////////////////////////////////////
}
