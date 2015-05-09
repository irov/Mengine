#	pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
	bool openglRenderErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line );
}

#	define OPENGL_RENDER_CHECK_ERROR(serviceProvider) Menge::openglRenderErrorCheck( serviceProvider, __FILE__, __LINE__ )

#	define GLCALL( ServiceProvider, Method, Args )\
	{\
		Method Args;\
		OPENGL_RENDER_CHECK_ERROR(ServiceProvider);\
	}
	

#	define IF_GLCALL( ServiceProvider, Method, Args )\
	for( bool __check = false; __check == false; __check = true, Method Args )\
	if( OPENGL_RENDER_CHECK_ERROR(ServiceProvider) == true )
