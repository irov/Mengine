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
		size_t countObject() const
		{
			size_t count = m_ptr->countObject();

			return count;
		}

	protected:
		FactoryPtr m_ptr;
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

		~FactoryPoolStore()
		{
			m_ptr = nullptr;
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
		size_t countObject() const
		{
			size_t count = m_ptr->countObject();

			return count;
		}

	protected:
		FactoryPtr m_ptr;
	};
}