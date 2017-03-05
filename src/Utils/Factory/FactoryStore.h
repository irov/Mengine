#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "Factory/FactoryDefault.h"
#	include "Factory/FactoryPool.h"
#	include "Factory/FactorableUnique.h"

#	include "stdex/thread_guard.h"

namespace Menge
{
	template<class C, class M, class T>
	class MethodFactoryListenerInterface
		: public FactoryListenerInterface
	{
	public:
		MethodFactoryListenerInterface( C * _self, M _method )
			: m_self(_self)
			, m_method(_method)
		{
		}

	protected:
		void onFactoryDestroyObject( Factorable * _object ) override
		{
			T * obj = static_cast<T *>(_object);

			(m_self->*m_method)( obj );
		}

	protected:
		void destroy() override
		{
			delete this;
		}

	protected:
		C * m_self;
		M m_method;
	};

	template<class T, size_t Count>
	class FactoryPoolStore
	{
	public:
		typedef FactoryPool<T, Count> TFactoryType;

	public:
		FactoryPoolStore()
		{
			m_ptr = new FactorableUnique<TFactoryType>();
		}

	public:
		void setMutex( const ThreadMutexInterfacePtr & _mutex )
		{
			m_ptr->setMutex( _mutex );
		}
		
	public:
		template<class C, class M>
		void setMethodListener( C * _self, M _method )
		{
			FactoryListenerInterfacePtr listener = new MethodFactoryListenerInterface<C, M, T>(_self, _method);

			this->setListener( listener );
		}

		void setListener( const FactoryListenerInterfacePtr & _listener )
		{
			m_ptr->setListener( _listener );
		}

	public:
		T * createObject()
		{
			Factorable * factorable = m_ptr->createObject();

#	ifdef _DEBUG
			if( dynamic_cast<T *>(factorable) == nullptr )
			{
                throw;
			}
#	endif

			T * t = static_cast<T *>(factorable);

			return t;
		}

		void destroyObject( Factorable * _object )
		{
			m_ptr->destroyObject( _object );
		}

	public:
		bool emptyObject() const
		{
			uint32_t count = m_ptr->countObject();

			return count == 0;
		}

		uint32_t countObject() const
		{
			uint32_t count = m_ptr->countObject();

			return count;
		}

	protected:
		FactoryPtr m_ptr;
	};
}