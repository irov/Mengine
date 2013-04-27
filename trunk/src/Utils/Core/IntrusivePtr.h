#	pragma once

namespace Menge
{
	template<class T> 
	class IntrusivePtr
	{
	public:
		typedef T element_type;

	public:
		IntrusivePtr()
			: m_ptr(nullptr)
		{
		}

		IntrusivePtr( T * _ptr )
			: m_ptr(_ptr)
		{
			this->incref();
		}

        template<class U>
        IntrusivePtr( U * _ptr )
            : m_ptr(static_cast<T *>(_ptr))
        {
            this->incref();
        }
        
		IntrusivePtr(const IntrusivePtr & _rhs)
			: m_ptr(_rhs.get())
		{
			this->incref();
		}

        template<class U>
        IntrusivePtr(const IntrusivePtr<U> & _rhs)
            : m_ptr(static_cast<T *>(_rhs.get()))
        {
            this->incref();
        }

		~IntrusivePtr()
		{
			this->decref();
		}

		IntrusivePtr & operator = ( const IntrusivePtr & _rhs )
		{
			IntrusivePtr swap_ptr(_rhs);
            swap_ptr.swap( *this );

			return *this;
		}

		IntrusivePtr & operator = ( T * _rhs )
		{
            IntrusivePtr swap_ptr(_rhs);
            swap_ptr.swap( *this );

			return *this;
		}

	public:
		T * get() const
		{
			return m_ptr;
		}

		T * operator -> () const
		{
			return m_ptr;
		}

		void swap( IntrusivePtr & _rhs )
		{
            T * tmp = m_ptr;
            m_ptr = _rhs.m_ptr;
            _rhs.m_ptr = tmp;
		}

	protected:
		void incref()
		{
			if( m_ptr != nullptr ) 
			{
				intrusive_ptr_add_ref( m_ptr );
			}
		}

		void decref()
		{
			if( m_ptr != nullptr )
			{
				intrusive_ptr_release( m_ptr );
			}
		}

    protected:
		T * m_ptr;
	};
    //////////////////////////////////////////////////////////////////////////
    template<class T> 
    inline void swap( const IntrusivePtr<T> & _left, const IntrusivePtr<T> & _right )
    {
        _left.swap( _right );
    }
    //////////////////////////////////////////////////////////////////////////
    template<class U, class T>
    inline U intrusive_static_cast( const IntrusivePtr<T> & _iptr )
    {
        typedef typename U::element_type U_type;

        T * t_ptr = _iptr.get();
        U_type * u_ptr = static_cast<U_type *>(t_ptr);

        return U(u_ptr);
    }
    //////////////////////////////////////////////////////////////////////////
    template<class U, class T>
    inline U intrusive_dynamic_cast( const IntrusivePtr<T> & _iptr )
    {
        typedef typename U::element_type U_type;

        T * t_ptr = _iptr.get();
        U_type * u_ptr = dynamic_cast<U_type *>(t_ptr);

        return U(u_ptr);
    }
    //////////////////////////////////////////////////////////////////////////
    template<class U, class T>
    inline U * intrusive_get( const IntrusivePtr<T> & _iptr )
    {
        T * t_ptr = _iptr.get();
        U * u_ptr = static_cast<U *>(t_ptr);

        return u_ptr;
    }
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
    template<class T>
    inline bool operator == ( const IntrusivePtr<T> & _left, nullptr_t )
    {
        T * ptr = _left.get();

        return ptr == nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T> 
    inline bool operator != ( const IntrusivePtr<T> & _left, nullptr_t )
    {
        T * ptr = _left.get();

        return ptr != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T> 
    inline bool operator == ( nullptr_t, const IntrusivePtr<T> & _right )
    {
        T * ptr = intrusive_get<T *>(_right);

        return ptr == nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T> 
    inline bool operator != ( nullptr_t, const IntrusivePtr<T> & _right )
    {
        T * ptr = intrusive_get<T *>(_right);

        return ptr != nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	template<class T> 
	inline bool operator < ( const IntrusivePtr<T> & _left, const IntrusivePtr<T> & _right )
	{
		return std::less<T *>()( _left.get(), _right.get() );
	}
}