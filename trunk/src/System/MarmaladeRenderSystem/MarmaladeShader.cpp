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
	bool MarmaladeShader::initialize( GLenum _type, const void * _source, size_t _size, bool _isCompile )
	{
		GLuint shaderId = glCreateShader( _type );

		if( shaderId == 0 )
		{
			return false;
		}

		if( _isCompile == false )
		{
			const char * str_source = static_cast<const char *>(_source);
			GLint str_size = (GLint)_size;

			glShaderSource( shaderId, 1, &str_source, &str_size );
			glCompileShader( shaderId );

			if( (s3eDeviceGetInt( S3E_DEVICE_OS ) != S3E_OS_ID_WS8 &&
				s3eDeviceGetInt( S3E_DEVICE_OS ) != S3E_OS_ID_WS81 &&
				s3eDeviceGetInt( S3E_DEVICE_OS ) != S3E_OS_ID_WP81
				) ||
				s3eDeviceGetInt( S3E_DEVICE_DX_FEATURE_LEVEL ) < 93
				)
			{
				// This is needed to compile shaders for Windows Store 8/8.1 and Windows Phone 8.1 using the Win32 Marmalade Simulator.
				// For more information look at README.IwGLES2.txt.
				if( IwGetCompileShadersPlatformType() == IW_CS_OS_ID_WS8 ||
					IwGetCompileShadersPlatformType() == IW_CS_OS_ID_WS81 ||
					IwGetCompileShadersPlatformType() == IW_CS_OS_ID_WP81
					)
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
			glShaderBinary( 1, &shaderId, 0x93B0, _source, _size );
		}

		GLint status;
		glGetShaderiv( shaderId, GL_COMPILE_STATUS, &status );

		if( status == GL_FALSE )
		{
			GLint infoLogLength;
			glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &infoLogLength );

			GLchar* strInfoLog = new GLchar[infoLogLength + 1];
			glGetShaderInfoLog( shaderId, infoLogLength, NULL, strInfoLog );

			LOGGER_ERROR(m_serviceProvider)("MarmaladeShader::initialize compilation shader error '%s'"
				, strInfoLog 
				);
			
			delete[] strInfoLog;

			return false;
		}

		m_shaderId = shaderId;

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeShader::finalize()
	{ 
		glDeleteShader( m_shaderId );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeShader::attach( GLuint _program )
	{ 
		glAttachShader( _program, m_shaderId );
	}
}	// namespace Menge
