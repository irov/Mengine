#pragma once

#include "Config/StdUtility.h"
#include "Config/DynamicCast.h"

#if defined(MENGINE_DEBUG)
#include "stdex/mpl.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class PointerT
    {
    public:
        PointerT( std::nullptr_t )
            : m_pointer( nullptr )
        {
        }

        PointerT( T * _pointer )
            : m_pointer( _pointer )
        {
        }

        PointerT( const PointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

    public:
        PointerT & operator = ( const PointerT & _pointer )
        {
            this->m_pointer = _pointer.m_pointer;

            return *this;
        }

        bool operator == ( const T * _ptr ) const
        {
            return this->m_pointer == _ptr;
        }

        bool operator != ( const T * _ptr ) const
        {
            return this->m_pointer != _ptr;
        }

    public:
        operator T * () const
        {
            return m_pointer;
        }

        template<class U>
        operator U * () const
        {
#if defined(MENGINE_DEBUG)
            if( m_pointer == nullptr )
            {
                return nullptr;
            }

            if( stdex::mpl::is_dynamic_cast<U *>::test( m_pointer ) == false )
            {
                throw;
            }
#endif

            U * t = static_cast<U *>(m_pointer);

            return t;
        }

        template<class U, class D>
        operator IntrusivePtr<U, D>() &&
        {
#if defined(MENGINE_DEBUG)
            if( m_pointer == nullptr )
            {
                return nullptr;
            }

            if( stdex::mpl::is_dynamic_cast<U *>::test( m_pointer ) == false )
            {
                throw;
            }
#endif

            return IntrusivePtr<U, D>::from( m_pointer );
        }

    protected:
        T * m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class ConstPointerT
    {
    public:
        ConstPointerT( std::nullptr_t )
            : m_pointer( nullptr )
        {
        }

        ConstPointerT( const T * _pointer )
            : m_pointer( _pointer )
        {
        }

        ConstPointerT( const ConstPointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

    public:
        ConstPointerT & operator = ( const ConstPointerT & _pointer )
        {
            this->m_pointer = _pointer.m_pointer;

            return *this;
        }

    public:
        bool operator == ( const T * _ptr ) const
        {
            return this->m_pointer == _ptr;
        }

        bool operator != ( const T * _ptr ) const
        {
            return this->m_pointer != _ptr;
        }

    public:
        operator const T * () const
        {
            return m_pointer;
        }

        template<class U>
        operator const U * () const
        {
#if defined(MENGINE_DEBUG)
            if( m_pointer == nullptr )
            {
                return nullptr;
            }

            if( stdex::mpl::is_dynamic_cast<const U *>::test( m_pointer ) == false )
            {
                throw;
            }
#endif

            const U * t = static_cast<const U *>(m_pointer);

            return t;
        }

        template<class U, class D>
        operator IntrusivePtr<U, D>() &&
        {
#if defined(MENGINE_DEBUG)
            if( m_pointer == nullptr )
            {
                return nullptr;
            }

            if( stdex::mpl::is_dynamic_cast<const U *>::test( m_pointer ) == false )
            {
                throw;
            }
#endif

            return IntrusivePtr<U, D>::from( m_pointer );
        }

    protected:
        const T * m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class DynamicPointerT
    {
    public:
        DynamicPointerT( std::nullptr_t )
            : m_pointer( nullptr )
        {
        }

        DynamicPointerT( T * _pointer )
            : m_pointer( _pointer )
        {
        }

        DynamicPointerT( const DynamicPointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

    public:
        DynamicPointerT & operator = ( const DynamicPointerT & _pointer )
        {
            this->m_pointer = _pointer.m_pointer;

            return *this;
        }

    public:
        bool operator == ( const T * _ptr ) const
        {
            return this->m_pointer == _ptr;
        }

        bool operator != ( const T * _ptr ) const
        {
            return this->m_pointer != _ptr;
        }

    public:
        operator T * () const
        {
            return m_pointer;
        }

        template<class U>
        operator U * () const
        {
            U * t = Helper::dynamicCast<U *>( m_pointer );

            return t;
        }

        template<class U, class D>
        operator IntrusivePtr<U, D>() &&
        {
            return IntrusivePtr<U, D>::dynamic_from( m_pointer );
        }

    protected:
        T * m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class DynamicConstPointerT
    {
    public:
        DynamicConstPointerT( std::nullptr_t )
            : m_pointer( nullptr )
        {
        }

        DynamicConstPointerT( const T * _pointer )
            : m_pointer( _pointer )
        {
        }

        DynamicConstPointerT( const DynamicConstPointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

    public:
        DynamicConstPointerT & operator = ( const DynamicConstPointerT & _pointer )
        {
            this->m_pointer = _pointer.m_pointer;

            return *this;
        }

        bool operator == ( const T * _ptr ) const
        {
            return this->m_pointer == _ptr;
        }

        bool operator != ( const T * _ptr ) const
        {
            return this->m_pointer != _ptr;
        }

    public:
        operator const T * () const
        {
            return m_pointer;
        }

        template<class U>
        operator const U * () const
        {
            const U * t = Helper::dynamicCast<const U *>( m_pointer );

            return t;
        }

        template<class U, class D>
        operator IntrusivePtr<U, D>() &&
        {
            return IntrusivePtr<U, D>::dynamic_from( m_pointer );
        }

    protected:
        const T * m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class PointerT<IntrusivePtr<T>>
    {
    public:
        PointerT( std::nullptr_t )
        {
        }

        PointerT( T * _pointer )
            : m_pointer( _pointer )
        {
        }

        PointerT( const PointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

        PointerT( PointerT && _pointer ) noexcept
            : m_pointer( std::forward<PointerT>( _pointer.m_pointer ) )
        {
        }

        PointerT( const IntrusivePtr<T> & _pointer )
            : m_pointer( _pointer )
        {
        }

        template<class U>
        PointerT( const IntrusivePtr<U> & _pointer )
            : m_pointer( _pointer )
        {
        }

        template<class U>
        PointerT( IntrusivePtr<U> && _pointer )
            : m_pointer( std::forward<IntrusivePtr<U>>( _pointer ) )
        {
        }

        template<class U, class D>
        PointerT( const IntrusivePtr<U, D> & _pointer )
            : m_pointer( _pointer )
        {
        }

        template<class U, class D>
        PointerT( IntrusivePtr<U, D> && _pointer )
            : m_pointer( std::forward<IntrusivePtr<U, D>>( _pointer ) )
        {
        }

    public:
        operator IntrusivePtr<T>() &&
        {
            return std::move( m_pointer );
        }

        template<class U>
        operator IntrusivePtr<U>() &&
        {
#if defined(MENGINE_DEBUG)
            T * p = m_pointer.get();

            if( p == nullptr )
            {
                return IntrusivePtr<U>();
            }

            if( stdex::mpl::is_dynamic_cast<U *>::test( p ) == false )
            {
                throw;
            }
#endif

            return IntrusivePtr<U>( std::move( m_pointer ) );
        }

        template<class D>
        operator IntrusivePtr<T, D>() &&
        {
            return IntrusivePtr<T, D>( std::move( m_pointer ) );
        }

        template<class U, class D>
        operator IntrusivePtr<U, D>() &&
        {
#if defined(MENGINE_DEBUG)
            T * p = m_pointer.get();

            if( p == nullptr )
            {
                return IntrusivePtr<U, D>();
            }

            if( stdex::mpl::is_dynamic_cast<U *>::test( p ) == false )
            {
                throw;
            }
#endif

            return std::move( m_pointer.template moveT<U, D>() );
        }

    protected:
        IntrusivePtr<T> m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class DynamicPointerT<IntrusivePtr<T>>
    {
    public:
        DynamicPointerT( std::nullptr_t )
        {
        }

        DynamicPointerT( T * _pointer )
            : m_pointer( _pointer )
        {
        }

        DynamicPointerT( const DynamicPointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

        DynamicPointerT( DynamicPointerT && _pointer )
            : m_pointer( std::forward<DynamicPointerT>( _pointer.m_pointer ) )
        {
        }

        DynamicPointerT( const IntrusivePtr<T> & _pointer )
            : m_pointer( _pointer )
        {
        }

        template<class U>
        DynamicPointerT( const IntrusivePtr<U> & _pointer )
            : m_pointer( _pointer )
        {
        }

        template<class U>
        DynamicPointerT( IntrusivePtr<U> && _pointer )
            : m_pointer( std::forward<IntrusivePtr<U>>( _pointer ) )
        {
        }

        template<class U, class D>
        DynamicPointerT( const IntrusivePtr<U, D> & _pointer )
            : m_pointer( _pointer )
        {
        }

        template<class U, class D>
        DynamicPointerT( IntrusivePtr<U, D> && _pointer )
            : m_pointer( std::forward<IntrusivePtr<U, D>>( _pointer ) )
        {
        }

    public:
        operator IntrusivePtr<T>() &&
        {
            return std::move( m_pointer );
        }

        template<class U>
        operator IntrusivePtr<U>() &&
        {
            return IntrusivePtr<U>::dynamic_from( std::move( m_pointer ) );
        }

        template<class D>
        operator IntrusivePtr<T, D>() &&
        {
            return IntrusivePtr<T, D>( std::move( m_pointer ) );
        }

        template<class U, class D>
        operator IntrusivePtr<U, D>() &&
        {
#if defined(MENGINE_DEBUG)
            T * p = m_pointer.get();

            if( p == nullptr )
            {
                return IntrusivePtr<U, D>();
            }

            if( stdex::mpl::is_dynamic_cast<U *>::test( p ) == false )
            {
                throw;
            }
#endif

            return std::move( m_pointer.template moveT<U, D>() );
        }

    protected:
        IntrusivePtr<T> m_pointer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef ConstPointerT<void> ConstPointer;
    typedef PointerT<void> Pointer;
    typedef DynamicConstPointerT<void> DynamicConstPointer;
    typedef DynamicPointerT<void> DynamicPointer;
    //////////////////////////////////////////////////////////////////////////
}
