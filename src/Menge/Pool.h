#	pragma once

#	include <vector>

namespace Menge
{
	template<class T>
	class Pool
	{
		typedef std::vector<T*> TContainerPool;

	public:
		Pool( bool _placement = true )
			: m_placement(_placement)
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

			if( m_placement ) 
			{
				_element->~T();
				new (_element) T();
			}

			m_pool.push_back( _element );
		}

	protected:
		TContainerPool m_pool;
		bool m_placement;
	};
}