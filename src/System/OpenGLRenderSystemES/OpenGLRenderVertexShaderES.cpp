#	include "OpenGLRenderVertexShaderES.h"
#	include "OpenGLRenderErrorES.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OpenGLRenderVertexShaderES::OpenGLRenderVertexShaderES()
		: m_serviceProvider( nullptr )
		, m_shaderId( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OpenGLRenderVertexShaderES::~OpenGLRenderVertexShaderES()
	{
		if( m_shaderId != 0 )
		{
			GLCALL( m_serviceProvider, glDeleteShader, (m_shaderId) );
			m_shaderId = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderVertexShaderES::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * OpenGLRenderVertexShaderES::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & OpenGLRenderVertexShaderES::getName() const
	{
		return m_name;
	}
    //////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderVertexShaderES::initialize( const ConstString & _name, const void * _source, size_t _size, bool _isCompile )
	{
		m_name = _name;

		GLuint shaderId;		
		GLCALLR( m_serviceProvider, shaderId, glCreateShader, (GL_VERTEX_SHADER) );

		if( shaderId == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeShader::initialize %s invalid create shader"
				, _name.c_str()
				);

			return false;
		}

		if( _isCompile == false )
		{
			const char * str_source = static_cast<const char *>(_source);
			GLint str_size = (GLint)_size;

			GLCALL( m_serviceProvider, glShaderSource, ( shaderId, 1, &str_source, &str_size ) );
			GLCALL( m_serviceProvider, glCompileShader, ( shaderId ) );
		}
		else
		{
			//GLCALL( m_serviceProvider, glShaderBinary, (1, &shaderId, GL_SHADER_BINARY_ANGLE_DX11, _source, _size) );
			return false;
		}

		GLint status;
		GLCALL( m_serviceProvider, glGetShaderiv, ( shaderId, GL_COMPILE_STATUS, &status ) );

		if( status == GL_FALSE )
		{
			GLchar errorLog[1024];
			GLCALL( m_serviceProvider, glGetShaderInfoLog, ( shaderId, 1023, NULL, errorLog ) );

			LOGGER_ERROR(m_serviceProvider)("MarmaladeShader::initialize compilation shader error '%s'"
				, errorLog
				);			

			return false;
		}

		m_shaderId = shaderId;

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderVertexShaderES::attach( GLuint _program )
	{ 
		GLCALL( m_serviceProvider, glAttachShader, ( _program, m_shaderId ) );
	}
}	// namespace Menge
