#pragma once

#include "Kernel/ConstStringHolder.h"
#include "Kernel/Hashgen.h"

#include "Config/StdDef.h"

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

    private:
        ConstString( std::nullptr_t ) = delete;

    public:
        static const ConstString & none();

    public:
        MENGINE_INLINE size_type size() const
        {
            if( m_holder == nullptr )
            {
                return 0U;
            }

            const size_type holder_size = m_holder->size();

            return holder_size;
        }

        MENGINE_INLINE const value_type * c_str() const
        {
            if( m_holder == nullptr )
            {
                return "";
            }

            const value_type * holder_str = m_holder->c_str();

            return holder_str;
        }

        MENGINE_INLINE bool empty() const
        {
            return m_holder == none().m_holder;
        }

        MENGINE_INLINE hash_type hash() const
        {
            if( m_holder == nullptr )
            {
                return 0;
            }

            const hash_type holder_hash = m_holder->hash();

            return holder_hash;
        }

        MENGINE_INLINE void clear()
        {
            *this = ConstString::none();
        }

    public:
        MENGINE_INLINE ConstString & operator = ( const ConstString & _right ) noexcept
        {
            m_holder = _right.m_holder;

            return *this;
        }

        MENGINE_INLINE bool operator == ( const ConstString & _right ) const noexcept
        {
            return m_holder == _right.m_holder;
        }

        MENGINE_INLINE bool operator != ( const ConstString & _right ) const noexcept
        {
            return m_holder != _right.m_holder;
        }

        MENGINE_INLINE bool operator < ( const ConstString & _right ) const noexcept
        {
            return m_holder < _right.m_holder;
        }

        MENGINE_INLINE bool operator <= ( const ConstString & _right ) const noexcept
        {
            return m_holder <= _right.m_holder;
        }

        MENGINE_INLINE bool operator > ( const ConstString & _right ) const noexcept
        {
            return m_holder > _right.m_holder;
        }

        MENGINE_INLINE bool operator >= ( const ConstString & _right ) const noexcept
        {
            return m_holder >= _right.m_holder;
        }

        struct less_type
        {
            bool operator () ( const ConstString & _left, const ConstString & _right ) const noexcept
            {
                return _left < _right;
            }
        };

        const value_type * begin() const;
        const value_type * end() const;

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
        HashType operator()( const ConstString & _value ) const
        {
            return _value.hash();
        }
    };
    //////////////////////////////////////////////////////////////////////////
}
