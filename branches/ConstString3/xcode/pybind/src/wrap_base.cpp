#	include "pybind/wrap_base.hpp"
#	include "pybind/system.hpp"

#	include <stdarg.h>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	wrap_base::wrap_base()
		: m_pyImpl(0)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void wrap_base::callMethod( const char * _method, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		call_method_va( m_pyImpl, _method, _format, valist );

		check_error();

		va_end( valist );
	}
}
