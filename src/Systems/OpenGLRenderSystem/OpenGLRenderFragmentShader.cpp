#include "OpenGLRenderFragmentShader.h"
#include "OpenGLRenderError.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderFragmentShader::OpenGLRenderFragmentShader()
        : m_shaderId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderFragmentShader::~OpenGLRenderFragmentShader()
    {
        MENGINE_ASSERTION_FATAL( m_shaderId == 0 );
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
    void OpenGLRenderFragmentShader::finalize()
    {
        MENGINE_ASSERTION_FATAL( this->getCompileReferenceCount() == 0 );

        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderFragmentShader::_compile()
    {
        GLuint shaderId;
        GLCALLR( shaderId, glCreateShader, (GL_FRAGMENT_SHADER) );

        if( shaderId == 0 )
        {
            LOGGER_ERROR( "'%s' invalid create fragment shader"
                , m_name.c_str()
            );

            return false;
        }

        const Char * str_source = m_memory->getBuffer();
        GLint str_size = (GLint)m_memory->getSize();

        GLCALL( glShaderSource, (shaderId, 1, &str_source, &str_size) );
        GLCALL( glCompileShader, (shaderId) );

        GLint status;
        GLCALL( glGetShaderiv, (shaderId, GL_COMPILE_STATUS, &status) );

        if( status == GL_FALSE )
        {
            GLchar errorLog[1024];
            GLCALL( glGetShaderInfoLog, (shaderId, 1023, NULL, errorLog) );

            LOGGER_ERROR( "compilation fragment shader '%s' error '%s'"
                , m_name.c_str()
                , errorLog
            );

            return false;
        }

        m_shaderId = shaderId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderFragmentShader::_release()
    {
        if( m_shaderId != 0 )
        {
            GLCALL( glDeleteShader, (m_shaderId) );
            m_shaderId = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderFragmentShader::attach( GLuint _program )
    {
        GLCALL( glAttachShader, (_program, m_shaderId) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderFragmentShader::onRenderReset()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderFragmentShader::onRenderRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
