#include "OpenGLRenderVertexShader.h"
#include "OpenGLRenderErrorHelper.h"

#include "Environment/OpenGL/OpenGLRenderSystemExtensionInterface.h"

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
        MENGINE_ASSERTION_FATAL( m_shaderId == 0, "shader is not release" );
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
    void OpenGLRenderVertexShader::finalize()
    {
        MENGINE_ASSERTION_FATAL( this->getCompileReferenceCount() == 0, "shader '%s' is used"
            , m_name.c_str()
        );

        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexShader::_compile()
    {
        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        GLuint shaderId = extension->genVertexShader();

        if( shaderId == 0 )
        {
            LOGGER_ERROR( "'%s' invalid create shader"
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
            GLchar errorLog[1024] = {'\0'};
            MENGINE_GLCALL( glGetShaderInfoLog, (shaderId, 1023, NULL, errorLog) );

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
    void OpenGLRenderVertexShader::_release()
    {
        if( m_shaderId != 0 )
        {
            OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
                ->getUnknown();
            
            extension->deleteVertexShader( m_shaderId );
            m_shaderId = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexShader::attach( GLuint _program )
    {
        MENGINE_GLCALL( glAttachShader, (_program, m_shaderId) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexShader::onRenderReset()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexShader::onRenderRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
