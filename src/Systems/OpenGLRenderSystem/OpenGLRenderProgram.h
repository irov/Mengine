#pragma once

#include "Interface/RenderSystemInterface.h"

#include "OpenGLRenderVertexShader.h"
#include "OpenGLRenderFragmentShader.h"
#include "OpenGLRenderVertexAttribute.h"

#include "OpenGLRenderExtension.h"
#include "OpenGLRenderResourceHandler.h"

#include "Kernel/Factorable.h"
#include "Kernel/CompilableReference.h"

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
        : public RenderProgramInterface
        , public OpenGLRenderResourceHandler
        , public CompilableReference
        , public Factorable
    {
        DECLARE_FACTORABLE( OpenGLRenderProgram );

    public:
        OpenGLRenderProgram();
        ~OpenGLRenderProgram() override;

    public:
        GLuint getProgramId() const;

    public:
        const ConstString & getName() const override;

    public:
        const RenderVertexAttributeInterfacePtr & getVertexAttribute() const override;

    public:
        const RenderFragmentShaderInterfacePtr & getFragmentShader() const override;
        const RenderVertexShaderInterfacePtr & getVertexShader() const override;

    public:
        bool initialize( const ConstString & _name, const OpenGLRenderVertexShaderPtr & _vertexShader, const OpenGLRenderFragmentShaderPtr & _fragmentShader, const OpenGLRenderVertexAttributePtr & _vertexAttribute, uint32_t _samplerCount );
        void finalize();

    public:
        bool _compile() override;
        void _release() override;

    public:
        bool enable() const;
        void disable() const;

        void bindMatrix( const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalWVPMatrix ) const;
        bool bindTexture( uint32_t _textureInd ) const;

    public:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        ConstString m_name;

        GLuint m_programId;

        OpenGLRenderVertexShaderPtr m_vertexShader;
        OpenGLRenderFragmentShaderPtr m_fragmentShader;
        OpenGLRenderVertexAttributePtr m_vertexAttribute;

        uint32_t m_samplerCount;

        GLint m_matrixLocation[EPML_MAX_COUNT];
        GLint m_samplerLocation[MENGINE_MAX_TEXTURE_STAGES];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderProgram, RenderProgramInterface> OpenGLRenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
}

