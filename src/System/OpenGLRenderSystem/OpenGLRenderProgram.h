#pragma once

#include "Interface/RenderSystemInterface.h"

#include "OpenGLRenderVertexShader.h"
#include "OpenGLRenderFragmentShader.h"
#include "OpenGLRenderVertexAttribute.h"

#include "OpenGLRenderExtension.h"

#include "Kernel/ServantBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EProgramMatrixLocation
    {
        EPML_VIEW = 0,
        EPML_PROJECTION,
        EPML_WORLD,
        EPML_VIEW_PROJECTION,
        EPML_WORLD_VIEW_PROJECTION,
        EPML_MAX_COUNT
    };
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderProgram
        : public ServantBase<RenderProgramInterface>
    {
    public:
        OpenGLRenderProgram();
        ~OpenGLRenderProgram() override;

    public:
        GLuint getProgramId() const;

    public:
        const ConstString & getName() const override;

    public:
        RenderVertexAttributeInterfacePtr getVertexAttribute() const override;

    public:
        RenderFragmentShaderInterfacePtr getFragmentShader() const override;
        RenderVertexShaderInterfacePtr getVertexShader() const override;

    public:
        bool initialize( const ConstString & _name, const OpenGLRenderVertexShaderPtr & _vertexShader, const OpenGLRenderFragmentShaderPtr & _fragmentShader, const OpenGLRenderVertexAttributePtr & _vertexAttribute, uint32_t _samplerCount );

    public:
        bool compile();
        void release();

    public:
        bool enable() const;
        void disable() const;

        void bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix ) const;
        bool bindTexture( uint32_t _textureInd ) const;

    protected:
        ConstString m_name;

        GLuint m_program;

        OpenGLRenderVertexShaderPtr m_vertexShader;
        OpenGLRenderFragmentShaderPtr m_fragmentShader;
        OpenGLRenderVertexAttributePtr m_vertexAttribute;

        uint32_t m_samplerCount;
        
        GLint m_matrixLocation[EPML_MAX_COUNT];
        GLint m_samplerLocation[MENGINE_MAX_TEXTURE_STAGES];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<OpenGLRenderProgram> OpenGLRenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
}

