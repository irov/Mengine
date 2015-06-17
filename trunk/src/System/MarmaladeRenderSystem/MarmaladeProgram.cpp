#	include "MarmaladeProgram.h"

#	include "MarmaladeRenderError.h"

#	include "Logger/Logger.h"

#	define VERTEX_ARRAY 0
#	define COLOR_ARRAY 1
#	define UV0_ARRAY 2
#	define UV1_ARRAY 3

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeProgram::MarmaladeProgram()
		: m_serviceProvider( nullptr )
		, m_program( 0 )
		, m_vertexShader( 0 )
		, m_fragmentShader( 0 )
		, m_transformLocation( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeProgram::~MarmaladeProgram()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * MarmaladeProgram::getServiceProvider()
	{ 
		return m_serviceProvider;
	}
    //////////////////////////////////////////////////////////////////////////
	bool MarmaladeProgram::initialize( const RenderShaderInterfacePtr & _vertexShader, const RenderShaderInterfacePtr & _fragmentShader )
	{
		GLuint program = glCreateProgram();

		if( program == 0 )
		{
			return false;
		}

		if( _vertexShader != nullptr )
		{
			m_vertexShader = stdex::intrusive_static_cast<MarmaladeShaderPtr>(_vertexShader);

			m_vertexShader->attach( program );
		}

		if( _fragmentShader != nullptr )
		{
			m_fragmentShader = stdex::intrusive_static_cast<MarmaladeShaderPtr>(_fragmentShader);

			m_fragmentShader->attach( program );
		}
		
		glBindAttribLocation( program, VERTEX_ARRAY, "inVert" );
		glBindAttribLocation( program, COLOR_ARRAY, "inCol" );
		glBindAttribLocation( program, UV0_ARRAY, "inUV0" );
		glBindAttribLocation( program, UV1_ARRAY, "inUV1" );

		glLinkProgram( program );

		GLint linked;
		glGetProgramiv( program, GL_LINK_STATUS, &linked );

		if (linked == GL_FALSE)
		{
			LOGGER_ERROR(m_serviceProvider)("RenderMaterialManager::shaderProgram - shader linking error");

			return false;
		}

		glUseProgram( program );

		int transformLocation = glGetUniformLocation( program, "mvpMat" );

		m_program = program;

		
		m_fragmentShader = _fragmentShader;

		m_transformLocation = transformLocation;
		m_samplerLocation[0] = glGetUniformLocation( program, "inSampler0" );
		m_samplerLocation[1] = glGetUniformLocation( program, "inSampler1" );

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::use() const
	{
		glUseProgram( m_program );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix ) const
	{
		m_mvpMat = _worldMatrix * _viewMatrix * _projectionMatrix;

		glUniformMatrix4fv( m_transformLocation, 1, GL_FALSE, m_mvpMat.buff() );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::bindTexture( unsigned int _textureInd, int _texture ) const
	{
		glActiveTexture(GL_TEXTURE0 + _textureInd);
		glBindTexture(GL_TEXTURE_2D, _texture);
		
		if( _textureInd > 1 || m_samplerLocation[_textureInd] < 0 )
		{
			return;
		}

		glUniform1i(m_samplerLocation[_textureInd], _textureInd);
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeProgram::finalize()
	{ 
		glDeleteProgram( m_program );

		m_vertexShader = nullptr;
		m_fragmentShader = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
