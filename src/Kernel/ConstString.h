#pragma once

#include "Kernel/ConstStringHolder.h"
#include "Kernel/Hashgen.h"

namespace Mengine
{
    class ConstString
    {
    public:
        typedef ConstStringHolder::size_type size_type;
        typedef ConstStringHolder::hash_type hash_type;
        typedef ConstStringHolder::value_type value_type;

    public:
        ConstString() noexcept
            : m_holder( nullptr )
        {
        }

        ConstString( const ConstString & _cstr ) noexcept
            : m_holder( _cstr.m_holder )
        {
        }

    public:
        explicit ConstString( const ConstStringHolder * _holder ) noexcept
            : m_holder( _holder )
        {
        }

    public:
        static const ConstString & none();

    public:
        inline size_type size() const
        {
            if( m_holder == nullptr )
            {
                return 0U;
            }

            const size_type holder_size = m_holder->size();

            return holder_size;
        }

        inline const value_type * c_str() const
        {
            if( m_holder == nullptr )
            {
                return "";
            }

            const value_type * holder_str = m_holder->c_str();

            return holder_str;
        }

        inline bool empty() const
        {
            return m_holder == none().m_holder;
        }

        inline hash_type hash() const
        {
            if( m_holder == nullptr )
            {
                return 0;
            }

            const hash_type holder_hash = m_holder->hash();

            return holder_hash;
        }

        inline void clear()
        {
            *this = ConstString::none();
        }

    public:
        inline ConstString & operator = ( const ConstString & _right ) noexcept
        {
            m_holder = _right.m_holder;

            return *this;
        }

        inline bool operator == ( const ConstString & _right ) const noexcept
        {
            return m_holder == _right.m_holder;
        }

        inline bool operator != ( const ConstString & _right ) const noexcept
        {
            return !this->operator == ( _right );
        }

        inline bool operator < ( const ConstString & _right ) const noexcept
        {
            return m_holder < _right.m_holder;
        }

        inline bool operator <= ( const ConstString & _right ) const noexcept
        {
            return m_holder <= _right.m_holder;
        }

        inline bool operator > ( const ConstString & _right ) const noexcept
        {
            const bool result = !(*this <= _right);

            return result;
        }

        inline bool operator >= ( const ConstString & _right ) const noexcept
        {
            const bool result = !(*this < _right);

            return result;
        }

        struct less_type
        {
            bool operator () ( const ConstString & _left, const ConstString & _right ) const noexcept
            {
                return _left < _right;
            }
        };

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
        HashType operator()( const ConstString & _value ) const
        {
            return _value.hash();
        }
    };
}
