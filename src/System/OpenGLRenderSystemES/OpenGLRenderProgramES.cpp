#	include "OpenGLRenderProgramES.h"
#	include "OpenGLRenderErrorES.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OpenGLRenderProgramES::OpenGLRenderProgramES()
		: m_serviceProvider( nullptr )
		, m_program( 0 )
		, m_samplerCount( 0 )
		, m_transformLocation( -1 )
	{
		for( uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			m_samplerLocation[i] = -1;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	OpenGLRenderProgramES::~OpenGLRenderProgramES()
	{
		if( m_program != 0 )
		{
			GLCALL( m_serviceProvider, glDeleteProgram, (m_program) );
			m_program = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderProgramES::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * OpenGLRenderProgramES::getServiceProvider() const
	{ 
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & OpenGLRenderProgramES::getName() const
	{ 
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderFragmentShaderInterfacePtr OpenGLRenderProgramES::getFragmentShader() const
	{
		return m_fragmentShader;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertexShaderInterfacePtr OpenGLRenderProgramES::getVertexShader() const
	{
		return m_vertexShader;
	}
    //////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderProgramES::initialize( const ConstString & _name, const OpenGLRenderVertexShaderESPtr & _vertexShader, const OpenGLRenderFragmentShaderESPtr & _fragmentShader, uint32_t _samplerCount )
	{
		m_name = _name;
        m_vertexShader = _vertexShader;
        m_fragmentShader = _fragmentShader;
		m_samplerCount = _samplerCount;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgramES::compile()
    {
		if( m_samplerCount > MENGE_MAX_TEXTURE_STAGES )
		{
			LOGGER_ERROR( m_serviceProvider )("OpenGLRenderProgramES::initialize %s don't support sampler count %d max %d"
				, m_name.c_str()
				, m_samplerCount
				, MENGE_MAX_TEXTURE_STAGES
				);

			return false;
		}

		GLuint program;
		GLCALLR( m_serviceProvider, program, glCreateProgram, () );

		if( program == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("OpenGLRenderProgramES::initialize %s invalid create program"
				, m_name.c_str()
				);

			return false;
		}

		if( m_vertexShader != nullptr )
		{
			m_vertexShader->attach( program );
		}

		if( m_fragmentShader != nullptr )
		{
			m_fragmentShader->attach( program );
		}
		
		GLCALL( m_serviceProvider, glBindAttribLocation, ( program, VERTEX_POSITION_ARRAY, "inVert" ) );
		GLCALL( m_serviceProvider, glBindAttribLocation, ( program, VERTEX_COLOR_ARRAY, "inCol" ) );

		for( uint32_t i = 0; i != MENGINE_RENDER_VERTEX_UV_COUNT; ++i )
		{
			char attrib[16];
			sprintf( attrib, "inUV%d", i );
		
			GLCALL( m_serviceProvider, glBindAttribLocation, (program, VERTEX_UV0_ARRAY + i, attrib) );
		}

		GLCALL( m_serviceProvider, glLinkProgram, ( program ) );

		GLint linked;
		GLCALL( m_serviceProvider, glGetProgramiv, ( program, GL_LINK_STATUS, &linked ) );

		if( linked == GL_FALSE )
		{
			GLchar errorLog[1024] = {0};
			GLCALL( m_serviceProvider, glGetProgramInfoLog, ( program, 1023, NULL, errorLog ) );

			LOGGER_ERROR( m_serviceProvider )("OpenGLRenderProgramES::shaderProgram '%s' - shader linking error '%s'"
                , m_name.c_str()
				, errorLog
				);

			return false;
		}
				
		int transformLocation;
		GLCALLR( m_serviceProvider, transformLocation, glGetUniformLocation, (program, "mvpMat") );

		m_transformLocation = transformLocation;

		for( uint32_t index = 0; index != m_samplerCount; ++index )
		{
			char samplerVar[16];
			sprintf( samplerVar, "inSampler%d", index );

			int location;
			GLCALLR( m_serviceProvider, location, glGetUniformLocation, (program, samplerVar) );

			m_samplerLocation[index] = location;
		}

		m_program = program;

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderProgramES::enable() const
	{
		GLCALL( m_serviceProvider, glUseProgram, (m_program) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderProgramES::disable() const
	{
		GLCALL( m_serviceProvider, glUseProgram, (0) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderProgramES::bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix ) const
	{
		m_mvpMat = _worldMatrix * _viewMatrix * _projectionMatrix;

		GLCALL( m_serviceProvider, glUniformMatrix4fv, (m_transformLocation, 1, GL_FALSE, m_mvpMat.buff()) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderProgramES::bindTexture( uint32_t _index ) const
	{	
		if( _index >= m_samplerCount )
		{
			LOGGER_ERROR( m_serviceProvider )("OpenGLRenderProgramES::bindTexture %s invalid support sampler count %d max %d"
				, m_name.c_str()
				, _index
				, m_samplerCount
				);

			return;
		}

		int location = m_samplerLocation[_index];

		GLCALL( m_serviceProvider, glUniform1i, (location, _index) );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
