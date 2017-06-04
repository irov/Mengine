#	include "OpenGLRenderFragmentShaderES.h"
#	include "OpenGLRenderErrorES.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OpenGLRenderFragmentShaderES::OpenGLRenderFragmentShaderES()
		: m_shaderId( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OpenGLRenderFragmentShaderES::~OpenGLRenderFragmentShaderES()
	{
		if( m_shaderId != 0 )
		{
			GLCALL( m_serviceProvider, glDeleteShader, (m_shaderId) );
			m_shaderId = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & OpenGLRenderFragmentShaderES::getName() const
	{
		return m_name;
	}
    //////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderFragmentShaderES::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _isCompile )
	{
        (void)_isCompile;
        
		m_name = _name;
        
        m_memory = _memory;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderFragmentShaderES::compile()
    {
		GLuint shaderId;
		GLCALLR( m_serviceProvider, shaderId, glCreateShader, ( GL_FRAGMENT_SHADER ) );

		if( shaderId == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("OpenGLRenderFragmentShaderES::initialize %s invalid create shader"
				, m_name.c_str()
				);

			return false;
		}

        const char * str_source = m_memory->getMemory();
		GLint str_size = (GLint)m_memory->getSize();

		GLCALL( m_serviceProvider, glShaderSource, ( shaderId, 1, &str_source, &str_size ) );
		GLCALL( m_serviceProvider, glCompileShader, ( shaderId ) );


		GLint status;
		GLCALL( m_serviceProvider, glGetShaderiv, ( shaderId, GL_COMPILE_STATUS, &status ) );

		if( status == GL_FALSE )
		{
			GLchar errorLog[1024];
			GLCALL( m_serviceProvider, glGetShaderInfoLog, ( shaderId, 1023, NULL, errorLog ) );

			LOGGER_ERROR(m_serviceProvider)("OpenGLRenderFragmentShaderES::initialize compilation shader error '%s'"
				, errorLog
				);			

			return false;
		}

		m_shaderId = shaderId;

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderFragmentShaderES::attach( GLuint _program )
	{ 
		GLCALL( m_serviceProvider, glAttachShader, ( _program, m_shaderId ) );
	}
}	// namespace Menge
