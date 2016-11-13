#   include "OpenGLRenderVertexShader.h"

#   include "OpenGLRenderError.h"

#   include "Logger/Logger.h"


namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexShader::OpenGLRenderVertexShader()
        : m_serviceProvider( nullptr )
        , m_shaderId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderVertexShader::~OpenGLRenderVertexShader()
    {
        if( m_shaderId != 0 )
        {
            GLCALL( m_serviceProvider, glDeleteShader, (m_shaderId) );
            m_shaderId = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexShader::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * OpenGLRenderVertexShader::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & OpenGLRenderVertexShader::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderVertexShader::initialize( const ConstString & _name, const void * _source, size_t _size, bool _isCompile )
    {
        m_name = _name;

        GLuint shaderId;		
        GLCALLR( m_serviceProvider, shaderId, glCreateShader, (GL_VERTEX_SHADER) );

        if( shaderId == 0 )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderVertexShader::initialize %s invalid create shader"
                , _name.c_str()
                );

            return false;
        }

        if( _isCompile == false )
        {
            const char * str_source = static_cast<const char *>(_source);
            GLint str_size = (GLint)_size;

            GLCALL( m_serviceProvider, glShaderSource, ( shaderId, 1, &str_source, &str_size ) );
            GLCALL( m_serviceProvider, glCompileShader, ( shaderId ) );
        }
        else
        {
            // binary shaders are not supported
        }

        GLint status;
        GLCALL( m_serviceProvider, glGetShaderiv, ( shaderId, GL_COMPILE_STATUS, &status ) );

        if( status == GL_FALSE )
        {
            GLchar errorLog[1024];
            GLCALL( m_serviceProvider, glGetShaderInfoLog, ( shaderId, sizeof(errorLog) - 1, nullptr, errorLog ) );

            LOGGER_ERROR(m_serviceProvider)("OpenGLRenderVertexShader::initialize compilation shader error '%s'"
                , errorLog
                );

            return false;
        }

        m_shaderId = shaderId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderVertexShader::attach( GLuint _program )
    { 
        GLCALL( m_serviceProvider, glAttachShader, ( _program, m_shaderId ) );
    }
}	// namespace Menge
