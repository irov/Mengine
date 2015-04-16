#	include "MarmaladeRenderError.h"

#	include "Logger/Logger.h"

#   include <IwGL.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const char * glGetErrorString( GLenum _err )
	{
		switch( _err )
		{
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW";
		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW";
		default:
			{
			}
		}

		return "GL_UNKNOWN";
	}
	//////////////////////////////////////////////////////////////////////////
	bool marmaladeRenderErrorCheck( ServiceProviderInterface * _serviceProvider, const char * _file, int _line )
	{
		GLenum err = glGetError();

		if( err == GL_NO_ERROR )
		{
			return false;
		}

		const char * err_str = glGetErrorString( err );		

		LOGGER_ERROR(_serviceProvider)("Marmalade render: %s:%d error %s:%d"
			, _file
			, _line
			, err_str
			, err 
			);

		glErr

        return true;
	}
}
