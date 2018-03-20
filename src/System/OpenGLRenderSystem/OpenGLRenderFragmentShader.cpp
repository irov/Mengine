#	include "OpenGLRenderFragmentShader.h"

#	include "OpenGLRenderError.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderFragmentShader::OpenGLRenderFragmentShader()
        : m_shaderId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderFragmentShader::~OpenGLRenderFragmentShader()
    {
        if( m_shaderId != 0 )
        {
            GLCALL( glDeleteShader, (m_shaderId) );
            m_shaderId = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & OpenGLRenderFragmentShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderFragmentShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory )
    {
        m_name = _name;
        m_memory = _memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderFragmentShader::compile()
    {
        GLuint shaderId;
        GLCALLR( shaderId, glCreateShader, ( GL_FRAGMENT_SHADER ) );

        if( shaderId == 0 )
        {
            LOGGER_ERROR("OpenGLRenderFragmentShader::initialize %s invalid create shader"
                , m_name.c_str()
                );

            return false;
        }

        const char * str_source = m_memory->getMemory();
        GLint str_size = (GLint)m_memory->getSize();

        GLCALL( glShaderSource, (shaderId, 1, &str_source, &str_size) );
        GLCALL( glCompileShader, (shaderId) );

        GLint status;
        GLCALL( glGetShaderiv, ( shaderId, GL_COMPILE_STATUS, &status ) );

        if( status == GL_FALSE )
        {
            GLchar errorLog[1024];
            GLCALL( glGetShaderInfoLog, ( shaderId, sizeof(errorLog) - 1, NULL, errorLog ) );

            LOGGER_ERROR("OpenGLRenderFragmentShader::initialize '%s' compilation shader error '%s'"
                , m_name.c_str()
                , errorLog
                );

            return false;
        }

        m_shaderId = shaderId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderFragmentShader::attach( GLuint _program )
    { 
        GLCALL( glAttachShader, ( _program, m_shaderId ) );
    }
}	// namespace Menge
