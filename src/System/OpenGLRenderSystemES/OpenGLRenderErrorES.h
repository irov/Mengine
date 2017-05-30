#	pragma once

#   include "Interface/ServiceInterface.h"

namespace Menge
{
	bool openglRenderErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line );
}

#	define OPENGL_RENDER_CHECK_ERROR(serviceProvider) Menge::openglRenderErrorCheck( serviceProvider, __FILE__, __LINE__ )

#	define GLCALL( ServiceProvider, Method, Args )\
	do{\
		Method Args;\
		OPENGL_RENDER_CHECK_ERROR(ServiceProvider);\
		}while(false);	

#	define GLCALLR( ServiceProvider, R, Method, Args )\
	do{\
		R = Method Args;\
		OPENGL_RENDER_CHECK_ERROR(ServiceProvider);\
				}while(false);

#	define IF_GLCALL( ServiceProvider, Method, Args )\
	Method Args;\
	if( OPENGL_RENDER_CHECK_ERROR(ServiceProvider) == true )
