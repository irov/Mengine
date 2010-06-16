#	pragma once

#	include "Config/Typedef.h"

#	include <map>
#	include <vector>
#	include <string>

namespace Menge
{
	class ConstStringManager;

	namespace Helper
	{
		class ConstStringHolder
		{
		public:	
			ConstStringHolder( const String & _value, std::size_t, ConstStringManager * _manager )
				: m_value(_value)
				, m_manager(_manager)
				, m_reference(1)
			{
			}

		public:
			const String & getString() const
			{
				return m_value;
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
					m_manager->removeString( m_value );
				}
			}

		protected:
			String m_value;
			ConstStringManager * m_manager;
			std::size_t m_reference;
		};
	}

	class ConstString
	{
	public:
		ConstString( const ConstString & _string )
			: m_holder(_string.m_holder)
		{
			m_holder->incref();
		}

		~ConstString()
		{
			m_holder->decref();
		}

	protected:
		ConstString( ConstStringHolder * _holder )
			: m_holder(_holder)
		{
			m_holder->incref();
		}

	public:
		ConstString & operator = ( const ConstString & _string )
		{
			m_holder = _string.m_holder;

			return *this;
		}

	public:
		const String & getString() const
		{
			return m_holder->getString();
		}

	public:
		bool operator == ( const ConstString & _string ) const
		{
			return m_holder == _string.m_holder;
		}

	protected:
		ConstStringHolder * m_holder;

		friend class ConstStringManager;
	};

	class ConstStringManager
	{
	public:
		ConstString cacheString( const String & _name );

		std::size_t findString( const String & _name ) const;
		const std::string & getString( std::size_t _identity ) const;
		
		void removeString( const String & _name );

	protected:
		typedef std::list<ConstStringHolder> TListConstString;
		typedef std::map<std::string, TListConstString::iterator> TMapConstString;

		TMapConstString m_cacheConstString;
		TListConstString m_poolConstString;
	};
}
