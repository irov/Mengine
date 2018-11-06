#include "OpenGLRenderProgram.h"

#include "OpenGLRenderError.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgram::OpenGLRenderProgram()
        : m_program( 0 )
        , m_samplerCount( 0 )
    {
        for( uint32_t i = 0; i != EPML_MAX_COUNT; ++i )
        {
            m_matrixLocation[i] = -1;
        }

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            m_samplerLocation[i] = -1;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgram::~OpenGLRenderProgram()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderProgram::getProgramId() const
    {
        return m_program;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & OpenGLRenderProgram::getName() const
    { 
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr OpenGLRenderProgram::getVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr OpenGLRenderProgram::getFragmentShader() const
    {
        return m_fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr OpenGLRenderProgram::getVertexShader() const
    {
        return m_vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgram::initialize( const ConstString & _name, const OpenGLRenderVertexShaderPtr & _vertexShader, const OpenGLRenderFragmentShaderPtr & _fragmentShader, const OpenGLRenderVertexAttributePtr & _vertexAttribute, uint32_t _samplerCount )
    {
        m_name = _name;
        m_vertexShader = _vertexShader;
        m_fragmentShader = _fragmentShader;
        m_vertexAttribute = _vertexAttribute;
        m_samplerCount = _samplerCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgram::compile()
    {
        if( m_samplerCount > MENGINE_MAX_TEXTURE_STAGES )
        {
            LOGGER_ERROR("program '%s' don't support sampler count %d max %d"
                , m_name.c_str()
                , m_samplerCount
                , MENGINE_MAX_TEXTURE_STAGES
                );

            return false;
        }

        GLuint program;
        GLCALLR( program, glCreateProgram, () );

        if( program == 0 )
        {
            LOGGER_ERROR("invalid create program '%s'"
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

        GLCALL( glLinkProgram, (program) );

        GLint linked;
        GLCALL( glGetProgramiv, (program, GL_LINK_STATUS, &linked) );

        if( linked == GL_FALSE )
        {
            GLchar errorLog[1024] = { 0 };
            GLCALL( glGetProgramInfoLog, (program, 1023, NULL, errorLog) );

            LOGGER_ERROR("program '%s' linking error '%s'"
                , m_name.c_str()
                , errorLog
                );

            return false;
        }

        if( m_vertexAttribute->compile( program ) == false )
        {
            LOGGER_ERROR( "invalid program '%s' bind vertex attribute"
                , m_name.c_str()
            );

            return false;
        }

        const char * matrix_uniforms[] = {"viewMatrix", "projectionMatrix", "worldMatrix", "vpMatrix", "wvpMatrix"};

        for( uint32_t i = 0; i != EPML_MAX_COUNT; ++i )
        {
            const char * uniform = matrix_uniforms[i];

            GLint location;
            GLCALLR( location, glGetUniformLocation, (program, uniform) );

            m_matrixLocation[i] = location;
        }
        
        for( uint32_t index = 0; index != m_samplerCount; ++index )
        {
            char samplerVar[16];
            sprintf( samplerVar, "inSampler%u", index );

            GLint location;
            GLCALLR( location, glGetUniformLocation, (program, samplerVar) );

            if( location == -1 )
            {
                LOGGER_ERROR( "program '%s' not found uniform sampler '%s'"
                    , m_name.c_str()
                    , samplerVar
                );

                return false;
            }

            m_samplerLocation[index] = location;
        }

        m_program = program;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgram::release()
    {
        for( uint32_t i = 0; i != EPML_MAX_COUNT; ++i )
        {
            m_matrixLocation[i] = -1;
        }

        for( uint32_t i = 0; i != MENGINE_MAX_TEXTURE_STAGES; ++i )
        {
            m_samplerLocation[i] = -1;
        }

        if( m_program != 0 )
        {
            GLCALL( glDeleteProgram, (m_program) );
            m_program = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgram::enable() const
    {
        GLCALL( glUseProgram, (m_program) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgram::disable() const
    {
        GLCALL( glUseProgram, (0) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgram::bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalWVPMatrix ) const
    {
        if( m_matrixLocation[EPML_VIEW] != -1 )
        {
            GLint location = m_matrixLocation[EPML_VIEW];

            GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, _viewMatrix.buff()) );
        }

        if( m_matrixLocation[EPML_PROJECTION] != -1 )
        {
            GLint location = m_matrixLocation[EPML_PROJECTION];

            GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, _projectionMatrix.buff()) );
        }

        if( m_matrixLocation[EPML_WORLD] != -1 )
        {
            GLint location = m_matrixLocation[EPML_WORLD];

            GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, _worldMatrix.buff()) );
        }

        if( m_matrixLocation[EPML_VIEW_PROJECTION] != -1 )
        {
            mt::mat4f vpMat = _viewMatrix * _projectionMatrix;

            GLint location = m_matrixLocation[EPML_VIEW_PROJECTION];

            GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, vpMat.buff()) );
        }

        if( m_matrixLocation[EPML_WORLD_VIEW_PROJECTION] != -1 )
        {
            mt::mat4f wvpMat = _viewMatrix * _projectionMatrix * _worldMatrix;

            GLint location = m_matrixLocation[EPML_WORLD_VIEW_PROJECTION];

            GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, _totalWVPMatrix.buff()) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgram::bindTexture( uint32_t _textureInd ) const
    {   
        if(_textureInd >= m_samplerCount )
        {
            LOGGER_ERROR("program '%s' invalid support sampler count %d max %d"
                , m_name.c_str()
                , _textureInd
                , m_samplerCount
                );

            return false;
        }

        int location = m_samplerLocation[_textureInd];

        GLCALL( glUniform1i, (location, _textureInd) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
