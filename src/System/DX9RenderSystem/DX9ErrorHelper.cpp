#	include "DX9ErrorHelper.h"

#	include "dxerr15.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9ErrorHelper::DX9ErrorHelper( ServiceProviderInterface * _serviceProvider, const char * _file, uint32_t _line, const char * _method )
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

		WCHAR desc[1024];

		DXGetErrorDescription(_hr, desc, 1024);

		LOGGER_ERROR(m_serviceProvider)("DX9RenderSystem error file %s line %d DX call '%s':\n err string: %ls\n err description: %ls"
			, m_file
			, m_line
			, m_method
			, DXGetErrorString(_hr)
			, desc
			);

		return true;
	}
}