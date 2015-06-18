#	pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
	bool marmaladeRenderErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line );
}

#	define MARMALADE_RENDER_CHECK_ERROR(serviceProvider) Menge::marmaladeRenderErrorCheck( serviceProvider, __FILE__, __LINE__ )

#	define GLCALL( ServiceProvider, Method, Args )\
	do{\
		Method Args;\
		MARMALADE_RENDER_CHECK_ERROR(ServiceProvider);\
	}while(false);	
	
#	define GLCALLR( ServiceProvider, R, Method, Args )\
	do{\
		R = Method Args;\
		MARMALADE_RENDER_CHECK_ERROR(ServiceProvider);\
			}while(false);

#	define IF_GLCALL( ServiceProvider, Method, Args )\
	for( bool __check = false; __check == false; __check = true, Method Args )\
	if( MARMALADE_RENDER_CHECK_ERROR(ServiceProvider) == true )
