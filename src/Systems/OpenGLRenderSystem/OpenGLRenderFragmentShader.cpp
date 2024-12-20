#include "OpenGLRenderFragmentShader.h"
#include "OpenGLRenderErrorHelper.h"

#include "Environment/OpenGL/OpenGLRenderSystemExtensionInterface.h"

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
        MENGINE_ASSERTION_FATAL( m_shaderId == 0, "fragment shader is not release" );
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
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderFragmentShader::_compile()
    {
        LOGGER_INFO( "render", "compile fragment shader '%s'"
            , m_name.c_str()
        );

        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        GLuint shaderId = extension->genFragmentShader();

        if( shaderId == 0 )
        {
            LOGGER_ERROR( "invalid create fragment shader '%s'"
                , m_name.c_str()
            );

            return false;
        }

        const Char * str_source = m_memory->getBuffer();
        GLint str_size = (GLint)m_memory->getSize();

        MENGINE_GLCALL( glShaderSource, (shaderId, 1, &str_source, &str_size) );
        MENGINE_GLCALL( glCompileShader, (shaderId) );

        GLint status;
        MENGINE_GLCALL( glGetShaderiv, (shaderId, GL_COMPILE_STATUS, &status) );

        if( status == GL_FALSE )
        {
            GLchar errorLog[1024 + 1] = {'\0'};
            MENGINE_GLCALL( glGetShaderInfoLog, (shaderId, 1024, NULL, errorLog) );

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
        LOGGER_INFO( "render", "release fragment shader '%s'"
            , m_name.c_str()
        );

        if( m_shaderId != 0 )
        {
            OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
                ->getUnknown();

            extension->deleteFragmentShader( m_shaderId );
            m_shaderId = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderFragmentShader::attach( GLuint _program )
    {
        MENGINE_GLCALL( glAttachShader, (_program, m_shaderId) );
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
