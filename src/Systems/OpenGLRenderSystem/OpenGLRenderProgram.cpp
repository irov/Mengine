#include "OpenGLRenderProgram.h"

#include "OpenGLRenderErrorHelper.h"

#include "Kernel/Logger.h"

#include "Config/StdIO.h"
#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgram::OpenGLRenderProgram()
        : m_programId( 0 )
        , m_samplerCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderProgram::~OpenGLRenderProgram()
    {
        MENGINE_ASSERTION_FATAL( m_programId == 0, "program '%u' is not released"
            , m_programId
        );
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderProgram::getProgramId() const
    {
        return m_programId;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & OpenGLRenderProgram::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexAttributeInterfacePtr & OpenGLRenderProgram::getVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderFragmentShaderInterfacePtr & OpenGLRenderProgram::getFragmentShader() const
    {
        return m_fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexShaderInterfacePtr & OpenGLRenderProgram::getVertexShader() const
    {
        return m_vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgram::initialize( const ConstString & _name, const OpenGLRenderVertexShaderPtr & _vertexShader, const OpenGLRenderFragmentShaderPtr & _fragmentShader, const OpenGLRenderVertexAttributePtr & _vertexAttribute, uint32_t _samplerCount )
    {
        Algorithm::fill_n( m_matrixLocation, EPML_MAX_COUNT, -1 );
        Algorithm::fill_n( m_samplerLocation, MENGINE_MAX_TEXTURE_STAGES, -1 );

        m_name = _name;
        m_vertexShader = _vertexShader;
        m_fragmentShader = _fragmentShader;
        m_vertexAttribute = _vertexAttribute;
        m_samplerCount = _samplerCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgram::finalize()
    {
        MENGINE_ASSERTION_FATAL( this->getCompileReferenceCount() == 0, "program '%s' is used"
            , m_name.c_str()
        );

        m_vertexShader = nullptr;
        m_fragmentShader = nullptr;
        m_vertexAttribute = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgram::_compile()
    {
        MENGINE_ASSERTION_FATAL( m_samplerCount <= MENGINE_MAX_TEXTURE_STAGES, "program '%s' don't support sampler count %d max %d"
            , m_name.c_str()
            , m_samplerCount
            , MENGINE_MAX_TEXTURE_STAGES
        );

        GLuint programId;
        MENGINE_GLCALLR( programId, glCreateProgram, () );

        if( programId == 0 )
        {
            LOGGER_ERROR( "invalid create program '%s'"
                , m_name.c_str()
            );

            return false;
        }

        if( m_vertexShader != nullptr )
        {
            if( m_vertexShader->compile() == false )
            {
                LOGGER_ERROR( "invalid create program '%s' invalid compile vertex shader '%s'"
                    , m_name.c_str()
                    , m_vertexShader->getName().c_str()
                );

                return false;
            }

            m_vertexShader->attach( programId );
        }

        if( m_fragmentShader != nullptr )
        {
            if( m_fragmentShader->compile() == false )
            {
                LOGGER_ERROR( "invalid create program '%s' invalid compile fragment shader '%s'"
                    , m_name.c_str()
                    , m_fragmentShader->getName().c_str()
                );

                return false;
            }

            m_fragmentShader->attach( programId );
        }

        MENGINE_GLCALL( glLinkProgram, (programId) );

        GLint linked;
        MENGINE_GLCALL( glGetProgramiv, (programId, GL_LINK_STATUS, &linked) );

        if( linked == GL_FALSE )
        {
            GLchar errorLog[1024] = {0};
            MENGINE_GLCALL( glGetProgramInfoLog, (programId, 1023, NULL, errorLog) );

            LOGGER_ERROR( "program '%s' linking error '%s'"
                , m_name.c_str()
                , errorLog
            );

            return false;
        }

        if( m_vertexAttribute->compile( programId ) == false )
        {
            LOGGER_ERROR( "invalid program '%s' bind vertex attribute"
                , m_name.c_str()
            );

            return false;
        }

        const Char * matrix_uniforms[] = {"viewMatrix", "projectionMatrix", "worldMatrix", "vpMatrix", "wvpMatrix"};

        for( uint32_t index = 0; index != EPML_MAX_COUNT; ++index )
        {
            const Char * uniform = matrix_uniforms[index];

            GLint location;
            MENGINE_GLCALLR( location, glGetUniformLocation, (programId, uniform) );

            m_matrixLocation[index] = location;
        }

        for( uint32_t index = 0; index != m_samplerCount; ++index )
        {
            Char samplerVar[16] = {'\0'};
            MENGINE_SNPRINTF( samplerVar, 15, "inSampler%u", index );

            GLint location;
            MENGINE_GLCALLR( location, glGetUniformLocation, (programId, samplerVar) );

            MENGINE_ASSERTION_FATAL( location != -1, "program '%s' not found uniform sampler '%s'"
                , m_name.c_str()
                , samplerVar
            );

            m_samplerLocation[index] = location;
        }

        m_programId = programId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgram::_release()
    {
        Algorithm::fill_n( m_matrixLocation, EPML_MAX_COUNT, -1 );
        Algorithm::fill_n( m_samplerLocation, MENGINE_MAX_TEXTURE_STAGES, -1 );

        if( m_programId != 0 )
        {
            MENGINE_GLCALL( glDeleteProgram, (m_programId) );
            m_programId = 0;
        }

        if( m_vertexShader != nullptr )
        {
            m_vertexShader->release();
        }

        if( m_fragmentShader != nullptr )
        {
            m_fragmentShader->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgram::enable() const
    {
        MENGINE_GLCALL( glUseProgram, (m_programId) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgram::disable() const
    {
        MENGINE_GLCALL( glUseProgram, (0) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgram::bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalWVPMatrix ) const
    {
        if( m_matrixLocation[EPML_VIEW] != -1 )
        {
            GLint location = m_matrixLocation[EPML_VIEW];

            MENGINE_GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, _viewMatrix.buff()) );
        }

        if( m_matrixLocation[EPML_PROJECTION] != -1 )
        {
            GLint location = m_matrixLocation[EPML_PROJECTION];

            MENGINE_GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, _projectionMatrix.buff()) );
        }

        if( m_matrixLocation[EPML_WORLD] != -1 )
        {
            GLint location = m_matrixLocation[EPML_WORLD];

            MENGINE_GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, _worldMatrix.buff()) );
        }

        if( m_matrixLocation[EPML_VIEW_PROJECTION] != -1 )
        {
            mt::mat4f vpMat = _viewMatrix * _projectionMatrix;

            GLint location = m_matrixLocation[EPML_VIEW_PROJECTION];

            MENGINE_GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, vpMat.buff()) );
        }

        if( m_matrixLocation[EPML_WORLD_VIEW_PROJECTION] != -1 )
        {
            GLint location = m_matrixLocation[EPML_WORLD_VIEW_PROJECTION];

            MENGINE_GLCALL( glUniformMatrix4fv, (location, 1, GL_FALSE, _totalWVPMatrix.buff()) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgram::bindTexture( uint32_t _textureInd ) const
    {
        MENGINE_ASSERTION_FATAL( _textureInd < m_samplerCount, "program '%s' invalid support sampler count %d max %d"
            , m_name.c_str()
            , _textureInd
            , m_samplerCount
        );

        GLint location = m_samplerLocation[_textureInd];

        MENGINE_GLCALL( glUniform1i, (location, _textureInd) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderProgram::onRenderReset()
    {
        this->_release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderProgram::onRenderRestore()
    {
        bool successful = this->_compile();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
