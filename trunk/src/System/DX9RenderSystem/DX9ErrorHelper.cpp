#	include "DX9ErrorHelper.h"

#	include <dxerr.h>

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9ErrorHelper::DX9ErrorHelper( ServiceProviderInterface * _serviceProvider, const char * _file, size_t _line, const char * _method )
		: m_serviceProvider(_serviceProvider)
		, m_file(_file)
		, m_line(_line)
		, m_method(_method)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9ErrorHelper::operator == ( HRESULT _hr )
	{
		if( _hr == S_OK )
		{
			return false;
		}

		LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem error file %s line %d DX call '%s':\n err string: %s\n err description: %s"
			, m_file
			, m_line
			, m_method
			, DXGetErrorStringA(_hr)
			, DXGetErrorDescriptionA(_hr)
			);

		return true;
	}
}