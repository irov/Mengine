#	pragma once

#   include "Core/ConstStringHolder.h"
#   include "Core/ConstStringHolderNone.h"

#	include "Core/IntrusivePtr.h"
#	include "Core/IntrusiveLinked.h"

#   include "Config/Typedef.h"

namespace Menge
{
    template<class T>
	class ConstStringT
	{
	public:
		ConstStringT()
            : m_holder(ConstStringT::none().m_holder->owner())
        {
        }

		ConstStringT( const ConstStringT & _cstr )
            : m_holder(_cstr.m_holder->owner())
        {
        }

    public:
        typedef size_t size_type;
        typedef ConstStringHolderT<T> TStringHolder;

	public:
        explicit ConstStringT( TStringHolder * _holder )
            : m_holder(_holder)
        {

        }

    public:
        static const ConstStringT<T> & none();

	public:
		inline size_t size() const
		{
			return m_holder->size();
		}

		inline const T * c_str() const
		{
			return m_holder->c_str();
		}

		inline bool empty() const
		{
			return m_holder->empty();
		}

        inline size_t hash() const
        {
            return m_holder->hash();
        }

		inline void clear()
		{
            *this = ConstStringT::none();
		}

	public:
		inline ConstStringT & operator = ( const ConstStringT & _right )
		{
			TStringHolder * right_holder = _right.m_holder.get();

			if( m_holder->equal( right_holder ) == false )
			{
				m_holder = _right.m_holder->owner();
			}

			return *this;
		}

		inline bool operator == ( const ConstStringT & _right ) const
		{
			TStringHolder * right_holder = _right.m_holder.get();
			bool result = m_holder->equal( right_holder );

			return result;
		}

		inline bool operator != ( const ConstStringT & _cstr ) const
		{
			return ! this->operator == ( _cstr );
		}

		inline bool operator < ( const ConstStringT & _cstr ) const
		{
			TStringHolder * right_holder = _cstr.m_holder.get();
			bool result = m_holder->less( right_holder );

			return result;
		}

        inline bool operator <= ( const ConstStringT & _cstr ) const
        {
            if( *this < _cstr )
            {
                return true;
            }

            TStringHolder * right_holder = _cstr.m_holder.get();
            bool result = m_holder->equal( right_holder );

            return result;
        }

        inline bool operator > ( const ConstStringT & _right ) const
        {
            bool result = ! ( *this <= _right );

            return result;
        }

        inline bool operator >= ( const ConstStringT & _right ) const
        {
            bool result = ! ( *this < _right );

            return result;
        }

	public:
        typedef ConstStringHolderT<T> TConstStringHolder;
        typedef IntrusivePtr<TConstStringHolder> TConstStringHolderPtr;
		TConstStringHolderPtr m_holder;
	};
    //////////////////////////////////////////////////////////////////////////
    template<class T>
	bool operator == ( const ConstStringT<T> & _left, const T * _right )
    {
        const T * left_str = _left.c_str();

        if( Detail::cs_strcmp( left_str, _right ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
	bool operator == ( const T * _left, const ConstStringT<T> & _right )
    {
        const T * right_str = _right.c_str();

        if( Detail::cs_strcmp( _left, right_str ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef ConstStringT<char> ConstString;
    typedef ConstStringT<wchar_t> ConstWString;
}