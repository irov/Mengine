#	pragma once

#	include "Factory/FactoryDefault.h"
#	include "Factory/FactoryPool.h"
#	include "Factory/FactorableUnique.h"

namespace Menge
{
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
		Factorable * createObject()
		{
			Factorable * factorable = m_ptr->createObject();

			return factorable;
		}

		void destroyObject( Factorable * _object )
		{
			m_ptr->destroyObject( _object );
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
	};

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

	template<class T, uint32_t Count>
	class FactoryPoolStore
	{
	public:
		typedef FactoryPool<T, Count> TFactoryType;

	public:
		FactoryPoolStore()
		{
			m_ptr = new FactorableUnique<TFactoryType>();
		}

		~FactoryPoolStore()
		{
			m_ptr = nullptr;
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
			Factorable * factorable = m_ptr->createObject();

			return factorable;
		}

		void destroyObject( Factorable * _object )
		{
			m_ptr->destroyObject( _object );
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
	};
}