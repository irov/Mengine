#include "OpenGLRenderVertexShader.h"
#include "OpenGLRenderError.h"

#include "Kernel/Logger.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexShader::OpenGLRenderVertexShader()
        : m_shaderId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexShader::~OpenGLRenderVertexShader()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & OpenGLRenderVertexShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexShader::initialize( const ConstString & _name, const MemoryInterfacePtr & _memory )
    {
        m_name = _name;
        m_memory = _memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexShader::compile()
    {
        GLuint shaderId;
        GLCALLR( shaderId, glCreateShader, (GL_VERTEX_SHADER) );

        if( shaderId == 0 )
        {
            LOGGER_ERROR("'%s' invalid create shader"
                , m_name.c_str()
                );

            return false;
        }

        const Char * str_source = m_memory->getMemory();
        GLint str_size = (GLint)m_memory->getSize();

        GLCALL( glShaderSource, (shaderId, 1, &str_source, &str_size) );
        GLCALL( glCompileShader, (shaderId) );

        GLint status;
        GLCALL( glGetShaderiv, (shaderId, GL_COMPILE_STATUS, &status) );

        if( status == GL_FALSE )
        {
            GLchar errorLog[1024];
            GLCALL( glGetShaderInfoLog, (shaderId, 1023, NULL, errorLog) );

            LOGGER_ERROR( "compilation vertex shader '%s' error '%s'"
                , m_name.c_str()
                , errorLog
                );

            return false;
        }

        m_shaderId = shaderId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexShader::release()
    {
        if( m_shaderId != 0 )
        {
            GLCALL( glDeleteShader, (m_shaderId) );
            m_shaderId = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexShader::attach( GLuint _program )
    { 
        GLCALL( glAttachShader, ( _program, m_shaderId ) );
    }
}
