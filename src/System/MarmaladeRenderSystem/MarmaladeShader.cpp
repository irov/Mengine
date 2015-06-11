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
		, m_program( 0 )
		, m_vertexShader( 0 )
		, m_fragmentShader( 0 )
		, m_transformLocation( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeShader::~MarmaladeShader()
	{
	}
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladeShader::initialize(ServiceProviderInterface * _serviceProvider, const char * _vsSrc, const char * _fsSrc)
	{
		m_serviceProvider = _serviceProvider;

		GLuint program = glCreateProgram();

		if (program == 0)
		{
			return false;
		}

		GLuint vertexShader = this->createShader_(GL_VERTEX_SHADER, _vsSrc);

		if (vertexShader == 0)
		{
			return false;
		}

		GLuint fragmentShader = this->createShader_(GL_FRAGMENT_SHADER, _fsSrc);

		if (fragmentShader == 0)
		{
			return false;
		}

		if ((s3eDeviceGetInt(S3E_DEVICE_OS) != S3E_OS_ID_WS8 &&
			s3eDeviceGetInt(S3E_DEVICE_OS) != S3E_OS_ID_WS81 &&
			s3eDeviceGetInt(S3E_DEVICE_OS) != S3E_OS_ID_WP81
			) ||
			s3eDeviceGetInt(S3E_DEVICE_DX_FEATURE_LEVEL) < 93
			)
		{
			// This is needed to compile shaders for Windows Store 8/8.1 and Windows Phone 8.1 using the Win32 Marmalade Simulator.
			// For more information look at README.IwGLES2.txt.
			if (IwGetCompileShadersPlatformType() == IW_CS_OS_ID_WS8 ||
				IwGetCompileShadersPlatformType() == IW_CS_OS_ID_WS81 ||
				IwGetCompileShadersPlatformType() == IW_CS_OS_ID_WP81
				)
			{
				s3eRegisterShader(_vsSrc, IW_GL_ST_VERTEX, IW_DX_FL_9_3);
				s3eRegisterShader(_fsSrc, IW_GL_ST_PIXEL, IW_DX_FL_9_3);
			}
		}

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glBindAttribLocation(program, VERTEX_ARRAY, "inVert");
		glBindAttribLocation(program, COLOR_ARRAY, "inCol");
		glBindAttribLocation(program, UV0_ARRAY, "inUV0");
		glBindAttribLocation(program, UV1_ARRAY, "inUV1");

		glLinkProgram(program);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);

		if (linked == GL_FALSE)
		{
			LOGGER_ERROR(m_serviceProvider)("RenderMaterialManager::shaderProgram - shader linking error");

			return false;
		}

		glUseProgram(program);

		int transformLocation = glGetUniformLocation(program, "mvpMat");

		m_program = program;
		m_vertexShader = vertexShader;
		m_fragmentShader = fragmentShader;

		m_transformLocation = transformLocation;
		m_samplerLocation[0] = glGetUniformLocation(program, "inSampler0");
		m_samplerLocation[1] = glGetUniformLocation(program, "inSampler1");

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeShader::use(const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix) const
	{
		glUseProgram(m_program);

		m_mvpMat = _worldMatrix * _viewMatrix * _projectionMatrix;

		glUniformMatrix4fv(m_transformLocation, 1, GL_FALSE, m_mvpMat.buff());
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeShader::bindTexture(unsigned int _textureInd, int _texture) const
	{
		glActiveTexture(GL_TEXTURE0 + _textureInd);
		glBindTexture(GL_TEXTURE_2D, _texture);
		if (_textureInd > 1 || m_samplerLocation[_textureInd] < 0)
			return;
		glUniform1i(m_samplerLocation[_textureInd], _textureInd);
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeShader::finalize()
	{ 
		glDeleteProgram( m_program );

		glDeleteShader( m_vertexShader );
		glDeleteShader( m_fragmentShader );
	}
	//////////////////////////////////////////////////////////////////////////
	GLuint MarmaladeShader::createShader_( GLenum type, const char * _source )
	{
		GLuint shader = glCreateShader( type );

		if( shader == 0 )
		{
			return 0;
		}

		glShaderSource( shader, 1, &_source, NULL );
		glCompileShader( shader );

		return shader;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
