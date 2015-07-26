#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "Factory/FactoryDefault.h"
#	include "Factory/FactoryPool.h"
#	include "Factory/FactorableUnique.h"

#	include "Core/ThreadGuard.h"

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

	template<class T>
	class FactoryDefaultStore
	{
	public:
		typedef FactoryDefault<T> TFactoryType;

	public:
		FactoryDefaultStore()
		{
			m_ptr = new FactorableUnique<TFactoryType>();
		}

	public:
		void setMutex( const ThreadMutexInterfacePtr & _mutex )
		{
			m_mutex = _mutex;
		}

	public:
		template<class C, class M>
		void setMethodListener( C * _self, M _method )
		{
			FactoryListenerInterfacePtr listener = new MethodFactoryListenerInterface<C, M, T>( _self, _method );

			this->setListener( listener );
		}

		void setListener( const FactoryListenerInterfacePtr & _listener )
		{
			m_ptr->setListener( _listener );
		}

	public:
		Factorable * createObject()
		{
			Factorable * factorable;

			if( m_mutex == nullptr )
			{
				THREAD_GUARD_CHECK( this, nullptr, "FactoryDefaultStore::createObject" );

				factorable = m_ptr->createObject();
			}
			else
			{
				m_mutex->lock();

				factorable = m_ptr->createObject();

				m_mutex->unlock();
			}

			return factorable;
		}

		void destroyObject( Factorable * _object )
		{
			Factorable * factorable;

			if( m_mutex == nullptr )
			{
				THREAD_GUARD_CHECK( this, nullptr, "FactoryDefaultStore::destroyObject" );

				m_ptr->destroyObject( _object );
			}
			else
			{
				m_mutex->lock();

				m_ptr->destroyObject( _object );

				m_mutex->unlock();
			}
		}

	public:
		T * createObjectT()
		{
			Factorable * obj = m_ptr->createObject();

			T * t = static_cast<T *>(obj);

			return t;
		}

	public:
		uint32_t countObject() const
		{
			uint32_t count = m_ptr->countObject();

			return count;
		}

	protected:
		FactoryPtr m_ptr;

		ThreadMutexInterfacePtr m_mutex;

		THREAD_GUARD_INIT
	};

	template<class T, uint32_t Count>
	class FactoryPoolStore
	{
	public:
		typedef FactoryPool<T, Count> TFactoryType;

	public:
		FactoryPoolStore()
			: m_mutex(nullptr)
		{
			m_ptr = new FactorableUnique<TFactoryType>();
		}

		~FactoryPoolStore()
		{
			m_ptr = nullptr;
			m_mutex = nullptr;
		}

	public:
		void setMutex( const ThreadMutexInterfacePtr & _mutex )
		{
			m_mutex = _mutex;
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
		Factorable * createObject()
		{
			Factorable * factorable;

			if( m_mutex == nullptr )
			{
				THREAD_GUARD_CHECK( this, nullptr, "FactoryPoolStore::createObject" );

				factorable = m_ptr->createObject();
			}
			else
			{
				m_mutex->lock();

				factorable = m_ptr->createObject();

				m_mutex->unlock();
			}			

			return factorable;
		}

		void destroyObject( Factorable * _object )
		{
			Factorable * factorable;

			if( m_mutex == nullptr )
			{
				THREAD_GUARD_CHECK( this, nullptr, "FactoryPoolStore::destroyObject" );

				m_ptr->destroyObject( _object );
			}
			else
			{ 
				m_mutex->lock();

				m_ptr->destroyObject( _object );

				m_mutex->unlock();
			}
		}

	public:
		T * createObjectT()
		{
			Factorable * obj = m_ptr->createObject();

			T * t = static_cast<T *>(obj);

			return t;
		}

	public:
		uint32_t countObject() const
		{
			uint32_t count = m_ptr->countObject();

			return count;
		}

	protected:
		FactoryPtr m_ptr;

		ThreadMutexInterfacePtr m_mutex;

		THREAD_GUARD_INIT
	};
}