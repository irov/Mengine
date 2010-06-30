#	pragma once


namespace Menge
{
	template<class T> 
	class IntrusivePtr
	{
	public:
		typedef IntrusivePtr<T> this_type;
		typedef T element_type;

	public:
		IntrusivePtr()
			: m_ptr(0)
		{
		}

		IntrusivePtr( T * _ptr )
			: m_ptr(_ptr)
		{
			incref();
		}

		template<class U>
		IntrusivePtr( const IntrusivePtr<U> & _rhs )
			: m_ptr(_rhs.get())
		{
			incref();
		}

		IntrusivePtr(const IntrusivePtr & _rhs)
			: m_ptr(_rhs.m_ptr)
		{
			incref();
		}

		~IntrusivePtr()
		{
			decref();
		}

		template<class U> 
		IntrusivePtr & operator = ( const IntrusivePtr<U> & _rhs )
		{
			this_type(_rhs).swap(*this);
			return *this;
		}

		IntrusivePtr & operator = ( const IntrusivePtr & _rhs )
		{
			this_type(_rhs).swap(*this);
			return *this;
		}

		IntrusivePtr & operator = ( T * _rhs )
		{
			this_type(_rhs).swap(*this);
			return *this;
		}

	public:
		T * get() const
		{
			return m_ptr;
		}

		T & operator*() const
		{
			return *m_ptr;
		}

		T * operator->() const
		{
			return m_ptr;
		}

		void swap( IntrusivePtr & _rhs )
		{
			std::swap( m_ptr, _rhs.m_ptr );
		}

	private:
		void incref()
		{
			if( m_ptr != 0 ) 
			{
				intrusive_ptr_add_ref( m_ptr );
			}
		}

		void decref()
		{
			if( m_ptr != 0 )
			{
				intrusive_ptr_release( m_ptr );
			}
		}

	private:
		T * m_ptr;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class T> 
	inline bool operator == ( const IntrusivePtr<T> & _left, const IntrusivePtr<T> & _right )
	{
		return _left.get() == _right.get();
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T> 
	inline bool operator != ( const IntrusivePtr<T> & _left, const IntrusivePtr<T> & _right )
	{
		return _left.get() != _right.get();
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T, class U> 
	inline bool operator == ( const IntrusivePtr<T> & _left, const IntrusivePtr<U> & _right )
	{
		return _left.get() == _right.get();
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T, class U> 
	inline bool operator != ( const IntrusivePtr<T> & _left, const IntrusivePtr<U> & _right )
	{
		return _left.get() != _right.get();
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T, class U> 
	inline bool operator == ( const IntrusivePtr<T> & _left, const U * _right )
	{
		return _left.get() == _right;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T, class U> 
	inline bool operator != ( const IntrusivePtr<T> & _left, const U * _right )
	{
		return _left.get() == _right;
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T, class U> 
	inline bool operator == ( const T * _left, const IntrusivePtr<U> & _right )
	{
		return _left == _right.get();
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T, class U> 
	inline bool operator != ( const T * _left, const IntrusivePtr<U> & _right )
	{
		return _left != _right.get();
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T> 
	inline bool operator < ( const IntrusivePtr<T> & _left, const IntrusivePtr<T> & _right )
	{
		return std::less<T *>()( _left.get(), _right.get() );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T> void swap( const IntrusivePtr<T> & _left, const IntrusivePtr<T> & _right )
	{
		_left.swap(_right);
	}
}