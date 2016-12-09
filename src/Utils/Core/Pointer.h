#	pragma once

#	include "stdex/mpl.h"

namespace Menge
{
	class Pointer
	{
	public:
		Pointer( void * _pointer )
			: m_pointer( _pointer )
		{
		}

	public:
		operator void * () const
		{
			return m_pointer;
		}

		template<class U>
		operator U ()
		{
#	ifdef _DEBUG
			if( stdex::mpl::is_dynamic_cast<U>::test( m_pointer ) == false )
			{
                throw;
			}
#	endif

			U t = static_cast<U>(m_pointer);

			return t;
		}

	protected:
		void * m_pointer;
	};

	template<class T>
	class PointerT
	{
	public:
		PointerT( T * _pointer )
			: m_pointer( _pointer )
		{
		}

	public:
		operator T * () const
		{
			return m_pointer;
		}

		template<class U>
		operator U ()
		{
#	ifdef _DEBUG
			if( stdex::is_dynamic_cast<U>::test( m_pointer ) == false )
			{
                throw;
			}
#	endif

			U t = static_cast<U>(m_pointer);

			return t;
		}

	protected:
		T * m_pointer;
	};
}