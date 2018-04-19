#pragma once

#include "Core/ConstStringHolder.h"

namespace Mengine
{
    class ConstString
    {
    public:
        ConstString()
        {
        }

        ConstString( const ConstString & _cstr )
            : m_holder( _cstr.m_holder )
        {
        }

    public:
        typedef ConstStringHolder::size_type size_type;
        typedef ConstStringHolder::hash_type hash_type;
        typedef ConstStringHolder::value_type value_type;

    public:
        explicit ConstString( const ConstStringHolderPtr & _holder )
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

            size_type holder_size = m_holder->size();

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
            if( m_holder == nullptr )
            {
                return true;
            }

            bool holder_empty = m_holder->empty();

            return holder_empty;
        }

        inline hash_type hash() const
        {
            if( m_holder == nullptr )
            {
                return 0;
            }

            hash_type holder_hash = m_holder->hash();

            return holder_hash;
        }

        inline void clear()
        {
            *this = ConstString::none();;
        }

        inline void swap( ConstString & _cs )
        {
            m_holder.swap( _cs.m_holder );
        }

    public:
        inline ConstString & operator = ( const ConstString & _right )
        {
            m_holder = _right.m_holder;

            return *this;
        }

        inline bool operator == ( const ConstString & _right ) const
        {
            return m_holder == _right.m_holder;
        }

        inline bool operator != ( const ConstString & _right ) const
        {
            return !this->operator == ( _right );
        }

        inline bool operator < ( const ConstString & _right ) const
        {
            return m_holder < _right.m_holder;
        }

        inline bool operator <= ( const ConstString & _right ) const
        {
            return m_holder <= _right.m_holder;
        }

        inline bool operator > ( const ConstString & _right ) const
        {
            bool result = !(*this <= _right);

            return result;
        }

        inline bool operator >= ( const ConstString & _right ) const
        {
            bool result = !(*this < _right);

            return result;
        }

        struct less_type
        {
            bool operator () ( const ConstString & _left, const ConstString & _right ) const
            {
                return _left < _right;
            }
        };

    protected:
        ConstStringHolderPtr m_holder;
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
} 

namespace std
{
	inline void swap( Mengine::ConstString & _left, Mengine::ConstString & _right )
	{
		_left.swap( _right );
	}
}
