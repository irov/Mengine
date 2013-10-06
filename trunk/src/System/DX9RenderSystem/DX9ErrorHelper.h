#	pragma once

#	include <d3d9.h>

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class DX9ErrorHelper
	{
	public:
		DX9ErrorHelper( ServiceProviderInterface * _serviceProvider, const char * _file, size_t _line, const char * _method );
		
	public:
		bool operator == ( HRESULT _hr );

	protected:
		ServiceProviderInterface * m_serviceProvider;
		const char * m_file;
		size_t m_line;
		const char * m_method;
	};
	//////////////////////////////////////////////////////////////////////////
#	define DXERRORCHECK( ServiceProvider, Method, HRES )\
	(DX9ErrorHelper(ServiceProvider, __FILE__, __LINE__, Method ) == HRES)
	//////////////////////////////////////////////////////////////////////////
#	define IF_DXERRORCHECK( ServiceProvider, Method, HRES )\
	if( DXERRORCHECK(ServiceProvider, #Method, HRES) )
	//////////////////////////////////////////////////////////////////////////
#	define DXCALL( ServiceProvider, Device, Method, Args )\
	(DXERRORCHECK(ServiceProvider, #Method, Device -> Method Args))
	//////////////////////////////////////////////////////////////////////////
#	define IF_DXCALL( ServiceProvider, Device, Method, Args )\
	if( DXCALL(ServiceProvider, Device, Method, Args) )
}