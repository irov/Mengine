#	pragma once

#	include <vector>

namespace Menge
{
	template<class T, bool B> 
	class PoolPlacement
	{
	public:
		static void placement( T * _element )
		{
		}
	};

	template<class T> 
	class PoolPlacement<T,true>
	{
	public:
		static void placement( T * _element )
		{
			_element->~T();
			new (_element) T();
		}
	};

	template<class T, bool B>
	class Pool
	{
		typedef std::vector<T*> TContainerPool;

	public:
		Pool()
		{
		}

		~Pool()
		{
			for( typename TContainerPool::iterator 
				it = m_pool.begin(), 
				it_end = m_pool.end();
			it != it_end;
			++it )
			{
				delete (*it);
			}
		}

	public:
		T * get()
		{
			if( m_pool.empty() == false )
			{
				T * element = m_pool.back();
				m_pool.pop_back();
				return element;
			}

			return new T();
		}

		void release( T * _element )
		{
			if( _element == 0 )
			{
				return;
			}

			PoolPlacement<T,B>::placement( _element );

			m_pool.push_back( _element );
		}

	protected:
		TContainerPool m_pool;
	};
}
