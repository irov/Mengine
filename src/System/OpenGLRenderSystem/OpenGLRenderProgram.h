#   pragma once

#   include "Interface/RenderSystemInterface.h"

#   include "OpenGLRenderShader.h"
#   include "OpenGLRenderVertexShader.h"
#   include "OpenGLRenderFragmentShader.h"

#   include "OpenGLRenderHeader.h"

namespace Menge
{
    class OpenGLRenderProgram
        : public RenderProgramInterface
    {
    public:
        OpenGLRenderProgram();
        ~OpenGLRenderProgram();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        const ConstString & getName() const override;

    public:
        RenderFragmentShaderInterfacePtr getFragmentShader() const override;
        RenderVertexShaderInterfacePtr getVertexShader() const override;

    public:
        bool initialize( const ConstString & _name, const OpenGLRenderVertexShaderPtr & _vertexShader, const OpenGLRenderFragmentShaderPtr & _fragmentShader, uint32_t _samplerCount );
        bool compile();

    public:
        void enable() const;
        void disable() const;

        void bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix ) const;
        void bindTexture( uint32_t _textureInd ) const;

    protected:
        GLuint createShader_( GLenum type, const char * _source );

    protected:
        ServiceProviderInterface * m_serviceProvider;

        ConstString m_name;

        GLuint m_program;

        OpenGLRenderVertexShaderPtr m_vertexShader;
        OpenGLRenderFragmentShaderPtr m_fragmentShader;

        uint32_t m_samplerCount;
        
        mutable mt::mat4f m_mvpMat;

        int m_transformLocation;
        int m_samplerLocation[MENGE_MAX_TEXTURE_STAGES];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<OpenGLRenderProgram> OpenGLRenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
}   // namespace Menge

