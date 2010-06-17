#	pragma once

#	include "Config/Typedef.h"
#	include "Core/Holder.h"

#	include <map>
#	include <vector>
#	include <string>

namespace Menge
{
	class ConstManager;
	class ConstStringHolder;
	class ConstString;

	inline static void conststring_incref( ConstStringHolder * _holder );
	inline static void conststring_decref( ConstStringHolder * _holder );
	inline static const String & conststring_get( ConstStringHolder * _holder );
	inline static void conststring_gen( ConstString & _out, ConstStringHolder * _holder, const String & _str );

	class ConstString
	{
	public:
		ConstString()
			: m_holder(0)
		{
		}

		~ConstString()
		{
			conststring_decref( m_holder );
		}

		ConstString( const ConstString & _string )
			: m_holder(_string.m_holder)
		{
			conststring_incref( m_holder );
		}



	protected:
		ConstString( ConstStringHolder * _holder )
			: m_holder(_holder)
		{
			conststring_incref( m_holder );
		}

	public:
		ConstString & operator = ( const ConstString & _string )
		{
			m_holder = _string.m_holder;

			return *this;
		}

	public:
		const String & str() const
		{
			return conststring_get( m_holder );
		}

		const char * c_str() const
		{
			return str().c_str();
		}

		std::size_t size() const
		{
			return str().size();
		}

		ConstString get( const String & _str ) const
		{
			ConstString value;
			conststring_gen( value, m_holder, _str );
			return value;
		}

	public:
		bool invalid() const
		{
			return m_holder == 0;
		}

		bool empty() const
		{
			return str().empty();
		}

	public:
		bool operator == ( const ConstString & _string ) const
		{
			return m_holder == _string.m_holder;
		}

		bool operator < ( const ConstString & _string ) const
		{
			return m_holder < _string.m_holder;
		}

	protected:
		ConstStringHolder * m_holder;

		friend class ConstManager;
	};

	inline ConstString operator + ( const ConstString & _left, const ConstString & _right )
	{
		const String & left = _left.str();
		const String & right = _right.str();

		ConstString result = _left.get( left + right );

		return result;
	}
	
	class ConstManager
		: public Holder<ConstManager>
	{
	public:
		typedef std::list<ConstStringHolder> TListConstString;
		typedef std::map<std::string, TListConstString::iterator> TMapConstString;

	public:
		ConstString genString( const String & _value );
		void removeString( TMapConstString::iterator _iterator );

	protected:
		TMapConstString m_cacheConstString;
		TListConstString m_poolConstString;
	};

	class ConstStringHolder
	{
	public:
		ConstStringHolder( ConstManager * _manager )
			: m_manager(_manager)
			, m_reference(1)
		{
		}

	public:
		void cache( ConstManager::TMapConstString::iterator _iterator )
		{
			m_iterator = _iterator;
		}

	public:
		const String & getString() const
		{
			return m_iterator->first;
		}

		ConstString genString( const String & _value )
		{
			return m_manager->genString( _value );
		}

	public:
		void incref()
		{
			++m_reference;
		}

		void decref()
		{
			if( --m_reference == 0 )
			{
				m_manager->removeString( m_iterator );
			}
		}

	protected:
		ConstManager::TMapConstString::iterator m_iterator;
		ConstManager * m_manager;
		std::size_t m_reference;
	};

	inline static void conststring_incref( ConstStringHolder * _holder )
	{
		_holder->incref();
	}

	inline static void conststring_decref( ConstStringHolder * _holder )
	{
		_holder->decref();
	}

	inline static const String & conststring_get( ConstStringHolder * _holder )
	{
		return _holder->getString();
	}

	inline static void conststring_gen( ConstString & _out, ConstStringHolder * _holder, const String & _str )
	{
		_out = _holder->genString( _str );
	}

	namespace Utils
	{
		const ConstString & emptyConstString();
	}
}
