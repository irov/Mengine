#	include "MarmaladeShader.h"

#	include "MarmaladeRenderError.h"

#	include "Logger/Logger.h"

#	define VERTEX_ARRAY 0
#	define COLOR_ARRAY 1
#	define UV0_ARRAY 2
#	define UV1_ARRAY 3

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeShader::MarmaladeShader()
		: m_serviceProvider(nullptr)
		, m_shaderId( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeShader::~MarmaladeShader()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeShader::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * MarmaladeShader::getServiceProvider()
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & MarmaladeShader::getName() const
	{
		return m_name;
	}
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladeShader::initialize( const ConstString & _name, GLenum _type, const void * _source, size_t _size, bool _isCompile )
	{
		m_name = _name;

		GLuint shaderId;		
		GLCALLR( m_serviceProvider, shaderId, glCreateShader, ( _type ) );

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

			int32 device = s3eDeviceGetInt( S3E_DEVICE_OS );
			int32 deviceFeatureLevel = s3eDeviceGetInt( S3E_DEVICE_DX_FEATURE_LEVEL );

			if( (device != S3E_OS_ID_WS8 && device != S3E_OS_ID_WS81 && device != S3E_OS_ID_WP81) ||
				deviceFeatureLevel < 93 )
			{
				IwCompileShadersPlatformType shadersPlatformType = IwGetCompileShadersPlatformType();
				// This is needed to compile shaders for Windows Store 8/8.1 and Windows Phone 8.1 using the Win32 Marmalade Simulator.
				// For more information look at README.IwGLES2.txt.
				if( shadersPlatformType == IW_CS_OS_ID_WS8 ||
					shadersPlatformType == IW_CS_OS_ID_WS81 ||
					shadersPlatformType == IW_CS_OS_ID_WP81 )
				{
					if( _type == GL_VERTEX_SHADER )
					{
						s3eRegisterShader( str_source, IW_GL_ST_VERTEX, IW_DX_FL_9_3 );
					}
					else if( _type == GL_FRAGMENT_SHADER )
					{
						s3eRegisterShader( str_source, IW_GL_ST_PIXEL, IW_DX_FL_9_3 );
					}
				}
			}
		}
		else
		{
			GLCALL( m_serviceProvider, glShaderBinary, (1, &shaderId, GL_SHADER_BINARY_ANGLE_DX11, _source, _size) );
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
	void MarmaladeShader::finalize()
	{ 
		GLCALL( m_serviceProvider, glDeleteShader, ( m_shaderId ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeShader::attach( GLuint _program )
	{ 
		GLCALL( m_serviceProvider, glAttachShader, ( _program, m_shaderId ) );
	}
}	// namespace Menge
