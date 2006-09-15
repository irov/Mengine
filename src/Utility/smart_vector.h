#	pragma once

namespace Utility
{
	template<class T> 
	class smart_vector
	{
	public:
		typedef T * iterator;
		typedef const T * const_iterator;
		typedef T value_type;

		typedef T& reference;
		typedef const T& const_reference;

	public: 
		smart_vector()
			: m_NumOfVectorObject(0)
			, m_CapacityVectorObject(0)
			, m_VectorObject(0)
		{

		}

		smart_vector( size_t Size)
			: m_NumOfVectorObject(0)
			, m_CapacityVectorObject(Size)
		{
			m_VectorObject =  new T[Size]();
		}

		~smart_vector()
		{
			delete []m_VectorObject;
		}

	public:
		inline T & operator[](size_t i)
		{
			return m_VectorObject[i];
		}

		inline const T & operator[](size_t i)const
		{
			return m_VectorObject[i];
		}

		inline void reserve(size_t Size)
		{
			m_NumOfVectorObject = 0;

			if( m_VectorObject != 0 )
			{
				delete [] m_VectorObject;
			}

			m_VectorObject =  new T[Size]();
			m_CapacityVectorObject = Size;
		}

		inline void resize(size_t Size)
		{
			if( Size > m_CapacityVectorObject)
			{
				delete [] m_VectorObject;
				m_VectorObject =  new T[Size]();
				m_CapacityVectorObject = Size;
			}
						
			m_NumOfVectorObject = Size;
		}

		inline void release()
		{
			delete [] m_VectorObject;
			m_VectorObject = 0;

			m_NumOfVectorObject = 0;
			m_CapacityVectorObject = 0;
		}

		inline T * data()
		{
			return m_VectorObject;
		}

		inline const T * data()const
		{
			return m_VectorObject;
		}

		inline size_t size()const
		{
			return m_NumOfVectorObject;
		}

		inline bool empty()const
		{
			return m_NumOfVectorObject == 0;
		}

		inline size_t capacity()const
		{
			return m_CapacityVectorObject;
		}

		inline reference back()
		{
			return m_VectorObject[m_NumOfVectorObject-1];
		}

		inline const_reference back()const
		{
			return m_VectorObject[m_NumOfVectorObject-1];
		}

		inline void push_back(const T &_T)
		{
			if( m_NumOfVectorObject < m_CapacityVectorObject )
			{
				m_VectorObject[m_NumOfVectorObject] = _T;
				++m_NumOfVectorObject;
			}
		}

		inline void pop_back()
		{
			--m_NumOfVectorObject;
		}

		inline T & push_back_reserve()
		{
			T & Object = m_VectorObject[m_NumOfVectorObject];

			if( m_NumOfVectorObject < m_CapacityVectorObject )
			{
				++m_NumOfVectorObject;
			}

			return Object;
		}

		inline void erase( size_t i)
		{
			if( m_NumOfVectorObject > 0 )
			{
				size_t j = --m_NumOfVectorObject;
				m_VectorObject[i] = m_VectorObject[j];
			}
			else
			{
				m_NumOfVectorObject = 0;
			}
		}

		inline void clear()
		{
			m_NumOfVectorObject = 0;
		}

		inline iterator begin()
		{
			return m_VectorObject;
		}

		inline const_iterator begin() const
		{
			return m_VectorObject;
		}

		inline iterator end()
		{
			return m_VectorObject + m_NumOfVectorObject;
		}

		inline const_iterator end() const
		{
			return m_VectorObject + m_NumOfVectorObject;
		}

		template<class T_Function>
			void remove_if( T_Function Function)
		{
			size_t i = 0;
			while(i < m_NumOfVectorObject)
			{
				if( Function(m_VectorObject[i]) == true )
				{
					size_t j = --m_NumOfVectorObject;
					m_VectorObject[i] = m_VectorObject[j];
				}
				else
				{
					++i;
				}
			}
		}

		template<class T_Function>
			void leave_if( T_Function Function )
		{
			size_t i = 0;
			while(i < m_NumOfVectorObject)
			{
				if( Function(m_VectorObject[i]) == false )
				{
					size_t j = --m_NumOfVectorObject;
					m_VectorObject[i] = m_VectorObject[j];
				}
				else
				{
					++i;
				}
			}
		}

	private:
		T *		m_VectorObject;
		size_t	m_NumOfVectorObject;
		size_t	m_CapacityVectorObject;
	};
}