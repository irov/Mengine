#pragma once

#include "Kernel/ConstStringHolder.h"
#include "Kernel/Hashgen.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ConstString
    {
    public:
        typedef ConstStringHolder::size_type size_type;
        typedef ConstStringHolder::hash_type hash_type;
        typedef ConstStringHolder::value_type value_type;

    public:
        MENGINE_CONSTEXPR ConstString() noexcept
            : m_holder( nullptr )
        {
        }

        MENGINE_CONSTEXPR ConstString( const ConstString & _cstr ) noexcept
            : m_holder( _cstr.m_holder )
        {
        }

        MENGINE_CONSTEXPR ConstString( ConstString && _cstr ) noexcept
            : m_holder( _cstr.m_holder )
        {
            _cstr.m_holder = nullptr;
        }

    public:
        explicit ConstString( const ConstStringHolder * _holder ) noexcept
            : m_holder( _holder )
        {
        }

    private:
        ConstString( std::nullptr_t ) = delete;

    public:
        static MENGINE_INLINE const ConstString & none() noexcept
        {
            static const ConstString s_none;

            return s_none;
        }

    public:
        MENGINE_CONSTEXPR size_type size() const
        {
            if( m_holder == nullptr )
            {
                return 0U;
            }

            const size_type holder_size = m_holder->size();

            return holder_size;
        }

        MENGINE_CONSTEXPR const value_type * c_str() const
        {
            if( m_holder == nullptr )
            {
                return "";
            }

            const value_type * holder_str = m_holder->c_str();

            return holder_str;
        }

        MENGINE_CONSTEXPR bool empty() const noexcept
        {
            return m_holder == nullptr;
        }

        MENGINE_CONSTEXPR hash_type hash() const
        {
            if( m_holder == nullptr )
            {
                return 0;
            }

            const hash_type holder_hash = m_holder->hash();

            return holder_hash;
        }

        MENGINE_CONSTEXPR void clear() noexcept
        {
            m_holder = nullptr;
        }

    public:
        MENGINE_CONSTEXPR ConstString & operator = ( const ConstString & _right ) noexcept
        {
            m_holder = _right.m_holder;

            return *this;
        }

        MENGINE_CONSTEXPR bool operator == ( const ConstString & _right ) const noexcept
        {
            return m_holder == _right.m_holder;
        }

        MENGINE_CONSTEXPR bool operator != ( const ConstString & _right ) const noexcept
        {
            return m_holder != _right.m_holder;
        }

        MENGINE_CONSTEXPR bool operator < ( const ConstString & _right ) const noexcept
        {
            return m_holder < _right.m_holder;
        }

        MENGINE_CONSTEXPR bool operator <= ( const ConstString & _right ) const noexcept
        {
            return m_holder <= _right.m_holder;
        }

        MENGINE_CONSTEXPR bool operator > ( const ConstString & _right ) const noexcept
        {
            return m_holder > _right.m_holder;
        }

        MENGINE_CONSTEXPR bool operator >= ( const ConstString & _right ) const noexcept
        {
            return m_holder >= _right.m_holder;
        }

    public:
        MENGINE_CONSTEXPR value_type operator [] ( size_type _index ) const
        {
            const value_type * holder_str = m_holder->c_str();

            value_type ch = holder_str[_index];

            return ch;
        }

    public:
        struct less_type
        {
            MENGINE_CONSTEXPR bool operator () ( const ConstString & _left, const ConstString & _right ) const noexcept
            {
                return _left < _right;
            }
        };

        MENGINE_CONSTEXPR const value_type * begin() const
        {
            const value_type * holder_str = m_holder->c_str();

            return holder_str;
        }

        MENGINE_CONSTEXPR const value_type * end() const
        {
            const size_type holder_size = m_holder->size();
            const value_type * holder_str = m_holder->c_str();

            return holder_str + holder_size;
        }

    private:
        bool operator == ( std::nullptr_t ) const = delete;

    protected:
        const ConstStringHolder * m_holder;
    };
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const ConstString & _left, const ConstString::value_type * _right );
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const ConstString::value_type * _left, const ConstString & _right );
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const ConstString & _left, const ConstString::value_type * _right );
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const ConstString::value_type * _left, const ConstString & _right );
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct Hashgen<ConstString>
    {
        MENGINE_CONSTEXPR HashType operator()( const ConstString & _value ) const
        {
            return _value.hash();
        }
    };
    //////////////////////////////////////////////////////////////////////////
}
