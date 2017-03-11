#	pragma once

#	include "stdex/mpl.h"
#   include "stdex/intrusive_ptr.h"

namespace Menge
{
	template<class T>
	class PointerT
	{
	public:
		PointerT( T * _pointer )
			: m_pointer( _pointer )
		{
		}

        PointerT( const PointerT & _pointer )
            : m_pointer( _pointer.m_pointer )
        {
        }

	public:
		operator T * () const
		{
			return m_pointer;
		}

		template<class U>
		operator U * ()
		{
#	ifdef _DEBUG
			if( stdex::mpl::is_dynamic_cast<U *>::test( m_pointer ) == false )
			{
                throw;
			}
#	endif

			U * t = static_cast<U *>(m_pointer);

			return t;
		}

	protected:
		T * m_pointer;
	};

    typedef PointerT<void> Pointer;
}