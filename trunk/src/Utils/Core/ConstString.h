#	pragma once

#   include "Core/ConstStringHolder.h"

#	include "Core/IntrusivePtr.h"
#	include "Core/IntrusiveLinked.h"

#   include "Config/Typedef.h"

namespace Menge
{
	class ConstString
	{
	public:
		ConstString();
		ConstString( const ConstString & _cstr );

    public:
        typedef size_t size_type;

	public:
        explicit ConstString( ConstStringHolder * _holder );

    public:
        static const ConstString & none();

	public:
		inline size_t size() const
		{
			return m_holder->size();
		}

		inline const char * c_str() const
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
            *this = ConstString::none();
		}

	public:
		inline ConstString & operator = ( const ConstString & _right )
		{
			ConstStringHolder * right_holder = _right.m_holder.get();

			if( m_holder->equal( right_holder ) == false )
			{
				m_holder = _right.m_holder->owner();
			}

			return *this;
		}

		inline bool operator == ( const ConstString & _right ) const
		{
			ConstStringHolder * right_holder = _right.m_holder.get();
			bool result = m_holder->equal( right_holder );

			return result;
		}

		inline bool operator != ( const ConstString & _cstr ) const
		{
			return ! this->operator == ( _cstr );
		}

		inline bool operator < ( const ConstString & _cstr ) const
		{
			ConstStringHolder * right_holder = _cstr.m_holder.get();
			bool result = m_holder->less( right_holder );

			return result;
		}

        inline bool operator <= ( const ConstString & _cstr ) const
        {
            if( *this < _cstr )
            {
                return true;
            }

            ConstStringHolder * right_holder = _cstr.m_holder.get();
            bool result = m_holder->equal( right_holder );

            return result;
        }

        inline bool operator > ( const ConstString & _right ) const
        {
            bool result = ! ( *this <= _right );

            return result;
        }

        inline bool operator >= ( const ConstString & _right ) const
        {
            bool result = ! ( *this < _right );

            return result;
        }

	public:
		ConstStringHolderPtr m_holder;
	};

	bool operator == ( const ConstString & _left, const char * _right );
	bool operator == ( const char * _left, const ConstString & _right );
}